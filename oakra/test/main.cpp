#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include "oakra.h"


void main() {

	OAKRA_init();
	//output
	//OAKRA_Module_OutputNull *output = new OAKRA_Module_OutputNull();
	//output->setSourceCount(1);

	OAKRA_Module_OutputDS *dsout = new OAKRA_Module_OutputDS();
	dsout->start(0);
	dsout->beginThread();

	OAKRA_Format dummy;
	dummy.rate = 44100;
	dummy.channels = 2;
	dummy.format = OAKRA_S16;
	dummy.size = 4;
	OAKRA_Module_OutputWinMM *output = new OAKRA_Module_OutputWinMM(8,dummy);
	output->start();
	output->beginThread();
	
	


	//sources
	//OAKRA_Module_InputSine *sine = new OAKRA_Module_InputSine(44100,220.0);
	//OAKRA_Module_1S16_to_2S16 *_glue = new OAKRA_Module_1S16_to_2S16();
	//_glue->source = sine;
	//OAKRA_Module_InputSine *sine2 = new OAKRA_Module_InputSine(44100,330.0);
	//OAKRA_Module_1S16_to_2S16 *_glue2 = new OAKRA_Module_1S16_to_2S16();
	//_glue2->source = sine2;
	//OAKRA_Module_2S16_to_2S16_Mixer *mixer = new OAKRA_Module_2S16_to_2S16_Mixer();
	//mixer->shift = 1;
	//mixer->setSourceCount(2);
	//mixer->setSource(0,_glue);
	//mixer->setSource(1,_glue2);

	//OAKRA_Module_2xS32_to_2xS16_Compressor *compressor = new OAKRA_Module_2xS32_to_2xS16_Compressor();
	//compressor->source = mixer;

	//output->source = compressor;
	//output->start();

	//OAKRA_Format dumbformat = {2,OAKRA_S16,44100,4};
	//OAKRA_Module_InputDUMB *dumb = new OAKRA_Module_InputDUMB(dumbformat);
	//dumb->load("Z:\\media\\music\\mods\\artists\\acetip\\future blue.mod");
	//dumb->load("Z:\\media\\music\\mods\\artists\\fleshbrain\\dominion.mod");
	//dumb->load("Z:\\media\\music\\mods\\artists\\fleshbrain\\stardust.mod");


	
	//ogg->load("Z:\\media\\music\\private\\troupe\\opal.ogg");
	//output->source = ogg;
	//output->start();

	/*OAKRA_Module_ResInteger_Base *ff = new OAKRA_Module_ResInteger_16S<2,32>();
	ff->setSkip(15);
	ff->source = ogg;
	output->source = ff;
	output->start();*/

	OAKRA_Module_InputOGG *ogg = new OAKRA_Module_InputOGG();
	OAKRA_Module_InputWAV *wav = new OAKRA_Module_InputWAV();
	OAKRA_Module_InputMP3 *mp3 = new OAKRA_Module_InputMP3();
	OAKRA_Format dumbFormat = { 2,OAKRA_S16,48000,4 };
	OAKRA_Module_InputDUMB *dumb = new OAKRA_Module_InputDUMB( dumbFormat);
	OAKRA_Module_MusicPlayer *mp = new OAKRA_Module_MusicPlayer();
	mp->ogg = ogg;
	mp->wav = wav;
	mp->mp3 = mp3;
	mp->dumb = dumb;
	//mp->load(true,"Z:\\media\\music\\private\\troupe\\opal.ogg");
	//mp->load(true,"Z:\\resources\\sounds\\crap\\8BIT\\ANIMALS\\SG0129.WAV");
	//mp->load(false,"Z:\\media\\music\\mods\\artists\\fleshbrain\\stardust.mod");
	//mp->load(true,"Z:\\resources\\sounds\\crap\\16BIT\\SNDFX\\HE05ABP.WAV");
	
	//mp->load(true,"Z:\\resources\\sounds\\crap\\8BIT\\ANIMALS\\SG0129.WAV");

	//OAKRA_CachedSound sfx;
	//sfx.mp->wav = wav;
	//sfx.load("D:\\0509\\107 - Gonzalez's (Gato's) Song.wav");

	//mp->load(false,"Z:\\projects\\active\\bsdv32\\redist\\res\\music\\X01C.IT");
	mp->load(false,"Z:\\media\\music\\artists\\Exodus\\2005 ^ Shovel Headed Kill Machine\\01 - Raze.mp3");
	OAKRA_Module_ResInteger_Base *ff = new OAKRA_Module_ResInteger_16S<2,32>();
	ff->setSkip(0);
	ff->source = mp;


	//mp->load(true,"D:\\0509\\107 - Gonzalez's (Gato's) Song.wav");
	
	
	//mp->setLoopInfo(42352,468495);

	//OAKRA_Voice *musicvoice = dsout->getVoice(mp->queryFormat());
	//OAKRA_Voice *musicvoice = dsout->getVoice(sfx.queryFormat());
	//musicvoice->setVol(255);
	//musicvoice->source = ff;

	OAKRA_Voice *dummyvoice = output->getVoice(mp->queryFormat());
	dummyvoice->setSource(ff);
	



	//OAKRA_Module_InputOGG *ogg = new OAKRA_Module_InputOGG();
	//ogg->load();
	//OAKRA_BasicModule *interp = new OAKRA_Module_ResInterp_S16<2,32>(48000,44100);
	//OAKRA_Module_ResInteger_Base *ff = new OAKRA_Module_ResInteger_16S<2,32>();
	//ff->setSkip(0);
	//interp->source = ogg;
	//ff->source = interp;
	//output->source = ff;

	

	//OAKRA_Module_InputWAV *wav = new OAKRA_Module_InputWAV();
	//wav->format.channels = 2;
	//wav->format.rate = 44100;
	//wav->format.format = OAKRA_S16;
	//wav->format.size = 4;
	//wav->loadQuick("d:\\0509\\Coroner - About Life.wav");
	//output->source = wav;
	//output->start();

	//OAKRA_Module_InputMP3 *mp3 = new OAKRA_Module_InputMP3();
	//mp3->load("d:\\0509\\Coq_Roq_-_Bob_Your_Head.mp3");
	//mp3->load("z:\\resources\\sounds\\funny\\Tech_Agent_-_Apple_Tech_Support_-_00_-_Americas_Most_Wanted.mp3");
	//mp3->load("z:\\resources\\sounds\\funny\\kvi_aircheck_031003.mp3");
	//mp3->load("d:\\0509\\sexwithwomens(instrumental).mp3");

	//OAKRA_Module_InputWAV *wav = new OAKRA_Module_InputWAV();
	//wav->format.channels = 1;
	//wav->format.rate = 44100;
	//wav->format.format = OAKRA_S16;
	//wav->format.size = 2;
	//wav->load("d:\\0509\\Coroner - About Life-mono.wav");

	//OAKRA_Module_VolPanner_1S16_to_2S32 *test = new OAKRA_Module_VolPanner_1S16_to_2S32();
	//test->source = wav;
	//test->pan = -255;
	//test->vol = 100;
	//OAKRA_Module_2S32_to_2S16 *blah = new OAKRA_Module_2S32_to_2S16();
	//blah->source = test;




	printf("OAKRA Memory used (sort of): %d\n",OAKRA_Manager::getMemoryUsage());

	int start = timeGetTime();
	int switched = 0;
	for(;;) {
		//output->update();
		//dsout->update();
		Sleep(10);
	}

}

