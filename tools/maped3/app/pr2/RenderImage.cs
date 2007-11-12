using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace winmaped2.pr2 {
    // TODO: come up with a better name for this class.
    // ALSO TODO: Unify with winmaped2.Canvas
    public unsafe class RenderImage : IDisposable {
        enum Variety {
            _Buffer, _Bitmap
        };

        Variety variety;

        BitmapData bitmapData;
        Bitmap bmp;
        bool bDisposed;
        int* buf;
        int width;
        int height;
        int pitch;
        int stride;

        public RenderImage() {
            bDisposed = false;
        }

        public int* Pixels {
            get {
                return buf;
            }
        }

        public int Pitch {
            get {
                return pitch;
            }
        }

        public int Stride {
            get {
                return stride;
            }
        }

        public int Width {
            get {
                return width;
            }
        }

        public int Height {
            get {
                return height;
            }
        }

        public unsafe void Dispose() {
            if (!bDisposed) {
                bDisposed = true;
                if (variety == Variety._Bitmap) {
                    bmp.UnlockBits(bitmapData);
                } else if (variety == Variety._Buffer) {
                    System.Runtime.InteropServices.Marshal.FreeHGlobal((IntPtr)buf);
                }
            }
        }

        public unsafe int[] getArray() {
            int[] ret = new int[width * height];

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    ret[y * width + x] = buf[y * pitch + x];
                }
            }

            return ret;
        }

        public unsafe Bitmap getBitmap() {
            int w = width;
            int h = height;
            int p = pitch;
            Bitmap bmp = new Bitmap(w, h, PixelFormat.Format32bppArgb);
            BitmapData bmpd = bmp.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            int* data = (int*)bmpd.Scan0.ToPointer();

            int j = 0;
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    data[j++] = buf[y * p + x];
                }
            }
            bmp.UnlockBits(bmpd);
            return bmp;
        }

        public unsafe void clear(int color) {
            int* data = buf;
            int xadd = pitch - width;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++)
                    *data++ = color;
                data += xadd;
            }
        }

        public unsafe int getPixel(int x, int y) {
            return buf[y * pitch + x];
        }

        public unsafe static RenderImage create(Bitmap bmp) {
            int w = bmp.Width;
            int h = bmp.Height;
            RenderImage img = create(w, h);

            BitmapData bmpd = bmp.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            int dp = img.pitch;
            int sp = bmpd.Stride / 4;

            int* srcdata = (int*)bmpd.Scan0.ToPointer();
            int* destdata = (int*)img.Pixels;

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    destdata[y * dp + x] = srcdata[y * sp + x];
                }
            }
            bmp.UnlockBits(bmpd);

            return img;
        }

        public unsafe static RenderImage create(int width, int height) {
            RenderImage img = new RenderImage();
            img.variety = Variety._Buffer;
            img.width = width;
            img.height = height;
            img.buf = (int*)System.Runtime.InteropServices.Marshal.AllocHGlobal(width * height * 4);
            img.stride = width * 4;
            img.pitch = width;
            return img;
        }

        public unsafe static RenderImage lockBitmap(Bitmap bmp) {
            ImageLockMode imageLockMode = ImageLockMode.ReadWrite;
            RenderImage img = new RenderImage();
            img.variety = Variety._Bitmap;
            img.bmp = bmp;
            img.bitmapData = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), imageLockMode, PixelFormat.Format32bppArgb);

            img.buf = (int*)img.bitmapData.Scan0.ToPointer();
            img.width = img.bitmapData.Width;
            img.height = img.bitmapData.Height;
            img.stride = img.bitmapData.Stride;
            img.pitch = img.stride / 4;

            return img;
        }

    }
}
