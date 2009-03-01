#include "xerxes.h"

#include "iphone_interface.h"

bool joy_initd = false;
stick sticks[4];


int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
float mwheel;

void mouse_Init() {
	mouse_l = 0;
	mouse_r = 0;
	mouse_m = 0;
	mwheel = 0.0;
}

void mouse_set(int x, int y) {
	mouse_x = x;
	mouse_y = y;
}


int joy_NumSticks() {
	return 1;
}

int joy_Init() {

	memset(&sticks[0],0,sizeof(stick));

	joy_initd = true;
	return 1;
}

//static KPADStatus _kpadstatus;

void joy_Close() {
	
}

//static float pointing_scalex = 1.0f ; // Screen pointing scale
//static float pointing_scaley = 1.0f ; // Screen pointing scal

extern int ctrlmode = 0;

void joy_Update() {
}

void mouse_Update() {
	mouse_l = iphone_mouses[0].l;
	mouse_x = iphone_mouses[0].x;
	mouse_y = iphone_mouses[0].y;
}
