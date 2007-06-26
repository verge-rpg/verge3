/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef TIMER_H
#define TIMER_H

extern quad systemtime, timer, vctimer, hooktimer;
extern quad key_timer, key_repeater, repeatedkey;

class xTimer
{
	MMRESULT currtimer;
public:
	xTimer(int hz, LPTIMECALLBACK TimeProc);
	~xTimer();
	int hz;
};

extern xTimer *systimer;
void timer_Init(int hz);

#endif