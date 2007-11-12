using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2.Tests {
    public class RenderFixture {
        public const int BLACK = unchecked((int)0xFF000000);
        public const int WHITE = BLACK | 0xFFFFFF;
        public const int MAGENTA = BLACK | 0xFF00FF;
        public const int GREEN = BLACK | 0x00FF00;

        public static int[] CreatePixels(int width, int height, int color) {
            int[] pixels = new int[width * height];
            for (int i = 0; i < width * height; i++) {
                pixels[i] = color;
            }
            return pixels;
        }

        public static int[] CreateStippledPixels(int width, int height, int color1, int color2) {
            int[] pixels = new int[width * height];
            for (int i = 0; i < width * height; i++) {
                pixels[i] = ((i & 1) != 0) ? color1 : color2;
            }
            return pixels;
        }
    }
}
