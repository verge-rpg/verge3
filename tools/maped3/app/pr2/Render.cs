using System;
using System.Drawing;
using System.Drawing.Imaging;

namespace winmaped2 {
    public sealed class Render {
        public unsafe class Image : IDisposable {
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

            public Image() {
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

            public unsafe static Image create(Bitmap bmp) {
                int w = bmp.Width;
                int h = bmp.Height;
                Image img = create(w, h);

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

            public unsafe static Image create(int width, int height) {
                Image img = new Image();
                img.variety = Variety._Buffer;
                img.width = width;
                img.height = height;
                img.buf = (int*)System.Runtime.InteropServices.Marshal.AllocHGlobal(width * height * 4);
                img.stride = width * 4;
                img.pitch = width;
                return img;
            }

            public unsafe static Image lockBitmap(Bitmap bmp) {
                ImageLockMode imageLockMode = ImageLockMode.ReadWrite;
                Image img = new Image();
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

        private Render() { }

        public static Bitmap createBitmap(int width, int height) {
            return new Bitmap(width, height, PixelFormat.Format32bppArgb);
        }

        private static int HsbIntermediateValuesToRgbComponent(float m1, float m2, float h) {
            if (h < 0) {
                h++;
            } else if (h > 1) {
                h--;
            }

            if (h * 6 < 1) return (int)(255 * (m1 + (m2 - m1) * h * 6));
            if (h * 2 < 1) return (int)(255 * m2);
            if (h * 3 < 2) return (int)(255 * (m1 + (m2 - m1) * (2.0f / 3 - h) * 6));
            return (int)(255 * m1);
        }

        private static int HsbToColor(float h, float s, float b) {
            float m1;
            float m2;

            h = h / 360;
            m2 = (b <= 0.5) ? b * (s + 1) : b + s - b * s;
            m1 = b * 2 - m2;

            int r = HsbIntermediateValuesToRgbComponent(m1, m2, h + 1.0f / 3);
            int g = HsbIntermediateValuesToRgbComponent(m1, m2, h);
            int b2 = HsbIntermediateValuesToRgbComponent(m1, m2, h - 1.0f / 3);

            return (unchecked((int)0xFF000000) | (r << 16) | (g << 8) | b2);
        }

        public static int makeColor(int r, int g, int b) {
            return (int)((0xFF000000) | ((uint)r << 16) | ((uint)g << 8) | ((uint)b));
        }

        public unsafe static void render(Render.Image dest, int x, int y, Render.Image src, bool drawZero) {
            render(dest, x, y, src.Width, src.Height, src.Pixels, drawZero);
        }

        private unsafe static bool clip(ref int x0, ref int y0, ref int xlen, ref int ylen, ref int* s, ref int* d, int spitch, int dpitch, int cx1, int cx2, int cy1, int cy2) {
            if (x0 >= cx2 || y0 >= cy2 || x0 + xlen <= cx1 || y0 + ylen <= cy1) {
                return true;
            }

            if (x0 + xlen > cx2) {
                xlen = cx2 - x0;
            }
            if (y0 + ylen > cy2) {
                ylen = cy2 - y0;
            }

            if (x0 < cx1) {
                s += (cx1 - x0);
                xlen -= (cx1 - x0);
                x0 = cx1;
            }
            if (y0 < cy1) {
                s += (cy1 - y0) * spitch;
                ylen -= (cy1 - y0);
                y0 = cy1;
            }

            d += (y0 * dpitch) + x0;

            return false;
        }

        private static void handlePixel(int src, ref int dest, int op, bool mixFlag, bool tflag) {
            if (mixFlag) {
                if (tflag) {
                    if ((src & 0x00FFFFFF) != 0x00FF00FF) {
                        dest = mixPixel(src, dest, op);
                    }
                } else dest = mixPixel(src, dest, op);
            } else {
                if (tflag) {
                    if ((src & 0x00FFFFFF) != 0x00FF00FF) {
                        dest = src;
                    }
                } else {
                    dest = src;
                }
            }
        }

        private static int mixPixel(int src, int dest, int op) {
            const int MASK = unchecked((int)0xFF000000);
            switch (op) {
                case 3: {
                        int r = (((src & 0x00FF0000) * 3 + (dest & 0x00FF0000)) >> 2) & 0x00FF0000;
                        int g = (((src & 0x0000FF00) * 3 + (dest & 0x0000FF00)) >> 2) & 0x0000FF00;
                        int b = (((src & 0x000000FF) * 3 + (dest & 0x000000FF)) >> 2) & 0x000000FF;
                        return (MASK) | (r | g | b);
                    }
                case 5: {
                        int r = ((src & 0x00FF0000) + (dest & 0x00FF0000)) & 0x01FE0000;
                        int g = ((src & 0x0000FF00) + (dest & 0x0000FF00)) & 0x0001FE00;
                        int b = ((src & 0x000000FF) + (dest & 0x000000FF)) & 0x000001FE;
                        return (MASK) | ((r | g | b) >> 1);
                    }
                case 1:
                    return src;

                case 2:
                    return dest;

                case 4:
                    return (int)((~(uint)dest) | 0xFF000000);

                default:
                    return 0;
            }
        }

        public static unsafe void render(Render.Image dest, int x, int y, int xlen, int ylen, int[] pixels, bool drawZero) {
            fixed (int* p = pixels) {
                render(dest, x, y, xlen, ylen, p, drawZero);
            }
        }

        public static unsafe void render(Render.Image dest, int x, int y, int xlen, int ylen, int* pixels, bool drawZero) {
            int* s = pixels;
            int* d = dest.Pixels;

            int spitch = xlen;
            int dpitch = dest.Pitch;

            if (clip(ref x, ref y, ref xlen, ref ylen, ref s, ref d, spitch, dpitch, 0, dest.Width, 0, dest.Height)) {
                return;
            }

            for (; ylen != 0; ylen--) {
                for (int xx = 0; xx < xlen; xx++) {
                    handlePixel(s[xx], ref d[xx], 0, false, !drawZero);
                }

                s += spitch;
                d += dpitch;
            }
        }

        public static void renderBox(Render.Image img, int x0, int y0, int w, int h, int color, Render.PixelOp op) {
            renderSolid(img, x0, y0, w, 1, color, op);
            renderSolid(img, x0, y0 + h - 1, w, 1, color, op);
            renderSolid(img, x0, y0, 1, h, color, op);
            renderSolid(img, x0 + w - 1, y0, 1, h, color, op);
        }

        public unsafe static void renderColoredStippleTile(Render.Image img, int x0, int y0, int color1, int color2) {
            int xlen = 16;
            int ylen = 16;

            int* s = null;
            int* d = img.Pixels;

            int dpitch = img.Pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, 0, dpitch, 0, img.Width, 0, img.Height)) {
                return;
            }

            for (; ylen > 0; ylen--) {
                for (int x = 0; x < xlen; x++) {
                    if (((ylen ^ x) & 1) != 0) {
                        d[x] = color1;
                    } else {
                        d[x] = color2;
                    }
                }
                d += dpitch;
            }
        }

        public unsafe static void renderColoredTile(Render.Image img, int x0, int y0, int color) {
            //pr2.Render.renderColoredTile(img, x0, y0, color);
            //return;

            int xlen = 16;
            int ylen = 16;

            int* s = null;
            int* d = img.Pixels;

            int dpitch = img.Pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, 0, dpitch, 0, img.Width, 0, img.Height))
                return;

