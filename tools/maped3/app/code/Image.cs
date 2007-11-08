using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2.code {
    class Image {
        public Image(int[] pixels) {
            this.pixels = pixels;
        }

        public int[] Pixels {
            get {
                return pixels;
            }
        }

        int[] pixels;
    }
}
