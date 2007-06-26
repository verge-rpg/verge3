#include "xerxes.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psputils.h>
#include <pspgu.h>
#include <psprtc.h>


/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


PSP_MODULE_INFO(APPNAME, 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list[262144];

struct SpriteVertex {
	short u, v;
	short pad;
	short x, y, z;
};

#define SPRITE_HFLIP 1
void psp_renderSprite(int flags, int x, int y, int w, int h) {
	log("%d %d %d %d %d\n",flags,x,y,w,h);
	SpriteVertex *vertices = (SpriteVertex*)sceGuGetMemory(2*sizeof(SpriteVertex));
	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	vertices[1].u = w;
	vertices[1].v = h;
	vertices[1].x = x+w;
	vertices[1].y = y+h;
	vertices[1].z = 0;
	if(flags & SPRITE_HFLIP) {
		vertices[0].u = w;
		vertices[1].u = 0;
	}
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_COLOR_4444|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vertices);
}


void *psp_showPage(image *framebuffer, int tyres) {

	//perhaps:
	//sync at the top, instead of the bottom,
	//so that the framerendering can go on while the engine
	//proceeds instead of having to wait in showpage

	//sceGuSync(0,0);

	//swap the R and B bitfields, goddammit
	/*int w = framebuffer->width;
	int p = framebuffer->pitch;
	int h = framebuffer->height;
	unsigned short *buf = (unsigned short *)framebuffer->data;
	for(int y=0;y<h;y++) {
		for(int x=0;x<w;x++)
			buf[x] = (buf[x]<<11)|(buf[x]>>11)|(buf[x]&0x07E0);
		buf += p;
	}*/

	//make sure the software buffer is flushed
	sceKernelDcacheWritebackAll();

	//sceGuStart(GU_DIRECT,list);

	//// clear screen
	////sceGuClearColor(0xff000000);
	////sceGuClear(GU_COLOR_BUFFER_BIT);

	//sceGuTexMode(GU_PSM_5650,0,0,0);
	//sceGuTexImage(0,framebuffer->pitch,tyres,framebuffer->pitch,framebuffer->data);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGB);
	//sceGuTexFilter(GU_NEAREST,GU_NEAREST);
	//sceGuTexScale(1.0f/(float)framebuffer->pitch,1.0f/(float)tyres);
	//sceGuTexOffset(0.0f,0.0f);

	//psp_renderSprite(0,
	//	(480-framebuffer->width)/2,
	//	(272-framebuffer->height)/2,
	//	framebuffer->width,framebuffer->height);

	//sceGuFinish();
	//sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	void *backBuffer = sceGuSwapBuffers();
	//sceGuSync(0,0);
	return backBuffer;

	//sceDisplayWaitVblankStart();

}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
void psp_init() {
	SetupCallbacks();

	sceGuInit();

	//sceGuStart(GU_DIRECT,list);
	//sceGuDrawBuffer(GU_PSM_5650,(void*)0,512);
	//sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x44000,512);
	//sceGuViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuEnable(GU_SCISSOR_TEST);
	//sceGuFrontFace(GU_CW);
	//sceGuShadeModel(GU_SMOOTH);
	//sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	//sceGuFinish();
	//sceGuSync(0,0);
	//sceGuDisplay(GU_TRUE);


	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_5650,(void*)0,512);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x44000,512);
	//sceGuViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuEnable(GU_SCISSOR_TEST);
	//sceGuFrontFace(GU_CW);
	//sceGuShadeModel(GU_SMOOTH);
	//sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0,0);
	sceGuDisplay(GU_TRUE);

}
