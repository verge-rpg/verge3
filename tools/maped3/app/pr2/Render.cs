using System;
using System.Drawing;
using System.Drawing.Imaging;

namespace winmaped2 {
    public sealed class Render {
        private Render() { }

        public static Bitmap createBitmap(int width, int height) {
            return new Bitmap(width, height, PixelFormat.Format32bppArgb);
        }

        public static int HsbIntermediateValuesToRgbComponent(float m1, float m2, float h) {
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

        public static int HsbToColor(float h, float s, float b) {
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

        public unsafe static void render(pr2.Render.Image dest, int x, int y, pr2.Render.Image src, bool drawZero) {
            render(dest, x, y, src.width, src.height, src.buf, drawZero);
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

        public static unsafe void render(pr2.Render.Image dest, int x, int y, int xlen, int ylen, int* pixels, bool drawZero) {
            int* s = pixels;
            int* d = dest.buf;

            int spitch = xlen;
            int dpitch = dest.pitch;

            if (clip(ref x, ref y, ref xlen, ref ylen, ref s, ref d, spitch, dpitch, 0, dest.width, 0, dest.height)) {
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

        public static void renderBox(pr2.Render.Image img, int x0, int y0, int w, int h, int color, pr2.Render.PixelOp op) {
            renderSolid(img, x0, y0, w, 1, color, op);
            renderSolid(img, x0, y0 + h - 1, w, 1, color, op);
            renderSolid(img, x0, y0, 1, h, color, op);
            renderSolid(img, x0 + w - 1, y0, 1, h, color, op);
        }

        public unsafe static void renderColoredStippleTile(pr2.Render.Image img, int x0, int y0, int color1, int color2) {
            int xlen = 16;
            int ylen = 16;

            int* s = null;
            int* d = img.buf;

            int dpitch = img.pitch;

            if (clip(ref x0, ref y0, ref xlen, ref ylen, ref s, ref d, 0, dpitch, 0, img.width, 0, img.height)) {
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

        public static void renderColoredTile(pr2.Render.Image img, int x0, int y0, int color) {
            pr2.Render.renderColoredTile(img, x0, y0, color);
        }

        public static void renderColoredTile_50Alpha(pr2.Render.Image img, int x0, int y0, int color) {
            pr2.Render.renderColoredTile_50Alpha(img, x0, y0, color);
        }

        public static void renderColorPicker(pr2.Render.Image img, float h) {
            pr2.Render.renderColorPicker(img, h);
        }

        public static void renderNumber(pr2.Render.Image img, int x0, int y0, int number, int color) {
            pr2.Render.renderNumber(img, x0, y0, number, color);
        }

        public unsafe static void renderObsTile(pr2.Render.Image img, int x0, int y0, int* obsdata, bool clearbuf, int color) {
            pr2.Render.renderObsTile(img, x0, y0, obsdata, clearbuf, color);
        }

        public unsafe static void renderObsTileFast(pr2.Render.Image img, int x0, int y0, int* obsdata, bool clearbuf) {
            pr2.Render.renderObsTileFast(img, x0, y0, obsdata, clearbuf);
        }

        public unsafe static void renderSolid(pr2.Render.Image img, int x0, int y0, int w, int h, int color, pr2.Render.PixelOp op) {
            int bw = img.width;
            int bh = img.height;
            int bp = img.pitch;

            for (int y = 0; y < h; y++) {
                if (y + y0 >= 0 && y + y0 < bh) {
                    for (int x = 0; x < w; x++) {
                        if (x + x0 >= 0 && x + x0 < bw) {
                            handlePixel(color, ref img.buf[(y0 + y) * bp + x0 + x], (int)op, true, false);
                        }
                    }
                }
            }
        }

        public unsafe static void renderTile32(pr2.Render.Image img, int x0, int y0, int* tiledata, bool drawZero) {
            render(img, x0, y0, 16, 16, tiledata, drawZero);
        }

        public unsafe static void renderTile32_Mix(pr2.Render.Image img, int x0, int y0, int* tiledata, bool drawZero, pr2.Render.PixelOp op) {
            pr2.Render.renderTile32_Mix(img, x0, y0, tiledata, drawZero, op);
        }

        public static void renderTileCross(pr2.Render.Image img, int x0, int y0) {
            pr2.Render.renderTileCross(img, x0, y0);
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
            public static pr2.Render.Image create(Bitmap bmp);
            public static pr2.Render.Image create(int width, int height);
            public override void Dispose();
            public int[] getArray();
            public Bitmap getBitmap();
            public static pr2.Render.Image lockBitmap(Bitmap bmp);
            public static pr2.Render.Image lockRead(Bitmap bmp);
            public static pr2.Render.Image lockWrite(Bitmap bmp);
        }
        */
    }
}
