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
#define KEYSIZE_STR "25"
#define VALUESIZE 80
#define VALUESIZE_STR "80"

char cfgfn[255];

struct config_key
{
	char keyname[KEYSIZE];
	char value[VALUESIZE];
	config_key *next;
};

config_key *cf_top;

/***************************** code *****************************/


void cfg_Init(char *fn)
{
	char temp[1024];
	char line[1024];

	config_key *cf;
#ifdef __APPLE__
	getcwd(cfgfn,255);
	strcat(cfgfn, va("/%s", fn));
#elif __LINUX__
	getcwd(cfgfn,255);
	strcat(cfgfn, va("/%s", fn));
#elif __PSP__
	//mbg 9/5/05
	//make the bold assumption that PWD has not been changed
	//and never will be
	//better handling might be required in the future
	sprintf(cfgfn,"%s",fn);
#else
	GetCurrentDirectory(255, cfgfn);
	strcat(cfgfn, va("\\%s",fn));
#endif
	cf_top = 0;

	FILE *f = fopen(fn, "r");
	if (!f) return;

	cf = 0;
	while (!feof(f))
	{
		fgets(line, sizeof(line), f);
		// skip whitespace, read non-whitespace string then skip more whitespace
		int num_read = 0;
		sscanf(line," %s %n", temp, &num_read); // capture length read
		if (strlen(temp) > 0) // check for blank line
		{
			if(temp[0] == '#' || (temp[0] == '/' && temp[1] == '/')) {
				// comment - ignore until end of line
				continue;
			}

			if (strlen(temp)>=KEYSIZE)
				err("cfg_Init(), key too big");
			if (!cf)
				cf = cf_top = (config_key *) malloc(sizeof (config_key));
			else
			{
				cf->next = (config_key *) malloc(sizeof (config_key));
				cf = cf->next;
			}

			strcpy(cf->keyname, temp);
			cf->value[0] = '\0'; // ensure null-terminated value on error
			// now read non-whitespace string after key, up to VALUESIZE
			sscanf(line + num_read, " %" VALUESIZE_STR "s", cf->value);
			strclean(cf->value);
			cf->next = 0;
		}
	}
	fclose(f);
}


bool cfg_KeyPresent(char *key)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	config_key *cf = cf_top;
	while (cf)
	{
		if (!strcasecmp(key, cf->keyname))
			return true;
		cf = cf->next;
	}
	return false;
}


int cfg_GetIntKeyValue(char *key)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	config_key *cf = cf_top;
	while (cf)
	{
		if (!strcasecmp(key, cf->keyname))
			return atoi(cf->value);
		cf = cf->next;
	}
	return -1;
}


char *cfg_GetKeyValue(char *key)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	config_key *cf = cf_top;
	while (cf)
	{
		if (!strcasecmp(key, cf->keyname))
			return cf->value;
		cf = cf->next;
	}
	return NULL;
}

std::vector<std::string> cfg_Tokenize(char *key, char delim) {
	char *value = cfg_GetKeyValue(key);
	char *cp = value, *last = value;
	std::vector<std::string> tokens;
	for(;;) {
		while(*cp && *cp != delim) cp++;
		tokens.push_back(std::string(last,cp-last));
		if(!*cp) return tokens;
		cp++; last=cp;
	}
}

void cfg_SetKeyValue(char *key, char *value)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	_ASSERTE(strlen(value)<VALUESIZE);

	if (!cf_top)
	{
		cf_top = (config_key *) malloc(sizeof (config_key));
		strcpy(cf_top->keyname, key);
		strcpy(cf_top->value, value);
		cf_top->next = 0;
		return;
	}

	config_key *cf = cf_top, *last = cf_top;
	while (cf)
	{
		if (!strcasecmp(key, cf->keyname))
		{
			strcpy(cf->value, value);
			return;
		}
		last = cf;
		cf = cf->next;
	}
	last->next = (config_key *) malloc(sizeof (config_key));
	cf = last->next;
	strcpy(cf->keyname, key);
	strcpy(cf->value, value);
	cf->next = 0;
}


void cfg_SetDefaultKeyValue(char *key, char *value)
{
	if (cfg_KeyPresent(key)) return;
	cfg_SetKeyValue(key, value);
}


void cfg_DeleteKey(char *key)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	config_key *store, *last = 0, *cf = cf_top;
	while (cf)
	{
		if (!strcasecmp(key, cf->keyname))
		{
			store = cf->next;
			free(cf);
			if (last) last->next = store;
			else cf_top = store;
			return;
		}
		last = cf;
		cf = cf->next;
	}
	return;
}


void cfg_WriteConfig()
{
	bool first = true;
	config_key *cf = cf_top;
	FILE *f = fopen(cfgfn, "w");
	if (!f) err("cfg_WriteConfig(), could not open config file!");

	while (cf)
	{
		if (!first) fprintf(f,"\n");
		fprintf(f, "%s %s", cf->keyname, cf->value);
		cf = cf->next;
		first = false;
	}
	fclose(f);
}
