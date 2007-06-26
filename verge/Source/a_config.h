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

void cfg_Init(char *fn);
bool cfg_KeyPresent(char *key);
void cfg_SetKeyValue(char *key, char *value);
void cfg_SetDefaultKeyValue(char *key, char *value);
void cfg_DeleteKey(char *key);
void cfg_WriteConfig();
char *cfg_GetKeyValue(char *key);
int cfg_GetIntKeyValue(char *key);
std::vector<std::string> cfg_Tokenize(char *key, char delim);

#endif
