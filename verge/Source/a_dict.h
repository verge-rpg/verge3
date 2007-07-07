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
		std::string GetString(std::string key);
		void SetString(std::string key, std::string val);
		int ContainsString(std::string key);
		void RemoveString(std::string key);
		int Size();
		std::string ListKeys(std::string separator);
	private:
		std::map<std::string,std::string> data;
};

#endif