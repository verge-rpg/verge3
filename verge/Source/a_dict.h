/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef DICT_H
#define DICT_H

#include <map>

class dict {
	public:
		CStringRef GetString(CStringRef key);
		void SetString(CStringRef key, CStringRef val);
		int ContainsString(CStringRef key);
		void RemoveString(CStringRef key);
		int Size() { return data.size(); }
		StringRef ListKeys(CStringRef separator);
	private:
		typedef std::map<StringRef,StringRef> TDict;
		TDict data;
};

#endif