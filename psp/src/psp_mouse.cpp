/****************************************************************
	xerxes engine
	mac_mouse.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
float mwheel;

/***************************** code *****************************/

void mouse_Init() {
	mouse_l = 0;
	mouse_r = 0;
	mouse_m = 0;
	mwheel = 0.0;
}

void mouse_set(int x, int y) {
}

void mouse_Update() {
//actually do an update one day
}
