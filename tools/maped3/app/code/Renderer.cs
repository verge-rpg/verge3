using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    class Renderer {
        public Renderer(pr2.Render.Image backBuffer) {
            this.backBuffer = backBuffer;
        }

        public unsafe void render(Image src, int x, int y, bool drawZero) {
            fixed (int* pixels = src.Pixels) {
                Render.render(backBuffer, x, y, src.Width, src.Height, pixels, drawZero);
            }
        }

        public unsafe void renderTile32(Image src, int x, int y, bool drawZero) {
            fixed (int* tiledata = src.Pixels) {
                Render.renderTile32(backBuffer, x, y, tiledata, drawZero);
            }
        }

        public unsafe void renderColoredTile_50Alpha(int x0, int y0, int color) {
            Render.renderColoredTile_50Alpha(backBuffer, x0, y0, color);
        }

        public unsafe void renderNumber(int x0, int y0, int number, int color) {
            Render.renderNumber(backBuffer, x0, y0, number, color);
        }

        public unsafe void renderColoredTile(int x0, int y0, int color) {
            Render.renderColoredTile(backBuffer, x0, y0, color);
        }

        public unsafe void renderObsTile(Image src, int x0, int y0, bool clearbuf, int color) {
            fixed (int* obsdata = src.Pixels) {
                Render.renderObsTile(backBuffer, x0, y0, obsdata, clearbuf, color);
            }
        }

        public unsafe void renderObsTileFast(Image src, int x0, int y0, bool clearbuf) {
            fixed (int* obsdata = src.Pixels) {
                Render.renderObsTileFast(backBuffer, x0, y0, obsdata, clearbuf);
            }
        }

        public void renderBox(int x0, int y0, int w, int h, int color, pr2.Render.PixelOp op) {
            Render.renderBox(backBuffer, x0, y0, w, h, color, op);
        }

        public void renderColoredStippleTile(int x0, int y0, int color1, int color2) {
            Render.renderColoredStippleTile(backBuffer, x0, y0, color1, color2);
        }

        public void clear(int color) {
            backBuffer.clear(color);
        }

        public pr2.Render.Image BackBuffer {
            get {
                return backBuffer;
            }
        }

        public int Width {
            get {
                return BackBuffer.width;
            }
        }

        public int Height {
            get {
                return BackBuffer.height;
            }
        }

        private pr2.Render.Image backBuffer;
    }
}
