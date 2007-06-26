#include "xerxes.h"
#include <psptypes.h>
#include <psprtc.h>

quad systemtime = 0, timer = 0, vctimer = 0, hooktimer = 0;
quad key_timer, key_repeater, repeatedkey;

u64 timer_t0;
int timer_hz;
int timer_curr = 0;

void timer_tick() {
	//increment the psp timer
	timer_curr++; 
	//log("tick (%d)",timer_curr);

	//win_movie_update();
	//no movie support on psp

	systemtime++;
	if (engine_paused) return;
	timer++;
	vctimer++;
	hooktimer++;

	//MBG 9/5/05 TODO HACK FOR PSPVERGE
	if(sticks[0].button[7]) 
		for(int i=0;i<7;i++)
		{
			systemtime++;
			timer++;
			vctimer++;
			hooktimer++;
		}

	//we'll worry about fast forward later
	//tilde fast-forward
	//if(GetAsyncKeyState(0xC0))
	//{
	//	for(int i=0;i<7;i++)
	//	{
	//		systemtime++;
	//		timer++;
	//		vctimer++;
	//		hooktimer++;
	//	}
	//}
}

void timer_update() {
	u64 tcurr;
	sceRtcGetCurrentTick(&tcurr);
	int curr = (int)((tcurr-timer_t0)/10000);
	while(timer_curr<curr)
		timer_tick();
}

void timer_Init(int hz) {
	sceRtcGetCurrentTick(&timer_t0);
	timer_hz = hz;
}
