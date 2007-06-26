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
	win_timer.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

quad systemtime = 0, timer = 0, vctimer = 0, hooktimer = 0;
quad key_timer, key_repeater, repeatedkey;
xTimer *systimer;

/***************************** code *****************************/

void CALLBACK DefaultTimer(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	win_movie_update();

	systemtime++;
	if (engine_paused) return;
	timer++;
	vctimer++;
	hooktimer++;

	//tilde fast-forward
	if(GetAsyncKeyState(0xC0))
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

/****************************************************************/

xTimer::xTimer(int hz, LPTIMECALLBACK TimeProc)
{
	if(timeBeginPeriod(1000/hz)!=TIMERR_NOERROR)
		err("bad error with timeBeginPeriod(). please report this.");
	if (currtimer)
		timeKillEvent(currtimer);
    currtimer = timeSetEvent(1000/hz,0,(LPTIMECALLBACK)TimeProc,0,TIME_PERIODIC);
	xTimer::hz=hz;
}

xTimer::~xTimer()
{
	timeKillEvent(currtimer);
	timeEndPeriod(1000/hz);
    currtimer=0;
}
