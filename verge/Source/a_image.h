/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef IMAGE_H
#define IMAGE_H

extern int width, depth;

image *xLoadImage(char *fname);
image *xLoadImage0(char *name);
byte *xLoadImage8bpp(char *fname);
image *xLoadImage8(char *fname);

#endif
