using System;
using System.Collections.Generic;
using System.Text;

namespace winmaped2 {
    class RenderCache {
        Vsp24 vsp;
        Image[] tileImages;
        Image[] obstructionImages;

        public RenderCache(Vsp24 vsp) {
            this.vsp = vsp;

            tileImages = new winmaped2.Image[vsp.tileCount];
            obstructionImages = new Image[vsp.ObstructionTiles.Count];
        }

        public Image GetTileImage(int index) {
            if (tileImages[index] == null) {
                tileImages[index] = new Image(vsp.getPixels(index));
            }
            return tileImages[index];
        }

        public Image GetObstructionTileImage(int index) {
            if (obstructionImages[index] == null) {
                obstructionImages[index] = new Image(GetObstructionPixels(index));
            }
            return obstructionImages[index];
        }

        private int[] GetObstructionPixels(int index) {
            return ((VspObstructionTile)vsp.ObstructionTiles[index]).Pixels;
        }

        public int TileCount {
            get {
                return vsp.tileCount;
            }
        }
    }
}
