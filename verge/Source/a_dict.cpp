/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#include "xerxes.h"
#include "a_dict.h"

std::string dict::GetString(std::string key) {
	const std::map<std::string,std::string>::iterator i = data.find(key);
	if(i == data.end()) {
		se->Error("dict::GetString: No such key (%s)", key.c_str());
		return ""; // should never happen
	}
	return i->second;
}

void dict::SetString(std::string key, std::string val) {
	data[key] = val;
}

int dict::ContainsString(std::string key) {
	return data.count(key);
}

void dict::RemoveString(std::string key) {
	const std::map<std::string,std::string>::iterator i = data.find(key);
	if(i == data.end()) {
		se->Error("dict::RemoveString: No such key (%s)", key.c_str());
		return; // should never happen
	}
	data.erase(i);
}

int dict::Size() {
	return data.size();
}

std::string dict::ListKeys(std::string separator)
{
	std::string s = "";

	for(std::map<std::string,std::string>::iterator i = data.begin();
		i != data.end();
		i++)
	{
		s += std::string(i->first) + separator;
	}

	return s;
}