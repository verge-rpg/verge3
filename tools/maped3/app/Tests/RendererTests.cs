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
            Image img = new Image(16, 16, pixels);

            pr2.Render.Image destImage = Render.Image.create(16, 16);
            Renderer ren = new Renderer(destImage);

            ren.renderTile32(img, 0, 0, true);

            int[] resultPixels = ren.BackBuffer.getArray();
            Assert.AreEqual(pixels, resultPixels);
        }

        [Test]
        public void RenderSizeIsImageSize() {
            const int width = 42;
            const int height = 99;
            pr2.Render.Image backBuffer = Render.Image.create(width, height);
            Renderer ren = new Renderer(backBuffer);
            Assert.AreEqual(ren.Width, backBuffer.Width);
            Assert.AreEqual(ren.Height, backBuffer.Height);
        }

        [Test]
        public void RenderDrawsDeathMagenta() {
            Image src = new Image(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.Render.Image dest = Render.Image.create(16, 16);

            Renderer ren = new Renderer(dest);
            ren.render(src, 0, 0, true);
            Assert.AreEqual(dest.getArray(), src.Pixels);
        }

        [Test]
        public void RenderSkipsDeathMagenta() {
            Image src = new Image(16, 16, CreatePixels(16, 16, MAGENTA));
            pr2.Render.Image dest = Render.Image.create(16, 16);
            dest.clear(BLACK);

            Renderer ren = new Renderer(dest);
            ren.render(src, 0, 0, false);
            Assert.AreEqual(dest.getArray(), CreatePixels(16, 16, BLACK));
        }

        [Test]
        public void RenderStipple() {
            pr2.Render.Image dest = Render.Image.create(16, 16);
            Renderer ren = new Renderer(dest);

            ren.renderColoredStippleTile(0, 0, GREEN, WHITE);
            Assert.AreEqual(WHITE, dest.getPixel(0, 0));
            Assert.AreEqual(GREEN, dest.getPixel(1, 0));
            Assert.AreEqual(WHITE, dest.getPixel(2, 0));
            Assert.AreEqual(GREEN, dest.getPixel(3, 0));

            Assert.AreEqual(GREEN, dest.getPixel(0, 1));
            Assert.AreEqual(WHITE, dest.getPixel(1, 1));
        }

        [Test]
        public void RenderStippleClipping() {
            pr2.Render.Image dest = Render.Image.create(14, 14);
            Renderer ren = new Renderer(dest);
            ren.clear(BLACK);

            ren.renderColoredStippleTile(4, 4, GREEN, WHITE);
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
