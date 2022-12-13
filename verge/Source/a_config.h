/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <string_view>

#include "a_string.h"

void cfg_Init(std::string_view fn);
bool cfg_KeyPresent(std::string_view key);
void cfg_SetKeyValue(CStringRef key, CStringRef value);
void cfg_SetDefaultKeyValue(CStringRef key, CStringRef value);
void cfg_DeleteKey(std::string_view key);
void cfg_WriteConfig();
CStringRef cfg_GetKeyValue(std::string_view key);
int cfg_GetIntKeyValue(std::string_view key);
std::vector<std::string> cfg_Tokenize(std::string_view key, char delim);

#endif
