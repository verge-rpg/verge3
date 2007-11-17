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

            pr2.IRenderImage destImage = pr2.RenderImage.Create(16, 16);

            Render.render(destImage, 0, 0, img, true);

            int[] resultPixels = destImage.GetArray();
            Assert.AreEqual(pixels, resultPixels);
        }

        [Test]
        public void RenderDrawsDeathMagenta() {
            Canvas src = new Canvas(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.IRenderImage dest = pr2.RenderImage.Create(16, 16);

            Render.render(dest, 0, 0, src, true);
            Assert.AreEqual(dest.GetArray(), src.Pixels);
        }

        [Test]
        public void RenderSkipsDeathMagenta() {
            Canvas src = new Canvas(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.IRenderImage dest = pr2.RenderImage.Create(16, 16);
            dest.Clear(BLACK);

            Render.render(dest, 0, 0, src, false);
            Assert.AreEqual(dest.GetArray(), CreatePixels(16, 16, BLACK));
        }

        [Test]
        public void RenderStipple() {
            pr2.IRenderImage dest = pr2.RenderImage.Create(16, 16);

            Render.renderColoredStippleTile(dest, 0, 0, GREEN, WHITE);
            Assert.AreEqual(WHITE, dest.GetPixel(0, 0));
            Assert.AreEqual(GREEN, dest.GetPixel(1, 0));
            Assert.AreEqual(WHITE, dest.GetPixel(2, 0));
            Assert.AreEqual(GREEN, dest.GetPixel(3, 0));

            Assert.AreEqual(GREEN, dest.GetPixel(0, 1));
            Assert.AreEqual(WHITE, dest.GetPixel(1, 1));
        }

        [Test]
        public void RenderStippleClipping() {
            pr2.IRenderImage dest = pr2.RenderImage.Create(14, 14);
            dest.Clear(BLACK);

            Render.renderColoredStippleTile(dest, 4, 4, GREEN, WHITE);
            Assert.AreEqual(BLACK, dest.GetPixel(0, 0));
            Assert.AreEqual(BLACK, dest.GetPixel(1, 1));
            Assert.AreEqual(BLACK, dest.GetPixel(2, 2));
            Assert.AreEqual(BLACK, dest.GetPixel(3, 3));

            Assert.AreEqual(WHITE, dest.GetPixel(4, 4));
            Assert.AreEqual(GREEN, dest.GetPixel(5, 4));
            Assert.AreEqual(WHITE, dest.GetPixel(6, 4));
            Assert.AreEqual(GREEN, dest.GetPixel(7, 4));

            Assert.AreEqual(GREEN, dest.GetPixel(4, 5));
            Assert.AreEqual(WHITE, dest.GetPixel(5, 5));
            Assert.AreEqual(GREEN, dest.GetPixel(6, 5));
            Assert.AreEqual(WHITE, dest.GetPixel(7, 5));
        }
    }
}
