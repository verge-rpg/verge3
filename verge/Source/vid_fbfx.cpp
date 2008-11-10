#include "xerxes.h"

/************** 2xSAI **************/
//mbg 9/10/05 TODO
//if anyone wants to use 2xsai, we may have to add stubs in the
//non-defined section
#ifdef ENABLE_2XSAI

static quad ColorMask;
static quad LowPixelMask;
static quad QColorMask;
static quad QLowPixelMask;
static quad RedBlueMask;
static quad GreenMask;
static int PixelsPerMask;

static byte *src_line[4];
static byte *dst_line[2];

void Init_2xSAI()
{
	const int bpp = 32;
	int min_r=0, min_g=0, min_b=0;

	/* Get lowest color bit */
	for (int i=0; i<255; i++)
	{
		if (!min_r) min_r = MakeColor(i, 0, 0);
		if (!min_g) min_g = MakeColor(0, i, 0);
		if (!min_b)	min_b = MakeColor(0, 0, i);
	}
	ColorMask = (MakeColor(255,0,0)-min_r) | (MakeColor(0,255,0)-min_g) | (MakeColor(0,0,255)-min_b);
	LowPixelMask = min_r | min_g | min_b;
	QColorMask = (MakeColor(255,0,0)-3*min_r) | (MakeColor(0,255,0)-3*min_g) | (MakeColor(0,0,255)-3*min_b);
	QLowPixelMask = (min_r*3) | (min_g*3) | (min_b*3);
	RedBlueMask = MakeColor(255, 0, 255);
	GreenMask = MakeColor(0, 255, 0);

	PixelsPerMask = (bpp <= 16) ? 2 : 1;

	if (PixelsPerMask == 2)
	{
		ColorMask |= (ColorMask << 16);
		QColorMask |= (QColorMask << 16);
		LowPixelMask |= (LowPixelMask << 16);
		QLowPixelMask |= (QLowPixelMask << 16);
	}
}

#define GET_RESULT(A, B, C, D) ((A != C || A != D) - (B != C || B != D))
#define INTERPOLATE(A, B) (((A & ColorMask) >> 1) + ((B & ColorMask) >> 1) + (A & B & LowPixelMask))
#define Q_INTERPOLATE(A, B, C, D) ((A & QColorMask) >> 2) + ((B & QColorMask) >> 2) + ((C & QColorMask) >> 2) + ((D & QColorMask) >> 2) \
	+ ((((A & QLowPixelMask) + (B & QLowPixelMask) + (C & QLowPixelMask) + (D & QLowPixelMask)) >> 2) & QLowPixelMask)


void run2xSAI_engine(byte *src, quad src_pitch, image *dest, quad width, quad height);
void run2xSAI(image *src, image *dest)
{
	/* are both images non-null? */
	if (!src || !dest)
		return;

	/* image must be at least 4x4 */
	if (src->width < 4 || src->height < 4)
		return;

	/* make sure the destination image is at least 2x as big as the source. */
	if (dest->width < src->width*2 || dest->height < src->height*2)
		return;

	run2xSAI_engine((byte*)src->data, src->pitch*vid_bytesperpixel, dest, src->width, src->height);
}

