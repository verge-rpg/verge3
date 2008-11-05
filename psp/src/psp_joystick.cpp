#include "xerxes.h"
#include <pspctrl.h>

bool joy_initd = false;
stick sticks[4];

int joy_NumSticks() {
	return 1;
}

int joy_Init() {

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	memset(&sticks[0],0,sizeof(stick));

	joy_initd = true;
	return 1;
}

void joy_Close() {
}

void joy_Update() {
	SceCtrlData pad;
	//why does read slow us down to something terrible?
	//now i am having to use peek
	sceCtrlPeekBufferPositive(&pad, 1); 


	sticks[0].button[0] = (pad.Buttons & PSP_CTRL_CROSS)!=0;
	sticks[0].button[1] = (pad.Buttons & PSP_CTRL_CIRCLE)!=0;
	sticks[0].button[2] = (pad.Buttons & PSP_CTRL_SQUARE)!=0;
	sticks[0].button[3] = (pad.Buttons & PSP_CTRL_TRIANGLE)!=0;
	sticks[0].button[4] = (pad.Buttons & PSP_CTRL_SELECT)!=0;
	sticks[0].button[5] = (pad.Buttons & PSP_CTRL_START)!=0;
	sticks[0].button[6] = (pad.Buttons & PSP_CTRL_LTRIGGER)!=0;
	sticks[0].button[7] = (pad.Buttons & PSP_CTRL_RTRIGGER)!=0;
	sticks[0].up = (pad.Buttons & PSP_CTRL_UP)!=0;
	sticks[0].down = (pad.Buttons & PSP_CTRL_DOWN)!=0;
	sticks[0].left = (pad.Buttons & PSP_CTRL_LEFT)!=0;
	sticks[0].right = (pad.Buttons & PSP_CTRL_RIGHT)!=0;
	sticks[0].analog_x = pad.Lx;
	sticks[0].analog_y = pad.Ly;
}
