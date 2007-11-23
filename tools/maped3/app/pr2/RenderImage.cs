using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace winmaped2.pr2 {
    // TODO: come up with a better name for this class.
    // ALSO TODO: Unify with winmaped2.Canvas
    public unsafe abstract class RenderImage : winmaped2.pr2.IRenderImage {
        protected int* buf;
        protected int width;
        protected int height;
        protected int pitch;
        protected int stride;

        public int* Buffer {
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

        public abstract void Dispose();

        public unsafe void UpdatePixels(int[] newPixels) {
            if (newPixels.Length != width * height) {
                throw new Exception("RenderImage.UpdatePixels got incorrectly sized array");
            }
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    buf[y * pitch + x] = newPixels[y * width + x];
                }
            }
        }

        public unsafe int[] GetArray() {
            int[] ret = new int[width * height];

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    ret[y * width + x] = buf[y * pitch + x];
                }
            }

            return ret;
        }

        public unsafe Bitmap ConvertToBitmap() {
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

        public unsafe void Clear(int color) {
            int* data = buf;
            int xadd = pitch - width;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++)
                    *data++ = color;
                data += xadd;
            }
        }

        public IRenderImage Clone() {
            IRenderImage img = new BufferImage(width, height);
            img.UpdatePixels(GetArray());
            return img;
        }

        public unsafe int GetPixel(int x, int y) {
            return buf[y * pitch + x];
        }

        public unsafe void SetPixel(int x, int y, int color) {
            if (!(0 <= x && x < Width) ||
                !(0 <= y && y < Height)
            ) {
                throw new InvalidOperationException(String.Format("Bad arguments to SetPixel: {0},{1}", x, y));
            }
            buf[y * pitch + x] = color;
        }

        public static IRenderImage Create(Bitmap bmp) {
            return new BufferImage(bmp);
        }

        public static RenderImage Create(int width, int height) {
            return new BufferImage(width, height);
        }

        public static IRenderImage LockBitmap(Bitmap bmp) {
            return new LockedBitmap(bmp);
        }
    }
}
