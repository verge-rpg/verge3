using System;
using System.Collections.Generic;
using System.Text;

using NUnit.Framework;
using winmaped2;

namespace winmaped2.Tests {
    [TestFixture]
    public class RendererTests : RenderFixture {
        [Test]
        public void DrawImage() {
            int[] pixels = new int[16 * 16];
            for (int i = 0; i < pixels.Length; i++) {
                pixels[i] = 1;
            }
            Canvas img = new Canvas(16, 16, pixels);

            pr2.RenderImage destImage = pr2.RenderImage.create(16, 16);

            Render.render(destImage, 0, 0, img, true);

            int[] resultPixels = destImage.getArray();
            Assert.AreEqual(pixels, resultPixels);
        }

        [Test]
        public void RenderDrawsDeathMagenta() {
            Canvas src = new Canvas(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.RenderImage dest = pr2.RenderImage.create(16, 16);

            Render.render(dest, 0, 0, src, true);
            Assert.AreEqual(dest.getArray(), src.Pixels);
        }

        [Test]
        public void RenderSkipsDeathMagenta() {
            Canvas src = new Canvas(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.RenderImage dest = pr2.RenderImage.create(16, 16);
            dest.clear(BLACK);

            Render.render(dest, 0, 0, src, false);
            Assert.AreEqual(dest.getArray(), CreatePixels(16, 16, BLACK));
        }

        [Test]
        public void RenderStipple() {
            pr2.RenderImage dest = pr2.RenderImage.create(16, 16);

            Render.renderColoredStippleTile(dest, 0, 0, GREEN, WHITE);
            Assert.AreEqual(WHITE, dest.getPixel(0, 0));
            Assert.AreEqual(GREEN, dest.getPixel(1, 0));
            Assert.AreEqual(WHITE, dest.getPixel(2, 0));
            Assert.AreEqual(GREEN, dest.getPixel(3, 0));

            Assert.AreEqual(GREEN, dest.getPixel(0, 1));
            Assert.AreEqual(WHITE, dest.getPixel(1, 1));
        }

        [Test]
        public void RenderStippleClipping() {
            pr2.RenderImage dest = pr2.RenderImage.create(14, 14);
            dest.clear(BLACK);

            Render.renderColoredStippleTile(dest, 4, 4, GREEN, WHITE);
            Assert.AreEqual(BLACK, dest.getPixel(0, 0));
            Assert.AreEqual(BLACK, dest.getPixel(1, 1));
            Assert.AreEqual(BLACK, dest.getPixel(2, 2));
            Assert.AreEqual(BLACK, dest.getPixel(3, 3));

            Assert.AreEqual(WHITE, dest.getPixel(4, 4));
            Assert.AreEqual(GREEN, dest.getPixel(5, 4));
            Assert.AreEqual(WHITE, dest.getPixel(6, 4));
            Assert.AreEqual(GREEN, dest.getPixel(7, 4));

            Assert.AreEqual(GREEN, dest.getPixel(4, 5));
            Assert.AreEqual(WHITE, dest.getPixel(5, 5));
            Assert.AreEqual(GREEN, dest.getPixel(6, 5));
            Assert.AreEqual(WHITE, dest.getPixel(7, 5));
        }
    }
}