            for (; ylen > 0; ylen--) {
                for (int x = 0; x < xlen; x++) {
                    d[x] = color;
                }
                d += dpitch;
            }
        }

        public unsafe static void renderColoredTile_50Alpha(Render.Image img, int x0, int y0, int color) {
            int xlen = 16;
            int ylen = 16;

            int* s = null;
            int* d = img.Pixels;

            int dpitch = img.Pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, 0, dpitch, 0, img.Width, 0, img.Height)) {
                return;
            }

            for (; ylen > 0; ylen--) {
                for (int x = 0; x < xlen; x++) {
                    handlePixel(color, ref d[x], (int)Render.PixelOp.Alpha50, true, false);
                }

                d += dpitch;
            }

        }

        public unsafe static void renderColorPicker(Render.Image img, float h) {
            //pr2.Render.renderColorPicker(img, h);
            //return;

            int* dst = img.Pixels;
            for (int y = 0; y < 256; y++) {
                for (int x = 0; x < 256; x++) {
                    *dst = HsbToColor(h, (float)x / 256, (float)y / 256);
                    dst++;
                }
            }
        }

        public unsafe static void renderNumber(Render.Image img, int x0, int y0, int number, int color) {
            int height = img.Height;
            int width = img.Width;
            int* pixels = img.Pixels;

            char[] digits = number.ToString().ToCharArray();
            foreach (char c in digits) {
                int cn = c - '0';
                byte[] ba = BiosFont.Number(cn);

                int glyphWidth = ba[0];
                int glyphHeight = (ba.Length - 1) / glyphWidth;

                for (int y = 0; y < glyphHeight; y++) {
                    if (y0 + y < 0 || y0 + y >= height) continue;
                    for (int x = 0; x < glyphWidth; x++) {
                        if (x0 + x < 0 || x0 + x >= width) continue;
                        if (ba[1 + (y * glyphWidth) + x] == 1) {
                            pixels[(y0 + y) * width + (x0 + x)] = color;
                        }
                    }
                }
                x0 += glyphWidth + 1;
            }
        }

        public unsafe static void renderObsTile(Render.Image img, int x0, int y0, int[] obsdata, bool clearbuf, int color) {
            fixed (int* p = obsdata) {
                renderObsTile(img, x0, y0, p, clearbuf, color);
            }
        }

        public unsafe static void renderObsTile(Render.Image img, int x0, int y0, int* obsdata, bool clearbuf, int color) {
            int xlen = 16;
            int ylen = 16;

            const int WHITE = unchecked((int)0xFFFFFFFF);
            const int BLACK = unchecked((int)0xFF000000);

            int* s = obsdata;
            int* d = img.Pixels;

            const int spitch = 16;
            int dpitch = img.Pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, spitch, dpitch, 0, img.Width, 0, img.Height)) {
                return;
            }

            if (clearbuf) {
                for (; ylen > 0; ylen--) {
                    for (int x = 0; x < xlen; x++) {
                        d[x] = (s[x] != 0) ? WHITE : BLACK;
                    }
                    s += spitch;
                    d += dpitch;
                }
            } else {
                for (; ylen > 0; ylen--) {
                    for (int x = 0; x < xlen; x++) {
                        if (s[x] != 0) {
                            handlePixel(color, ref d[x], (int)Render.PixelOp.Alpha50, true, false);
                        }
                    }

                    s += spitch;
                    d += dpitch;
                }
            }
        }

        public unsafe static void renderObsTileFast(Render.Image img, int x0, int y0, int[] obsdata, bool clearbuf) {
            fixed (int* p = obsdata) {
                renderObsTileFast(img, x0, y0, p, clearbuf);
            }
        }

        public unsafe static void renderObsTileFast(Render.Image img, int x0, int y0, int* obsdata, bool clearbuf) {
            int xlen = 16;
            int ylen = 16;

            const int WHITE = unchecked((int)0xFFFFFFFF);
            const int BLACK = unchecked((int)0xFF000000);

            int* s = obsdata;
            int* d = img.Pixels;

            const int spitch = 16;
            int dpitch = img.Pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, spitch, dpitch, 0, img.Width, 0, img.Height)) {
                return;
            }

            if (clearbuf) {
                for (; ylen > 0; ylen--) {
                    for (int x = 0; x < xlen; x++) {
                        d[x] = (s[x] != 0) ? WHITE : BLACK;
                    }
                    s += spitch;
                    d += dpitch;
                }
            } else {
                for (; ylen > 0; ylen--) {
                    for (int x = 0; x < xlen; x++) {
                        if (s[x] != 0) d[x] = WHITE;
                    }
                    s += spitch;
                    d += dpitch;
                }
            }

        }

        public unsafe static void renderSolid(Render.Image img, int x0, int y0, int w, int h, int color, Render.PixelOp op) {
            int bw = img.Width;
            int bh = img.Height;
            int bp = img.Pitch;

            for (int y = 0; y < h; y++) {
                if (y + y0 >= 0 && y + y0 < bh) {
                    for (int x = 0; x < w; x++) {
                        if (x + x0 >= 0 && x + x0 < bw) {
                            handlePixel(color, ref img.Pixels[(y0 + y) * bp + x0 + x], (int)op, true, false);
                        }
                    }
                }
            }
        }

        public unsafe static void renderTile32(Render.Image img, int x0, int y0, int* tiledata, bool drawZero) {
            render(img, x0, y0, 16, 16, tiledata, drawZero);
        }

        public enum PixelOp {
            Src = 1,
            Dest = 2,
            Alpha50 = 3,
            DestInvert = 4,
            Alpha75 = 5,
        }

        /*
        public unsafe class Image : IDisposable {
            public int* buf;
            public int height;
            public int pitch;
            public int stride;
            public int width;

            public Image();

            public void clear(int color);
            public static Render.Image create(Bitmap bmp);
            public static Render.Image create(int width, int height);
            public override void Dispose();
            public int[] getArray();
            public Bitmap getBitmap();
            public static Render.Image lockBitmap(Bitmap bmp);
        }
        */
    }
}
