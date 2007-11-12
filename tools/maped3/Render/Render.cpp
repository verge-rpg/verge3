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
        static Bitmap __gc* createBitmap(int width, int height)
        {
            return __gc new Bitmap(width,height,PixelFormat::Format32bppArgb);
        }

        Render() { }

    public:
        __gc class Image : public IDisposable
        {
        public:
            int __nogc* buf;
            int width, height;
            int pitch, stride;

        private:
            __value enum Variety
            {
                _Buffer, _Bitmap
            };
            Variety variety;
            BitmapData __gc* bitmapData;
            Bitmap __gc* bmp;
            bool bDisposed;

        public:
            void Dispose()
            {
                if(bDisposed) return;
                bDisposed = true;
                if(variety == Variety::_Bitmap)
                    bmp->UnlockBits(bitmapData);
                if(variety == Variety::_Buffer)
                    free(buf);
            }
            Image()
            {
                bDisposed = false;
            }

        public:

            int getArray() __gc[]
            {
                int ret __gc[] = __gc new int __gc[width*height];

                for(int y=0;y<height;y++)
                    for(int x=0;x<width;x++)
                        ret[y*width+x] = buf[y*pitch+x];

                return ret;
            }

            Bitmap __gc* getBitmap()
            {
                int w = width;
                int h = height;
                int p = pitch;
                Bitmap __gc* bmp = __gc new Bitmap(w,h,PixelFormat::Format32bppArgb);
                BitmapData __gc* bmpd = bmp->LockBits(Rectangle(0,0,w,h),ImageLockMode::WriteOnly,PixelFormat::Format32bppArgb);
                int __nogc* data = (int __nogc*)bmpd->Scan0.ToPointer();

                int j = 0;
                for(int y=0;y<h;y++)
                    for(int x=0;x<w;x++)
                        data[j++] = buf[y*p+x];

                bmp->UnlockBits(bmpd);
                return bmp;	
            }

            void clear(int color)
            {
                int *data = buf;
                int xadd = stride - width*4;
                for(int y=0;y<height;y++)
                {
                    for(int x=0;x<width;x++)
                        *data++ = color;
                    data = (int *)((char *)data + xadd);
                }

            }


            int getPixel(int x, int y) {
                return buf[y * pitch + x];
            }

            static Image __gc* create(Bitmap __gc* bmp)
            {
                int w = bmp->Width;
                int h = bmp->Height;
                Image __gc* img = Render::Image::create(w,h);

                BitmapData __gc* bmpd = bmp->LockBits(Rectangle(0,0,w,h),ImageLockMode::ReadOnly,PixelFormat::Format32bppArgb);

                int dp = img->pitch;
                int sp = bmpd->Stride/4;

                int __nogc* srcdata = (int __nogc*)bmpd->Scan0.ToPointer();
                int __nogc* destdata = (int __nogc*)img->buf;

                for(int y=0;y<h;y++)
                    for(int x=0;x<w;x++)
                        destdata[y*dp+x] = srcdata[y*sp+x];

                bmp->UnlockBits(bmpd);

                return img;	
            }

            static Image __gc* create(int width, int height)
            {
                Image __gc* img = new Image();
                img->variety = Variety::_Buffer;
                img->width = width;
                img->height = height;
                img->buf = (int __nogc*)malloc(width*height*4);
                img->stride = width*4;
                img->pitch = width;
                return img;
            }
            static Image __gc* lockBitmap(Bitmap __gc* bmp)
            {
                return lockCommon(bmp, ImageLockMode::ReadWrite);
            }
            static Image __gc* lockWrite(Bitmap __gc* bmp)
            {
                return lockCommon(bmp, ImageLockMode::WriteOnly);
            }
            static Image __gc* lockRead(Bitmap __gc* bmp)
            {
                return lockCommon(bmp, ImageLockMode::ReadOnly);
            }

        private:
            static Image __gc* lockCommon(Bitmap __gc* bmp, ImageLockMode imageLockMode)
            {
                Image __gc* img = __gc new Image();
                img->variety = Variety::_Bitmap;
                img->bmp = bmp;
                img->bitmapData = bmp->LockBits(Rectangle(0,0,bmp->Width,bmp->Height),imageLockMode,PixelFormat::Format32bppArgb);

                img->buf = (int __nogc*)img->bitmapData->Scan0.ToPointer();
                img->width = img->bitmapData->Width;
                img->height = img->bitmapData->Height;
                img->stride = img->bitmapData->Stride;
                img->pitch = img->stride/4;

                return img;
            }

        public:

        };

    public:
        __value enum PixelOp
        {
            Src =1,
            Dest =2,
            Alpha50 =3,
            DestInvert =4,
            Alpha75 = 5
        };

        static int makeColor(int r, int g, int b)
        {
            return (int)((0xFF000000)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b));
        }



        static void renderColorPicker(Image __gc* img,  float h)
        {
            int* dst = img->buf;
            for(int y=0;y<256;y++)
            {
                for(int x=0;x<256;x++)
                {
                    *dst = HsbToColor(h, (float)x/256,(float)y/256);
                    dst++;
                }

            }
        }
        static int HsbToColor(float h, float s, float b)
        {
            float m1;
            float m2;

            h = h/360;
            m2 = (b<=0.5) ? b*(s+1) : b+s-b*s;
            m1 = b*2-m2;

            int r = HsbIntermediateValuesToRgbComponent(m1,m2,h + 1.0f/3);
            int g = HsbIntermediateValuesToRgbComponent(m1,m2,h);
            int b2 = HsbIntermediateValuesToRgbComponent(m1,m2,h - 1.0f/3);

            return ( 0xFF000000 | (r<<16) | (g<<8) | b2 );

        }
        static int HsbIntermediateValuesToRgbComponent(float m1, float m2, float h)
        {
            if(h<0)
            {
                h++;
            }
            else if(h>1)
            {
                h--;
            }

            if(h*6<1) return (int)(255*(m1+(m2-m1)*h*6));
            if(h*2<1) return (int)(255*m2);
            if(h*3<2) return (int)(255*(m1+(m2-m1)*(2.0f/3-h)*6));
            return (int)(255*m1);
        }

        // tatsumi
        static void renderChrFrame(Image __gc* img, int x0, int y0, int w, int h, unsigned char __nogc* framedata, int palette __gc[])
        {
            int* dst = img->buf + img->pitch * y0;
            int pitch = img->pitch;
            int srcofs = 0;
            int __pin* paldata = &palette[0];
            int cy=y0;
            int xmax=x0+w;
            int iw=img->width,ih=img->height;
            //renderBox(img, x0,y0,w,h,0xFFFFFFFF, PixelOp::Src);

            for(int y = 0; y < h; y++)
            {
                if (cy>=0&&cy<ih)
                    for(int x=x0; x<xmax; x++)
                    {
                        if(x>=ih)
                        {
                            srcofs += xmax-x;
                            x=xmax;
                            break;
                        }
                        else if(x>=0)
                        {
                            unsigned char px = framedata[srcofs];
                            if (px!=0) dst[x] = paldata[px];
                        }
                        srcofs++;
                    }
                    dst += pitch;
                    cy++;
            }
        }

        static void setPixel(Image __gc* img, int x0, int y0, int color)
        {
            if(x0<0 || x0 >= img->width || y0<0 || y0 >= img->height)
                return;
            img->buf[(y0)*img->pitch+x0] = color;	
        }

        static void renderSolid(Image __gc* img, int x0, int y0, int w, int h, int color, PixelOp op)
        {
            int bw = img->width;
            int bh = img->height;
            int bp = img->pitch;

            for(int y=0;y<h;y++)
                if(y+y0>=0 && y+y0<bh)
                    for(int x=0;x<w;x++)
                        if(x+x0>=0 && x+x0<bw)
                            handlePixel(color,img->buf[(y0+y)*bp+x0+x],op,1,0);
        }

        static void renderBox(Image __gc* img, int x0, int y0, int w, int h, int color, PixelOp op)
        {
            renderSolid(img, x0, y0, w, 1, color, op);
            renderSolid(img, x0, y0+h-1, w, 1, color, op);
            renderSolid(img, x0, y0, 1, h, color, op);
            renderSolid(img, x0+w-1, y0, 1, h, color, op);
        }

        static void renderColoredTile_50Alpha(Image __gc* img, int x0, int y0, int color)
        {
            int xlen=16;
            int ylen=16;

            int *s = 0;
            int *d = img->buf;

            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,0,dpitch,  0,img->width,0,img->height))
                return;

            for (; ylen; ylen--)
            {
                for (int x=0; x<xlen; x++)
                    handlePixel(color,d[x],PixelOp::Alpha50,1,0);

                d+=dpitch;
            }
        }
        static void renderColoredTile(Image __gc* img, int x0, int y0, int color)
        {
            int xlen=16;
            int ylen=16;

            int *s = 0;
            int *d = img->buf;

            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,0,dpitch,  0,img->width,0,img->height))
                return;

            for (; ylen; ylen--)
            {
                for (int x=0; x<xlen; x++)
                    d[x] = color;
                d+=dpitch;
            }
        }

        static void renderColoredStippleTile(Image __gc* img, int x0, int y0, int color1, int color2)
        {
            int xlen=16;
            int ylen=16;

            int *s = 0;
            int *d = img->buf;

            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,0,dpitch,  0,img->width,0,img->height))
                return;

            for (; ylen; ylen--)
            {
                for (int x=0; x<xlen; x++)
                    if((ylen^x)&1)
                        d[x] = color1;
                    else
                        d[x] = color2;
                d+=dpitch;
            }
        }

        static void renderObsTile(Image __gc* img, int x0, int y0, int* obsdata, bool clearbuf, int color)
        {
            int xlen=16;
            int ylen=16;

            //int color = (int)0xFFFFFFFF;

            int *s = obsdata;
            int *d = img->buf;

            const int spitch = 16;
            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,spitch,dpitch,  0,img->width,0,img->height))
                return;
            if(clearbuf)
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        if(s[x]!=0) d[x]=(int)0xFFFFFFFF;
                        else d[x]=(int)0xFF000000;

                        s+=spitch;
                        d+=dpitch;
                }
            else
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        if(s[x]!=0) handlePixel(color,d[x], PixelOp::Alpha50, 1, 0);

                    s+=spitch;
                    d+=dpitch;
                }
        }
        static void renderObsTileFast(Image __gc* img, int x0, int y0, int* obsdata, bool clearbuf)
        {
            int xlen=16;
            int ylen=16;

            int color = (int)0xFFFFFFFF;

            int *s = obsdata;
            int *d = img->buf;

            const int spitch = 16;
            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,spitch,dpitch,  0,img->width,0,img->height))
                return;
            if(clearbuf)
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        if(s[x]!=0) d[x]=(int)0xFFFFFFFF;
                        else d[x]=(int)0xFF000000;

                        s+=spitch;
                        d+=dpitch;
                }
            else
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        if(s[x]!=0) d[x]=(int)0xFFFFFFFF;

                    s+=spitch;
                    d+=dpitch;
                }
        }
        static void renderTileCross(Image __gc* img, int x0, int y0)
        {
            int pitch = img->pitch;
            int *ptr = img->buf + y0 * pitch + x0;
            for(int y=0;y<16;y++)
                ptr[pitch * y + 8] = 0xFFFFFFFF;
            for(int x=0;x<16;x++)
                ptr[pitch * 8 + x] = 0xFFFFFFFF;
        }

        static void renderTile32(Image __gc* img, int x0, int y0, int __nogc* tiledata, bool drawZero) {
            render(img, x0, y0, 16, 16, tiledata, drawZero);
        }

        static void renderTile32_Mix(Image __gc* img, int x0, int y0, int __nogc* tiledata, bool drawZero, PixelOp op)
        {
            int xlen=16;
            int ylen=16;

            int *s = tiledata;
            int *d = img->buf;

            const int spitch = 16;
            int dpitch = img->pitch;

            if(clip(x0,y0,xlen,ylen,s,d,spitch,dpitch,  0,img->width,0,img->height))
                return;

            if(drawZero)
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        handlePixel(s[x],d[x],op,1,0);

                    s+=spitch;
                    d+=dpitch;
                }
            else
                for (; ylen; ylen--)
                {
                    for (int x=0; x<xlen; x++)
                        handlePixel(s[x],d[x],op,1,1);

                    s+=spitch;
                    d+=dpitch;
                }
        }


        static void renderNumber(Image __gc* img, int x0, int y0, int number, int color)
        {
            int height = img->height;
            int width = img->width;
            int *pixels = img->buf;

            Char nums __gc[] = number.ToString()->ToCharArray();
            int cn = 0;
            for(int i=0;i<nums->Length;i++)
            {
                Char c = nums[i];
                cn = nums[i]-'0';
                byte ba __gc[] = BiosFont::Number(cn);

                int pxW = ba[0];
                int pxH = (ba->Length - 1) / pxW;

                for(int i = 0; i < pxH; i++)
                {
                    if ( y0+i < 0 || y0+i >= height ) continue;
                    for(int j = 0; j < pxW; j++)
                    {
                        if ( x0+j < 0 || x0+j >= width ) continue;
                        if ( ba[1 + (i*pxW)+j] == 1 )
                        {
                            pixels[(y0+i) * width + (x0+j)] = color;
                        }
                    }					
                }
                x0 += pxW + 1;
            }
        }	

        static void render(Image __gc* dest, int x, int y, int xlen, int ylen, int __nogc* pixels, bool drawZero) {
            int *s = pixels;
            int *d = dest->buf;

            const int spitch = xlen;
            int dpitch = dest->pitch;

            if(clip(x,y,xlen,ylen,s,d,spitch,dpitch,  0,dest->width,0,dest->height)) {
                return;
            }

            for (; ylen; ylen--) {
                for (int x=0; x<xlen; x++) {
                    handlePixel(s[x],d[x],0,0,drawZero ? 0 : 1);
                }

                s+=spitch;
                d+=dpitch;
            }
        }

        static void render(Image __gc* dest, int x, int y, Image __gc* src, bool drawZero) {
            render(dest, x, y, src->width, src->height, src->buf, drawZero);
        }
    };


}