void run2xSAI_engine(byte *src, quad src_pitch, image *dest, quad width, quad height)
{
	quad x, y;
	unsigned long color[16];

	/* Point to the first 3 lines. */
	src_line[0] = src;
	src_line[1] = src;
	src_line[2] = src + src_pitch;
	src_line[3] = src + src_pitch * 2;

	/* Can we write the results directly? */
	dst_line[0] = &((byte*)dest->data)[0];
	dst_line[1] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel];

	x = 0, y = 0;

	if (PixelsPerMask == 2)
	{
		word *sbp;
		sbp = (word*) src_line[0];
		color[0] = *sbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
		color[4] = color[0];   color[5] = color[0];   color[6] = *(sbp + 1);  color[7] = *(sbp + 2);
		sbp = (word*)src_line[2];
		color[8] = *sbp;     color[9] = color[8];     color[10] = *(sbp + 1); color[11] = *(sbp + 2);
		sbp = (word*)src_line[3];
		color[12] = *sbp;    color[13] = color[12];   color[14] = *(sbp + 1); color[15] = *(sbp + 2);
	}
	else
	{
		unsigned long *lbp;
		lbp = (unsigned long*) src_line[0];
		color[0] = *lbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
		color[4] = color[0];   color[5] = color[0];   color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
		lbp = (unsigned long*) src_line[2];
		color[8] = *lbp;     color[9] = color[8];     color[10] = *(lbp + 1); color[11] = *(lbp + 2);
		lbp = (unsigned long*) src_line[3];
		color[12] = *lbp;    color[13] = color[12];   color[14] = *(lbp + 1); color[15] = *(lbp + 2);
	}

	for (y = 0; y < height; y++)
	{
		/* Todo: x = width - 2, x = width - 1 */
		for (x = 0; x < width; x++) {
			unsigned long product1a, product1b, product2a, product2b;

//---------------------------------------  B0 B1 B2 B3    0  1  2  3
//                                         4  5* 6  S2 -> 4  5* 6  7
//                                         1  2  3  S1    8  9 10 11
//                                         A0 A1 A2 A3   12 13 14 15
//--------------------------------------
			if (color[9] == color[6] && color[5] != color[10])
			{
				product2b = color[9];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] != color[6])
			{
				product2b = color[5];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] == color[6])
			{
				int r = 0;

				r += GET_RESULT(color[6], color[5], color[8], color[13]);
				r += GET_RESULT(color[6], color[5], color[4], color[1]);
				r += GET_RESULT(color[6], color[5], color[14], color[11]);
				r += GET_RESULT(color[6], color[5], color[2], color[7]);

				if (r > 0)
					product1b = color[6];
				else if (r < 0)
					product1b = color[5];
				else
					product1b = INTERPOLATE(color[5], color[6]);

				product2b = product1b;

			}
			else
			{
				if (color[6] == color[10] && color[10] == color[13] && color[9] != color[14] && color[10] != color[12])
					product2b = Q_INTERPOLATE(color[10], color[10], color[10], color[9]);
				else if (color[5] == color[9] && color[9] == color[14] && color[13] != color[10] && color[9] != color[15])
					product2b = Q_INTERPOLATE(color[9], color[9], color[9], color[10]);
				else
					product2b = INTERPOLATE(color[9], color[10]);

				if (color[6] == color[10] && color[6] == color[1] && color[5] != color[2] && color[6] != color[0])
					product1b = Q_INTERPOLATE(color[6], color[6], color[6], color[5]);
				else if (color[5] == color[9] && color[5] == color[2] && color[1] != color[6] && color[5] != color[3])
					product1b = Q_INTERPOLATE(color[6], color[5], color[5], color[5]);
				else
					product1b = INTERPOLATE(color[5], color[6]);
			}

			if (color[5] == color[10] && color[9] != color[6] && color[4] == color[5] && color[5] != color[14])
				product2a = INTERPOLATE(color[9], color[5]);
			else if (color[5] == color[8] && color[6] == color[5] && color[4] != color[9] && color[5] != color[12])
				product2a = INTERPOLATE(color[9], color[5]);
			else
				product2a = color[9];

			if (color[9] == color[6] && color[5] != color[10] && color[8] == color[9] && color[9] != color[2])
				product1a = INTERPOLATE(color[9], color[5]);
			else if (color[4] == color[9] && color[10] == color[9] && color[8] != color[5] && color[9] != color[0])
				product1a = INTERPOLATE(color[9], color[5]);
			else
				product1a = color[5];

			if (PixelsPerMask == 2)
			{
				*((unsigned long *) (&dst_line[0][x * 4])) = product1a | (product1b << 16);
				*((unsigned long *) (&dst_line[1][x * 4])) = product2a | (product2b << 16);
			}
			else
			{
				*((unsigned long *) (&dst_line[0][x * 8])) = product1a;
				*((unsigned long *) (&dst_line[0][x * 8 + 4])) = product1b;
				*((unsigned long *) (&dst_line[1][x * 8])) = product2a;
				*((unsigned long *) (&dst_line[1][x * 8 + 4])) = product2b;
			}

			/* Move color matrix forward */
			color[0] = color[1]; color[4] = color[5]; color[8] = color[9];   color[12] = color[13];
			color[1] = color[2]; color[5] = color[6]; color[9] = color[10];  color[13] = color[14];
			color[2] = color[3]; color[6] = color[7]; color[10] = color[11]; color[14] = color[15];

			if (x < width - 3)
			{
				x += 3;
				if (PixelsPerMask == 2)
				{
					color[3] = *(((unsigned short*)src_line[0]) + x);
					color[7] = *(((unsigned short*)src_line[1]) + x);
					color[11] = *(((unsigned short*)src_line[2]) + x);
					color[15] = *(((unsigned short*)src_line[3]) + x);
				}
				else
				{
					color[3] = *(((unsigned long*)src_line[0]) + x);
					color[7] = *(((unsigned long*)src_line[1]) + x);
					color[11] = *(((unsigned long*)src_line[2]) + x);
					color[15] = *(((unsigned long*)src_line[3]) + x);
				}
				x -= 3;
			}
		}

		/* We're done with one line, so we shift the source lines up */
		src_line[0] = src_line[1];
		src_line[1] = src_line[2];
		src_line[2] = src_line[3];

		/* Read next line */
		if (y + 3 >= height)
			src_line[3] = src_line[2];
		else
			src_line[3] = src_line[2] + src_pitch;

		/* Then shift the color matrix up */
		if (PixelsPerMask == 2)
		{
			unsigned short *sbp;
			sbp = (unsigned short*)src_line[0];
			color[0] = *sbp;     color[1] = color[0];    color[2] = *(sbp + 1);  color[3] = *(sbp + 2);
			sbp = (unsigned short*)src_line[1];
			color[4] = *sbp;     color[5] = color[4];    color[6] = *(sbp + 1);  color[7] = *(sbp + 2);
			sbp = (unsigned short*)src_line[2];
			color[8] = *sbp;     color[9] = color[9];    color[10] = *(sbp + 1); color[11] = *(sbp + 2);
			sbp = (unsigned short*)src_line[3];
			color[12] = *sbp;    color[13] = color[12];  color[14] = *(sbp + 1); color[15] = *(sbp + 2);
		}
		else
		{
			unsigned long *lbp;
			lbp = (unsigned long*)src_line[0];
			color[0] = *lbp;     color[1] = color[0];    color[2] = *(lbp + 1);  color[3] = *(lbp + 2);
			lbp = (unsigned long*)src_line[1];
			color[4] = *lbp;     color[5] = color[4];    color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
			lbp = (unsigned long*)src_line[2];
			color[8] = *lbp;     color[9] = color[9];    color[10] = *(lbp + 1); color[11] = *(lbp + 2);
			lbp = (unsigned long*)src_line[3];
			color[12] = *lbp;    color[13] = color[12];  color[14] = *(lbp + 1); color[15] = *(lbp + 2);
		}

		/* Write the 2 lines, if not already done so */
		if (y < height - 1)
		{
			dst_line[0] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel*(y * 2 + 2)];
			dst_line[1] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel*(y * 2 + 3)];
		}
	}
}
#else
void Init_2xSAI(int bpp) {}
#endif //ENABLE_2XSAI

