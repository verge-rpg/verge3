/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef MOUSE_H
#define MOUSE_H

extern int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
extern float mwheel;

void mouse_set(int x, int y);

void mouse_Init();
void mouse_Update();

#endif