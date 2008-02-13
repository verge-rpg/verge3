#include <stdio.h>
#include <stdlib.h>
#include "a_config.h"


void err(char *errstring)
{
	printf("ERROR: subsystem error: {{%s}}",errstring);
	exit(0);
}

void chrmak_v5_v2(char *fname,int version_to_export);

int main(int argc, char **argv)
{
	FILE *inf=0;
	int version;

	printf("CHRMAK 5.10 02/29/04\n");
	printf("by matthew gambrell - zeromus@zeromus.org\n");
	printf("featuring ZLIB and CORONA\n");
	printf("NEW in this version:\n");
	printf("\t1. possibly broken support for version2 chrs!\n");
	printf("\t2. no support for chr alpha maps!\n");

	printf("\n");
	
	//make sure a filename operand is given
	if(argc==1)
	{
		printf("ERROR: no filename given to operate on.  please put it on the commandline.\n");
		exit(0);
	}

	//try to open file before we give it to a_config
	inf=fopen(argv[1],"rb");
	if(!inf)
	{
		printf("ERROR: couldn't open file. <%s>\n",argv[1]);
		exit(0);
	}
	fclose(inf);

	//read in the .mak file
	cfg_Init(argv[1]);
	
	//make sure we have a valid .mak file
	if(!cfg_KeyPresent("chr_version"))
	{
		printf("ERROR: chr_version missing. did you forget it or specify the wrong file? <%s>\n",argv[1]);
		exit(0);
	}
	version=atoi(cfg_GetKeyValue("chr_version"));

	//carry out the version-apropriate operation
	switch(version)
	{
	case 5:
	case 2:
		chrmak_v5_v2(argv[1],version);
		break;
	default:
		printf("ERROR: unsupported version number [%d]. <%s>\n",version,argv[1]);
		exit(0);
	}

	return 1;
}