int clip(int &x0,int &y0,int &xlen,int &ylen,int *&s,int *&d,const int spitch,const int dpitch,const int cx1,const int cx2,const int cy1,const int cy2)
{
    if(x0>=cx2 || y0>=cy2 || x0+xlen<=cx1 || y0+ylen<=cy1)
        return 1;

    if(x0+xlen>cx2) 
        xlen = cx2-x0;

    if (y0+ylen>cy2) 
        ylen = cy2-y0;

    if (x0<cx1)
    {
        s +=(cx1-x0);
        xlen-=(cx1-x0);
        x0  =cx1;
    }
    if (y0<cy1)
    {
        s +=(cy1-y0)*spitch;
        ylen-=(cy1-y0);
        y0  =cy1;
    }

    d += (y0 * dpitch) + x0;

    return 0;
}

void handlePixel(int &src, int &dest, const int op, const int mixFlag, const int tflag)
{
    if(mixFlag)
    {
        if(tflag)
        {
            if((src&0x00FFFFFF)!=0x00FF00FF)
                dest = mixPixel(src,dest,op);
        }
        else dest = mixPixel(src,dest,op);
    }
    else
    {
        if(tflag)
        {
            if((src&0x00FFFFFF)!=0x00FF00FF)
                dest = src;
        }
        else
            dest = src;
    }
}

