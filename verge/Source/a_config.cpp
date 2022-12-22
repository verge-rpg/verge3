/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	a_common.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

#define KEYSIZE 25
#define VALUESIZE 80
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

std::string cfg_filename;

struct CompareIgnoreCase
{
	using is_transparent = void;

    bool operator()(const StringRef& lhs, const StringRef& rhs) const { return strcasecmp(lhs, rhs) < 0; }
    bool operator()(const StringRef& lhs, std::string_view rhs) const { return strcasecmp(lhs.view(), rhs) < 0; }
    bool operator()(std::string_view lhs, const StringRef& rhs) const { return strcasecmp(lhs, rhs.view()) < 0; }
};

using ConfigMap = std::map<StringRef, StringRef, CompareIgnoreCase>;
ConfigMap cfg_dict;

/***************************** code *****************************/

void cfg_Init(std::string_view fn)
{
	char temp[1024];
	char line[1024];
	char value[VALUESIZE + 1];

	cfg_filename.reserve(255);

#if defined(__APPLE__) || defined(__IPHONE__) || defined(__LINUX__)
	getcwd(temp, sizeof(temp));
	cfg_filename += temp;
	cfg_filename += '/';
	cfg_filename += fn.data();
#elif __PSP__
	//mbg 9/5/05
	//make the bold assumption that PWD has not been changed
	//and never will be
	//better handling might be required in the future
	cfg_filename = fn;
#elif __WII__
	//likewise above
	cfg_filename = fn;
#elif __EMSCRIPTEN__
	cfg_filename = fn;
#else 
	GetCurrentDirectory(sizeof(temp), temp);
	cfg_filename += temp;
	cfg_filename += '/';
	cfg_filename += fn.data();
#endif

	FILE *f = FileOpen(cfg_filename.c_str(), "r");
	if (!f) return;

	while (!feof(f))
	{
		fgets(line, sizeof(line), f);
		// skip whitespace, read non-whitespace string then skip more whitespace
		int num_read = 0;
		temp[0] = 0;
		sscanf(line," %s %n", temp, &num_read); // capture length read
		if (strlen(temp) > 0) // check for blank line
		{
			if(temp[0] == '#' || (temp[0] == '/' && temp[1] == '/'))
			{
				// comment - ignore until end of line
				continue;
			}

			if (strlen(temp) >= KEYSIZE)
				err("cfg_Init(), key too big");

			value[0] = 0; // ensure null-terminated value on error
			// now read non-whitespace string after key, up to VALUESIZE
			sscanf(line + num_read, " %" TOSTRING(VALUESIZE) "s", value);
			strclean(value);
			
			cfg_dict[temp] = value;
		}
	}
	
	fclose(f);
}

bool cfg_KeyPresent(std::string_view key)
{
	const auto i = cfg_dict.find(key);
	return i != cfg_dict.end();
}

int cfg_GetIntKeyValue(std::string_view key)
{
	const auto i = cfg_dict.find(key);
	return i != cfg_dict.end()
		? atoi(i->second.c_str())
		: -1;
}

CStringRef cfg_GetKeyValue(std::string_view key)
{
	const auto i = cfg_dict.find(key);
	return i != cfg_dict.end()
		? i->second
		: empty_string;
}

std::vector<std::string> cfg_Tokenize(std::string_view key, char delim)
{
	const auto value = cfg_GetKeyValue(key);
	auto cp = value.c_str();
	auto last = value.c_str();

	std::vector<std::string> tokens;
	while (true)
	{
		while (*cp && *cp != delim)
		{
			cp++;
		}

		tokens.push_back(std::string(last, cp - last));

		if (!*cp)
		{
			return tokens;
		}

		cp++;
		last = cp;
	}
}

void cfg_SetKeyValue(CStringRef key, CStringRef value)
{
	cfg_dict[key] = value;
}

void cfg_SetDefaultKeyValue(CStringRef key, CStringRef value)
{
	if (cfg_KeyPresent(key.view())) return;
	cfg_SetKeyValue(key, value);
}

void cfg_DeleteKey(std::string_view key)
{
	const auto i = cfg_dict.find(key);
	if (i != cfg_dict.end())
	{
		cfg_dict.erase(i);
	}
}

void cfg_WriteConfig()
{
	FILE *f = FileOpen(cfg_filename.c_str(), "w");
	if (!f) err("cfg_WriteConfig(), could not open config file!");

	for (const auto& i : cfg_dict)
	{
		fprintf(f, "%s %s\n", i.first.c_str(), i.second.c_str());
	}

	FileCloseAndFlush(f);
}
