using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

using winmaped2;

namespace winmaped2.Tests {
    [TestFixture]
    public class ImageTest {
        const int BLACK = unchecked((int)0xFF000000);
        const int WHITE = unchecked((int)0xFFFFFFFF);
        const int GREEN = unchecked((int)0xFF00FF00);

        static int[] CreatePixels(int width, int height, int color) {
            int[] pixels = new int[width * height];
            for (int i = 0; i < width * height; i++) {
                pixels[i] = color;
            }
            return pixels;
        }

        int[] pixels;
        [SetUp]
        public void SetUp() {
            pixels = CreatePixels(16, 16, GREEN);
        }

        [Test]
        public unsafe void CanDrawOnPr2Image() {
            Image img = new Image(16, 16, pixels);

            pr2.Render.Image dest = pr2.Render.Image.create(16, 16);
            fixed (int* tiledata = img.Pixels)
                pr2.Render.renderTile32(dest, 0, 0, tiledata, true);

            int[] resultPixels = dest.getArray();
            Assert.AreEqual(pixels, resultPixels);

            dest.Dispose();
        }

        [Test]
        public void UpdatePixels() {
            Image img = new Image(16, 16, pixels);
            int[] blackPixels = CreatePixels(16, 16, BLACK);

            img.UpdatePixels(blackPixels);

            Assert.AreEqual(img.Pixels, blackPixels);

            int[] whitePixels = CreatePixels(32, 32, WHITE);
            try {
                img.UpdatePixels(whitePixels);
                Assert.Fail("Expected img.UpdatePixels(whitePixels) to fail");
            } catch (Exception) {
            }
        }

        [Test]
        public void Clone() {
            Image img = new Image(16, 16, CreatePixels(16, 16, BLACK));
            Image clone = img.Clone();

            Assert.AreNotSame(img, clone);
            Assert.AreNotSame(img.Pixels, clone.Pixels);
            Assert.AreEqual(img.Width, clone.Width);
            Assert.AreEqual(img.Height, clone.Height);
            Assert.AreEqual(img.Pixels, clone.Pixels);
        }
    }
}
