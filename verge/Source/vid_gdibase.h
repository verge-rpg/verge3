/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef DDBASE_H
#define DDBASE_H

void ddwin_Flip();
void dd_Flip();
void dd_Fallback();
int dd_SetMode(int xres, int yres, int bpp, bool windowflag);
void dd_Close();
void dd_RegisterBlitters();

#endif