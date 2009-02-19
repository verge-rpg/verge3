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

CStringRef dict::GetString(CStringRef key) {
	const TDict::iterator i = data.find(key);
	if(i == data.end()) {
		se->Error("dict::GetString: No such key (%s)", key.c_str());
		return empty_string; // should never happen
	}
	return i->second;
}

void dict::SetString(CStringRef key, CStringRef val) {
	data[key] = val;
}

int dict::ContainsString(CStringRef key) {
	return data.count(key);
}

void dict::RemoveString(CStringRef key) {
	const TDict::iterator i = data.find(key);
	if(i == data.end()) {
		se->Error("dict::RemoveString: No such key (%s)", key.c_str());
		return; // should never happen
	}
	data.erase(i);
}

StringRef dict::ListKeys(CStringRef separator)
{
	std::string s = "";

	for(TDict::iterator i = data.begin();
		i != data.end();
		i++)
	{
		s += i->first.str() + separator.str();
	}

	return s;
}