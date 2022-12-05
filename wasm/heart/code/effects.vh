/**********************************
********Effect system library.***** 
********By Tristan Michael.********
*******last updated 2/17/05********
******minor update by overkill*****
**********************************/

#define AFFECT_PLAYER	1
#define AFFECT_ENEMY	2

#define MAX_EFFECTS	2000

//--------------------------------------Effects
struct effect_type{
	int x1;
	int y1;
	int x2;
	int y2;
	int affectWho;
	string funct;
	int wild;
	int frame;
	int timeStamp;
	int frameStamp;
}

effect_type effect[MAX_EFFECTS];
int effects;
int currentEffect;

void renderEffects(){	
	for(currentEffect=0;currentEffect<effects;currentEffect++)
		callFunction(effect[currentEffect].funct);
}

void addEffect(int x1, int y1, int x2, int y2, int wild, int affectWho, string funct){
	effect[effects].x1=x1;
	effect[effects].y1=y1;
	effect[effects].x2=x2;
	effect[effects].y2=y2;
	effect[effects].funct=funct;
	effect[effects].wild=wild;
	effect[effects].frame=0;
	effect[effects].affectWho=affectWho;
	effect[effects].timeStamp=timer;
	effect[effects].frameStamp=timer;
	effects++;
}

void removeCurrentEffect(){
	effects--;
	effect[currentEffect].x1=effect[effects].x1;
	effect[currentEffect].y1=effect[effects].y1;
	effect[currentEffect].x2=effect[effects].x2;
	effect[currentEffect].y2=effect[effects].y2;
	effect[currentEffect].frame=effect[effects].frame;
	effect[currentEffect].funct=effect[effects].funct;
	effect[currentEffect].wild=effect[effects].wild;
	effect[currentEffect].affectWho=effect[effects].affectWho;
	effect[currentEffect].timeStamp=effect[effects].timestamp;
	effect[currentEffect].frameStamp=effect[effects].framestamp;
	currentEffect--;
}

//-----------------Layer 2
effect_type effect2[MAX_EFFECTS];
int effects2;

void renderEffects2(){	
	for(currentEffect=0;currentEffect<effects2;currentEffect++)
		callFunction(effect2[currentEffect].funct);
}

void addEffect2(int x1, int y1, int x2, int y2, int wild, int affectWho, string funct){
	effect2[effects2].x1=x1;
	effect2[effects2].y1=y1;
	effect2[effects2].x2=x2;
	effect2[effects2].y2=y2;
	effect2[effects2].funct=funct;
	effect2[effects2].wild=wild;
	effect2[effects2].frame=0;
	effect2[effects2].affectWho=affectWho;
	effect2[effects2].timeStamp=timer;
	effect2[effects2].frameStamp=timer;
	effects2++;
}

void removeCurrentEffect2(){
	effects2--;
	effect2[currentEffect].x1=effect2[effects2].x1;
	effect2[currentEffect].y1=effect2[effects2].y1;
	effect2[currentEffect].x2=effect2[effects2].x2;
	effect2[currentEffect].y2=effect2[effects2].y2;
	effect2[currentEffect].frame=effect2[effects2].frame;
	effect2[currentEffect].funct=effect2[effects2].funct;
	effect2[currentEffect].wild=effect2[effects2].wild;
	effect2[currentEffect].affectWho=effect2[effects2].affectWho;
	effect2[currentEffect].timeStamp=effect2[effects2].timestamp;
	effect2[currentEffect].frameStamp=effect2[effects2].framestamp;
	currentEffect--;
}