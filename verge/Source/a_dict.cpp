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

string dict::GetString(string key) {
	const std::map<string,string>::iterator i = data.find(key);
	if(i == data.end()) {
		vc->vcerr("dict::GetString: No such key (%s)", key.c_str());
		return ""; // should never happen
	}
	return i->second;
}

void dict::SetString(string key, string val) {
	data[key] = val;
}

int dict::ContainsString(string key) {
	return data.count(key);
}

void dict::RemoveString(string key) {
	const std::map<string,string>::iterator i = data.find(key);
	if(i == data.end()) {
		vc->vcerr("dict::RemoveString: No such key (%s)", key.c_str());
		return; // should never happen
	}
	data.erase(i);
}

int dict::Size() {
	return data.size();
}

string dict::ListKeys(string separator)
{
	string s = "";

	for(std::map<string,string>::iterator i = data.begin();
		i != data.end();
		i++)
	{
		s += string(i->first) + separator;
	}

	return s;
}