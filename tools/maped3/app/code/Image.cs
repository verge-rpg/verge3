using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    public class Image {
        int[] pixels;
        int width;
        int height;

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

        public void UpdatePixels(int[] newPixels) {
            if (newPixels.Length != pixels.Length) {
                throw new Exception("Image.UpdatePixels failed: Buffer size mismatch");
            }
            pixels = newPixels;
        }

        public Image Clone() {
            return new Image(width, height, (int[])pixels.Clone());
        }

        public void SetPixel(int x, int y, int color) {
            if (!(0 <= x && x < width) ||
                !(0 <= y && y < height)) {
                throw new Exception(string.Format("SetPixel to invalid point {0},{1}", x, y));
            }

            int offset = y * width + x;
            pixels[offset] = color;
        }

        public int GetPixel(int x, int y) {
            if (!(0 <= x && x < width) ||
                            !(0 <= y && y < height)) {
                throw new Exception(string.Format("SetPixel to invalid point {0},{1}", x, y));
            }

            int offset = y * width + x;
            return pixels[offset];
        }
    }
}
