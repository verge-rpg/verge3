#ifndef CONFIG_H
#define CONFIG_H

void cfg_Init(char *fn);
bool cfg_KeyPresent(char *key);
void cfg_SetKeyValue(char *key, char *value);
void cfg_SetDefaultKeyValue(char *key, char *value);
void cfg_DeleteKey(char *key);
void cfg_WriteConfig();
char *cfg_GetKeyValue(char *key);

#endif