using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2.code {
    class Renderer {
        public Renderer(pr2.Render.Image backBuffer) {
            this.backBuffer = backBuffer;
        }

        public unsafe void drawImage(Image src, int x, int y, bool drawZero) {
            fixed (int* tiledata = src.Pixels) {
                pr2.Render.renderTile32(backBuffer, x, y, tiledata, drawZero);
            }
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
