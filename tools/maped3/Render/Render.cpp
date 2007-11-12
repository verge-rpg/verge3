#pragma once

#using <mscorlib.dll>
#using <system.dll>
#using <system.drawing.dll>
#include <stdlib.h>

typedef unsigned char byte;

#include "BiosFont.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

__forceinline int mixPixel(int const src, int const dest, int const op);
__forceinline int clip(int &x0,int &y0,int &xlen,int &ylen,int *&s,int *&d,const int spitch,const int dpitch,const int cx1,const int cx2,const int cy1,const int cy2);
__forceinline void handlePixel(int &src, int &dest, const int op, const int mixFlag, const int tflag);

namespace pr2
{
    public __sealed __gc class Render
    {
    public:

    public:
        __gc __abstract class Image : public IDisposable
        {
        protected:
            int width, height;

        public:
            __property virtual int* get_Pixels() = 0;
            __property virtual int get_Pitch() = 0;
            __property virtual int get_Stride() = 0;

            __property virtual int get_Width() = 0;
            __property virtual int get_Height() = 0;

            virtual void Dispose() = 0;
            virtual int getArray() __gc[] = 0;

            virtual Bitmap __gc* getBitmap() = 0;
            virtual void clear(int color) = 0;
            virtual int getPixel(int x, int y) = 0;
        public:

        };
    };
}
