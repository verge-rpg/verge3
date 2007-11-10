using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    public class Image {
        public Image(int width, int height, int[] pixels) {
            this.pixels = pixels;
            this.width = width;
            this.height = height;
        }

        public int[] Pixels {
            get {
                return pixels;
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

        int[] pixels;
        int width;
        int height;
    }
}
