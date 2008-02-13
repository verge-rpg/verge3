/****************************************************************
	xerxes engine
	a_common.cpp
 ****************************************************************/

#include "a_config.h"
#include "a_common.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <crtdbg.h>

void err(char *errstring);

/***************************** data *****************************/

#define KEYSIZE 20
#define VALUESIZE 80

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
	char temp[80];
	config_key *cf;

	GetCurrentDirectory(255, cfgfn);
	strcat(cfgfn, va("\\%s",fn));
	cf_top = 0;

	FILE *f = fopen(fn, "r");
	if (!f) return;

	cf = 0;
	while (!feof(f))
	{
		fscanf(f,"%s%*c", temp);
		if (strlen(temp)>1 && isletter(temp[0]) && !feof(f)) 
		{
			if (strlen(temp)>=KEYSIZE)
				err("cfg_Init(), key too big");
			if (!cf) 
				cf = cf_top = (config_key *) malloc(sizeof config_key);
			else 
			{	
				cf->next = (config_key *) malloc(sizeof config_key);
				cf = cf->next;
			}
			
			strcpy(cf->keyname, temp);
			fgets(cf->value, VALUESIZE-1, f);
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


void cfg_SetKeyValue(char *key, char *value)
{
	_ASSERTE(strlen(key)<KEYSIZE);
	_ASSERTE(strlen(value)<VALUESIZE);

	if (!cf_top)
	{
		cf_top = (config_key *) malloc(sizeof config_key);
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
	last->next = (config_key *) malloc(sizeof config_key);
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
