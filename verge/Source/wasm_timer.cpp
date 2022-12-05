/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#ifdef __EMSCRIPTEN__

/****************************************************************
	xerxes engine
	wasm_timer.cpp
 ****************************************************************/

#include "xerxes.h"
#include <emscripten/eventloop.h>

/***************************** data *****************************/

quad systemtime = 0, timer = 0, vctimer = 0, hooktimer = 0;
xTimer* systimer;

/***************************** code *****************************/

void DefaultTimer(void* param)
{
	win_movie_update();

	systemtime++;
	if (engine_paused) return;
	timer++;
	vctimer++;
	hooktimer++;

	//tilde fast-forward
	if (isSDLKeyPressed(SDLK_BACKQUOTE))
	{
		for(int i=0;i<7;i++)
		{
			systemtime++;
			timer++;
			vctimer++;
			hooktimer++;
		}
	}
}

void timer_Init(int hz)
{
	systimer = new xTimer(hz, DefaultTimer);
}

xTimer::xTimer(int hz, xTimerCallback TimeProc)
{
	timer_id = emscripten_set_interval(&DefaultTimer, 1000 / hz, nullptr);
	xTimer::hz = hz;
}

xTimer::~xTimer()
{
	emscripten_clear_interval(timer_id);
    timer_id = 0;
}

#endif