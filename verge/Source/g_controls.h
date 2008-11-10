/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


extern byte up, down, left, right;
extern byte kill_up, kill_down, kill_left, kill_right;
extern byte b1, b2, b3, b4;
extern byte kill_b1, kill_b2, kill_b3, kill_b4;
extern StringRef bindbutton[4];

#define UnUp() { kill_up = true; up = false; }
#define UnDown() { kill_down = true; down = false; }
#define UnLeft() { kill_left = true; left = false; }
#define UnRight() { kill_right = true; right = false; }
#define UnB1() { kill_b1 = true; b1 = false; }
#define UnB2() { kill_b2 = true; b2 = false; }
#define UnB3() { kill_b3 = true; b3 = false; }
#define UnB4() { kill_b4 = true; b4 = false; }

extern byte k_b1, k_b2, k_b3, k_b4;
extern byte k_up, k_down, k_left, k_right; // Overkill (2006-06-25): Customizable directionals on the keyboard.
extern byte j_b1, j_b2, j_b3, j_b4;

void UpdateControls();
