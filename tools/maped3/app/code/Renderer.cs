using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    class Renderer {
        public Renderer(Render.Image backBuffer) {
            this.backBuffer = backBuffer;
        }

        public void render(Image src, int x, int y, bool drawZero) {
            Render.render(backBuffer, x, y, src.Width, src.Height, src.Pixels, drawZero);
        }

        public void renderColoredTile_50Alpha(int x0, int y0, int color) {
            Render.renderColoredTile_50Alpha(backBuffer, x0, y0, color);
        }

        public void renderNumber(int x0, int y0, int number, int color) {
            Render.renderNumber(backBuffer, x0, y0, number, color);
        }

        public void renderColoredTile(int x0, int y0, int color) {
            Render.renderColoredTile(backBuffer, x0, y0, color);
        }

        public void renderObsTile(Image src, int x0, int y0, bool clearbuf, int color) {
            Render.renderObsTile(backBuffer, x0, y0, src.Pixels, clearbuf, color);
        }

        public void renderObsTileFast(Image src, int x0, int y0, bool clearbuf) {
            Render.renderObsTileFast(backBuffer, x0, y0, src.Pixels, clearbuf);
        }

        public void renderBox(int x0, int y0, int w, int h, int color, Render.PixelOp op) {
            Render.renderBox(backBuffer, x0, y0, w, h, color, op);
        }

        public void renderColoredStippleTile(int x0, int y0, int color1, int color2) {
            Render.renderColoredStippleTile(backBuffer, x0, y0, color1, color2);
        }

        public void clear(int color) {
            backBuffer.clear(color);
        }

        public Render.Image BackBuffer {
            get {
                return backBuffer;
            }
        }

        public int Width {
            get {
                return BackBuffer.Width;
            }
        }

        public int Height {
            get {
                return BackBuffer.Height;
            }
        }

        private Render.Image backBuffer;
    }
}
