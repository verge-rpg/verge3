using System;

using NUnit.Framework;
using winmaped2;
using winmaped2.pr2;

namespace winmaped2.Tests {
    [TestFixture]
    public class VspTest {
        [Test]
        public void CloneTile() {
            Vsp24 vsp = new Vsp24();
            Vsp24Tile tile = (Vsp24Tile)vsp.CreateTile();
            Vsp24Tile clone = (Vsp24Tile)tile.Clone();
            Assert.AreEqual(tile.ColorAverage, clone.ColorAverage);
            Assert.AreEqual(tile.Image.GetArray(), clone.Image.GetArray());
        }
    }
}
