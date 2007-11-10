using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

using winmaped2;

namespace winmaped2.Tests {
    [TestFixture]
    public class ImageTest {
        [Test]
        public unsafe void CanDrawOnPr2Image() {
            int[] pixels = new int[16 * 16];
            for (int i = 0; i < 16 * 16; i++) {
                pixels[i] = 1;
            }
            Image img = new Image(16, 16, pixels);

            pr2.Render.Image dest = pr2.Render.Image.create(16, 16);
            fixed (int* tiledata = img.Pixels)
                pr2.Render.renderTile32(dest, 0, 0, tiledata, true);

            int[] resultPixels = dest.getArray();
            Assert.AreEqual(pixels, resultPixels);

            dest.Dispose();
        }
    }
}