int mixPixel(const int src, const int dest, const int op)
{
    switch(op)
    {
    case 3:
        {
            int r = (((src&0x00FF0000)*3 + (dest&0x00FF0000))>>2)&0x00FF0000;
            int g = (((src&0x0000FF00)*3 + (dest&0x0000FF00))>>2)&0x0000FF00;
            int b = (((src&0x000000FF)*3 + (dest&0x000000FF))>>2)&0x000000FF;
            return (0xFF000000)|(r|g|b);
        }
    case 5:
        {
            //int r = ((src&0x00FF0000)>>17) + ((dest&0x00FF0000)>>17);
            //int g = ((src&0x0000FF00)>>9)  + ((dest&0x0000FF00)>>9);
            //int b = ((src&0x000000FF)>>1)  + ((dest&0x000000FF)>>1);
            //return (int)((0xFF000000)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b));

            int r = ((src&0x00FF0000) + (dest&0x00FF0000))&0x01FE0000;
            int g = ((src&0x0000FF00) + (dest&0x0000FF00))&0x0001FE00;
            int b = ((src&0x000000FF) + (dest&0x000000FF))&0x000001FE;
            return (0xFF000000)|((r|g|b)>>1);

            /*	__m64 s;
            __m64 d;
            __m64 temp;

            s = _m_from_int(src);
            d = _m_from_int(dest);
            temp = _m_pxor(temp,temp);
            s = _m_punpcklbw(s,temp);
            d = _m_punpcklbw(d,temp);
            s = _m_paddusw(s,d);
            s = _m_psrlwi(s,1);
            s = _m_packuswb(s,temp);
            int ret = _m_to_int(s);
            _m_empty();
            return ret;*/


            /*			int rret;
            __asm
            {
            movd mm0,src;
            movd mm1,dest;
            pxor mm2,mm2;
            punpcklbw mm0,mm2;
            punpcklbw mm1,mm2;
            paddusw mm0,mm1;
            psrlw mm0,1;
            packuswb mm0,mm0;
            movd rret,mm0;
            emms;
            }
            return rret;*/

        }
    case 1:
        {
            return src;
        }
    case 2:
        {
            return dest;
        }
    case 4:
        {
            return (int)((~(unsigned int)dest) | 0xFF000000);
        }
    default:
        return 0;
    }
}