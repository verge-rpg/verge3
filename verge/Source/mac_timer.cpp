/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	mac_timer.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

quad systemtime = 0, timer = 0, vctimer = 0, hooktimer = 0;
xTimer *systimer;

/***************************** code *****************************/

Uint32 DefaultTimer(Uint32 interval, void *param)
{
	win_movie_update();

	systemtime++;
	if (engine_paused) return interval;
	timer++;
	vctimer++;
	hooktimer++;

	//tilde fast-forward
	if(isSDLKeyPressed(SDLK_BACKQUOTE))
	{
		for(int i=0;i<7;i++)
		{
			systemtime++;
			timer++;
			vctimer++;
			hooktimer++;
		}
	}
	return interval; // set it again for same length of time
}

void timer_Init(int hz)
{
	systimer = new xTimer(hz, DefaultTimer);
}

/****************************************************************/

#ifndef __EMSCRIPTEN__
#include <pthread.h>
#endif

xTimer::xTimer(int hz, SDL_TimerCallback TimeProc)
{
#ifndef __EMSCRIPTEN__
	pthread_t self = pthread_self();
	sched_param param;
	int policy;
	pthread_getschedparam(self,&policy,&param);
	param.sched_priority=0x7FFFFFFF;
	policy = 0;
	pthread_setschedparam(self,policy,&param);
	pthread_getschedparam(self,&policy,&param);
#endif

#ifndef __IPHONE__
	// timer set in ms
	timer_id = SDL_AddTimer(1000/hz, TimeProc, NULL);
	if(!timer_id)
		err("SDL_AddTimer failed");
#endif

	xTimer::hz=hz;
}

xTimer::~xTimer()
{
	SDL_RemoveTimer(timer_id);
    timer_id=0;
}
