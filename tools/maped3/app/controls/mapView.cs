using System;
using System.Windows.Forms;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace winmaped2 {

    public class mapView : Control {
        System.Timers.Timer slideTimer;

        private bool bDragging = false;
        private bool bDraggingTool = false;
        bool bSliding;
        bool bSlidingLeft, bSlidingRight, bSlidingDown, bSlidingUp;

        private Plugins.MapEventInfo currMapEventInfo;
        private Plugins.IMapPlugin currMapPlugin = null;
        private Plugins.IMapTool currMapTool = null;
        private Plugins.IMapDragTool currMapDragTool = null;

        Size viewSize;

        Bitmap bmp = null;

        private bool live { get { return ParentMap != null; } }

        int last_cursorX = -1, last_cursorY = -1;


        private int cursorX, cursorY;
        private bool bMouseContained = false;
        private VScrollBar vscrollbar;
        private HScrollBar hscrollbar;
        public void SetScrollBars(VScrollBar vs, HScrollBar hs) {
            vscrollbar = vs;
            hscrollbar = hs;
        }

        public int ZoomLevel {
            get {
                return Global.zoom;
            }
            set {
                Global.zoom = value;
                CalculateScrollValues();
                Invalidate();
            }
        }


        Map _ParentMap = null;
        public Map ParentMap { get { return _ParentMap; } set { _ParentMap = value; CalculateScrollValues(); Invalidate(); } }

        public mapView() {
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.Opaque, true);

            slideTimer = new System.Timers.Timer(100);
            slideTimer.Elapsed += new System.Timers.ElapsedEventHandler(slideTimer_Tick);

        }

        class RenderCache {
            public int[][] tiles;
            winmaped2.code.Image[] tileImages;

            public void SetTileCount(int count) {
                tiles = new int[count][];
                tileImages = new winmaped2.code.Image[count];
            }

            public int[] GetTile(int index) {
                return tiles[index];
            }

            public winmaped2.code.Image getTileImage(int index) {
                if (tileImages[index] == null) {
                    tileImages[index] = new winmaped2.code.Image(GetTile(index));
                }
                return tileImages[index];
            }
        }

        int counter = 0;
        unsafe void renderLayer(pr2.Render.Image img, MapLayer ml, int px, int py, RenderCache rc, bool drawzero, winmaped2.code.Renderer ren) {
            switch (ml.type) {
                case LayerType.Tile:
                    renderTileLayer(ren, ml, px, py, rc, drawzero);
                    break;
                case LayerType.Zone:
                    renderZoneLayer(img, ml, px, py, rc);
                    break;
                case LayerType.Obs:
                    renderObstructionLayer(img, ml, px, py);
                    break;
            }
        }

        unsafe private void renderObstructionLayer(pr2.Render.Image img, MapLayer ml, int px, int py) {
            int mtx = px / 16;
            int mty = py / 16;
            int mtox = px & 15;
            int mtoy = py & 15;

            //we add 2; one for the case where we are scrolled a little bit
            //(and so parts of two tiles are drawn instead of one complete)
            //and one for the case where the screen is a funny size and a remainder bit is shown
            int tw = img.width / 16 + 2;
            int th = img.height / 16 + 2;

            int layerWidth = ml.Width;
            int layerHeight = ml.Height;
            int cpx = -mtox;
            int cpy = -mtoy;

            tw = System.Math.Min(tw, layerWidth - mtx);
            th = System.Math.Min(th, layerHeight - mty);

            int tp;
            int tile;
            int xmin = -mtox;
            int xmax = xmin + tw * 16;

            fixed (short* tileMap = ml.Data)
                if (Global.RenderOptions.bTranslucentEffects) {
                    for (int ty = 0; ty < th; ty++, cpy += 16) {
                        tp = (ty + mty) * layerWidth + mtx;
                        for (cpx = xmin; cpx < xmax; cpx += 16)
                            if ((tile = tileMap[tp++]) != 0)
                                if (tile < ParentMap.vsp.ObstructionTiles.Count)
                                    fixed (int* tiledata = ((VspObstructionTile)ParentMap.vsp.ObstructionTiles[tile]).Pixels) {
                                        pr2.Render.renderObsTile(img, cpx, cpy, tiledata, false, UserPrefs.ObsColor);
                                    }
                    }
                } else {
                    for (int ty = 0; ty < th; ty++, cpy += 16) {
                        tp = (ty + mty) * layerWidth + mtx;
                        for (cpx = xmin; cpx < xmax; cpx += 16)
                            if ((tile = tileMap[tp++]) != 0)
                                if (tile < ParentMap.vsp.ObstructionTiles.Count)
                                    fixed (int* tiledata = ((VspObstructionTile)ParentMap.vsp.ObstructionTiles[tile]).Pixels) {
                                        pr2.Render.renderObsTileFast(img, cpx, cpy, tiledata, false);
                                    }
                    }
                }
        }

        unsafe private static void renderZoneLayer(pr2.Render.Image img, MapLayer ml, int px, int py, RenderCache rc) {
            int mtx = px / 16;
            int mty = py / 16;
            int mtox = px & 15;
            int mtoy = py & 15;

            //we add 2; one for the case where we are scrolled a little bit
            //(and so parts of two tiles are drawn instead of one complete)
            //and one for the case where the screen is a funny size and a remainder bit is shown
            int tw = img.width / 16 + 2;
            int th = img.height / 16 + 2;

            int layerWidth = ml.Width;
            int layerHeight = ml.Height;
            int cpx = -mtox;
            int cpy = -mtoy;

            tw = System.Math.Min(tw, layerWidth - mtx);
            th = System.Math.Min(th, layerHeight - mty);

            int tp;
            int tile;
            int xmin = -mtox;
            int xmax = xmin + tw * 16;

            fixed (short* tileMap = ml.Data)
                for (int ty = 0; ty < th; ty++, cpy += 16) {
                    tp = (ty + mty) * layerWidth + mtx;
                    if (Global.RenderOptions.bTranslucentEffects) {
                        for (cpx = xmin; cpx < xmax; cpx += 16)
                            if ((tile = tileMap[tp++]) != 0)
                                if (tile < rc.tiles.Length)
                                    fixed (int* tiledata = rc.tiles[tile]) {
                                        pr2.Render.renderColoredTile_50Alpha(img, cpx, cpy, UserPrefs.ZonesColor);
                                        pr2.Render.renderNumber(img, cpx, cpy, tile, unchecked((int)0xFFFFFFFF));
                                    }
                    } else {
                        for (cpx = xmin; cpx < xmax; cpx += 16)
                            if ((tile = tileMap[tp++]) != 0)
                                if (tile < rc.tiles.Length)
                                    fixed (int* tiledata = rc.tiles[tile]) {
                                        pr2.Render.renderColoredTile(img, cpx, cpy, UserPrefs.ZonesColor);
                                        pr2.Render.renderNumber(img, cpx, cpy, tile, unchecked((int)0xFFFFFFFF));
                                    }
                    }
                }
        }

        unsafe private static void renderTileLayer(winmaped2.code.Renderer ren, MapLayer layer, int px, int py, RenderCache rc, bool drawZero) {
            int mtx = px / 16;
            int mty = py / 16;
            int mtox = px & 15;
            int mtoy = py & 15;

            //we add 2; one for the case where we are scrolled a little bit
            //(and so parts of two tiles are drawn instead of one complete)
            //and one for the case where the screen is a funny size and a remainder bit is shown
            int tw = ren.Width / 16 + 2;
            int th = ren.Height / 16 + 2;

            int layerWidth = layer.Width;
            int layerHeight = layer.Height;
            int cpx = -mtox;
            int cpy = -mtoy;

            tw = System.Math.Min(tw, layerWidth - mtx);
            th = System.Math.Min(th, layerHeight - mty);

            int tp;
            int tile;
            int xmin = -mtox;
            int xmax = xmin + tw * 16;

            fixed (short* tileMap = layer.Data) {
                for (int ty = 0; ty < th; ty++, cpy += 16) {
                    tp = (ty + mty) * layerWidth + mtx;
                    for (cpx = xmin; cpx < xmax; cpx += 16) {
                        tile = tileMap[tp++];
                        if (Global.RenderOptions.bAnimate) {
                            tile = Global.FrameCalc.getframe(tile);
                        }

                        if (drawZero || tile != 0 && tile < rc.tiles.Length) {
                            ren.drawImage(rc.getTileImage(tile), cpx, cpy, false);
                        }
                    }
                }
            }
        }

        private void renderEntities(pr2.Render.Image img, MapLayer ml, int px, int py) {
            int mtx = px / 16;
            int mty = py / 16;
            int mtox = px & 15;
            int mtoy = py & 15;
            int tw = img.width / 16 + 2;
            int th = img.height / 16 + 2;

            foreach (MapEntity me in ParentMap.Entities) {
                int tx = me.TileX;
                int ty = me.TileY;

                if (tx >= mtx && tx <= mtx + tw && ty >= mty && ty <= mty + th) {
                    int cx = -mtox + (tx - mtx) * 16;
                    int cy = -mtoy + (ty - mty) * 16;
                    pr2.Render.renderColoredTile_50Alpha(img, cx, cy, UserPrefs.EntsColor);
                    pr2.Render.renderNumber(img, cx + 4, cy + 4, me.ID, unchecked((int)0xFFFFFFFF));
                }
            }
        }




        protected override void OnPaint(PaintEventArgs e) {
            if (!live)
                return;

            counter++;

            int xScroll = hscrollbar.Value;
            int yScroll = vscrollbar.Value;

            Map currMap = ParentMap;

            pr2.Render.Image img = pr2.Render.Image.lockBitmap(bmp);
            img.clear(unchecked((int)0xFF000000));

            RenderCache rc = new RenderCache();
            int tileCount = ParentMap.vsp.tileCount;
            rc.SetTileCount(tileCount);
            for (int i = 0; i < tileCount; i++)
                rc.tiles[i] = ParentMap.vsp.getPixels(i);

            Map mOld = null;
            MapLayer mlOld = null;
            if (currMapTool is Plugins.IMapTweaker) {
                mOld = currMapEventInfo.editedMap;
                mlOld = currMapEventInfo.editedLayer;
                currMapEventInfo.editedMap = mOld.tileCopy();
                currMapEventInfo.editedLayer = currMapEventInfo.editedMap.findLayer(mlOld.name);
                currMap = currMapEventInfo.editedMap;
                currMapEventInfo.bTweak = true;
                ((Plugins.IMapTweaker)currMapTool).tweakMap(currMapEventInfo);
                currMapEventInfo.bTweak = false;
            }

            bool bottomlayer = false;
            bool blayerfound = false;

            winmaped2.code.Renderer ren = new winmaped2.code.Renderer(img);

            for (int c = 0; c < ParentMap.RenderManager.Layers.Count; c++) {
                MapLayer mlCurr = (MapLayer)ParentMap.RenderManager.Layers[c];

                if (!blayerfound && mlCurr.type == LayerType.Tile) {
                    bottomlayer = true;
                    blayerfound = true;
                }

                if (!ParentMap.IsBrush)
                    if (!ParentMap.UIState[c].bRender) continue;

                if (mlCurr.type == LayerType.Entity)
                    renderEntities(img, mlCurr, xScroll, yScroll);

                if (mlCurr.type == LayerType.Tile || mlCurr.type == LayerType.Obs || mlCurr.type == LayerType.Zone) {
                    if (bDragging && currMapEventInfo.editedLayerIndex == c && currMapTool is Plugins.IMapPainter)
                    //if(Global.editedLayer == mlCurr && currMapTool is Plugins.IMapPainter)
						{
                        MapLayer mlOld2 = currMapEventInfo.editedLayer;
                        MapLayer mlTemp = mlOld2.copy();
                        currMapEventInfo.editedLayer = mlTemp;
                        currMapEventInfo.bTweak = true;
                        ((Plugins.IMapPainter)currMapTool).tweakLayer(currMapEventInfo);
                        currMapEventInfo.editedLayer = mlOld2;
                        currMapEventInfo.bTweak = false;
                        renderLayer(img, mlTemp, xScroll, yScroll, rc, bottomlayer, ren);

                        ((Plugins.IMapPainter)currMapTool).paintMap(currMapEventInfo, img);
                    } else
                        renderLayer(img, mlCurr, xScroll, yScroll, rc, bottomlayer, ren);
                    if (bottomlayer) bottomlayer = false;
                }
            }

            if (currMapTool is Plugins.IMapTweaker) {
                currMapEventInfo.editedMap = mOld;
                currMapEventInfo.editedLayer = mlOld;
            }

            img.Dispose();
            e.Graphics.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            e.Graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
            e.Graphics.CompositingMode = CompositingMode.SourceCopy;
            e.Graphics.CompositingQuality = CompositingQuality.HighSpeed;


            e.Graphics.DrawImage(bmp, 0, 0, bmp.Width * ZoomLevel, bmp.Height * ZoomLevel);




            if (!bDragging && bMouseContained) {
                Pen p = new Pen(Color.White, 2.0f);

                Plugins.MapCursorLocation mcl = new winmaped2.Plugins.MapCursorLocation();
                setupMapCursorLocation(ref mcl, cursorX, cursorY);

                int dx = (mcl.tx * 16 - xScroll);
                int dy = (mcl.ty * 16 - yScroll);

                //				if(dx != last_cursorX || dy != last_cursorY)
                //if (cursorX > 0 && cursorX < Size.Width && cursorY>0 && cursorY<Size.Height)
                e.Graphics.DrawRectangle(p, dx * ZoomLevel, dy * ZoomLevel, 16 * ZoomLevel, 16 * ZoomLevel);

                //				last_cursorX = dx;
                //				last_cursorY = dy;

            }

            if (currMapTool is Plugins.IMapPainter) {
                ((Plugins.IMapPainter)currMapTool).paintWindow(currMapEventInfo, e.Graphics);
            }

        }

        protected override void OnMouseEnter(EventArgs e) {
            bMouseContained = true;
            base.OnMouseEnter(e);
        }

        protected override void OnMouseLeave(EventArgs e) {
            bMouseContained = false;
            Invalidate();
            base.OnMouseLeave(e);
        }

        bool mdown = false;
        protected override void OnMouseDown(MouseEventArgs e) {
            if (!live || ParentMap.IsBrush)
                return;


            mdown = true;
            if (Global.editedLayer.type == LayerType.Entity) {

                if (Global.SelectedEntity != null) {
                    Global.SelectedEntity.TileX = Global.MapCursorLocation.X;
                    Global.SelectedEntity.TileY = Global.MapCursorLocation.Y;
                }
                return;
            }

            currMapPlugin = Global.toolPalette.currMapPlugin;
            if (currMapPlugin == null)
                return;


            //get the tool to be used
            currMapTool = currMapPlugin.getTool(
                    e.Button == MouseButtons.Left,
                    e.Button == MouseButtons.Right,
                    ((Control.ModifierKeys & Keys.Shift) != 0),
                    ((Control.ModifierKeys & Keys.Control) != 0),
                    ((Control.ModifierKeys & Keys.Alt) != 0)
                    );
            if (currMapTool == null)
                return;

            //ensure that the tool to be used works with the current edited layer
            MapLayer ml = null;

            foreach (LayerType t in currMapTool.layerTypes) {
                if (t == Global.editedLayer.type)
                    ml = Global.editedLayer;
            }

            if (ml == null) {
                currMapTool = null;
                return;
            }

            Capture = true;
            bDragging = true;
            ParentMap.bUpdating = true;


            //setup eventinfo
            currMapEventInfo = new Plugins.MapEventInfo();
            //currMapEventInfo.invalidateEvent += new EventHandler(invalidateEvent);

            currMapEventInfo.editedMap = ParentMap;
            currMapEventInfo.editedLayer = ml;
            currMapEventInfo.editedLayerIndex = ParentMap.Layers.IndexOf(ml);
            currMapEventInfo.lb = ((e.Button & MouseButtons.Left) > 0);
            currMapEventInfo.rb = ((e.Button & MouseButtons.Right) > 0);
            currMapEventInfo.opManager = Global.opManager;
            setupMapCursorLocation(ref currMapEventInfo.start, e.X, e.Y);
            setupMapCursorLocation(ref currMapEventInfo.current, e.X, e.Y);
            setupMapCursorLocation(ref currMapEventInfo.previous, e.X, e.Y);
            currMapEventInfo.bDirty = false;
            if (currMapTool is Plugins.IMapDragTool) {
                bDraggingTool = true;
                currMapDragTool = (Plugins.IMapDragTool)currMapTool;
                currMapDragTool.MouseDown(currMapEventInfo);
                currMapDragTool.MouseMove(currMapEventInfo);
                if (currMapDragTool is Plugins.IMapDragTileTool)
                    ((Plugins.IMapDragTileTool)currMapDragTool).MouseMoveTile(currMapEventInfo);
            }
            if (currMapTool is Plugins.IMapClickTool) {
                ((Plugins.IMapClickTool)currMapTool).MouseClick(currMapEventInfo);
            }
            if (currMapEventInfo.bDirty)
                Invalidate();
            currMapEventInfo.bDirty = false;
        }

        protected override void OnMouseMove(MouseEventArgs e) {

            cursorX = (e.X - ClientRectangle.Left);
            cursorY = (e.Y - ClientRectangle.Top);



            if (mdown && Global.editedLayer.type == LayerType.Entity) {
                if (Global.SelectedEntity != null) {
                    Global.SelectedEntity.TileX = Global.MapCursorLocation.X;
                    Global.SelectedEntity.TileY = Global.MapCursorLocation.Y;
                }
                Invalidate();
                return;
            }

            if (!live || ParentMap.IsBrush)
                return;


            if (bDragging) {
                bSlidingRight = (e.X >= ClientSize.Width);
                bSlidingLeft = (e.X < 0);
                bSlidingDown = (e.Y >= ClientSize.Height);
                bSlidingUp = (e.Y < 0);

                if (!bSliding && (bSlidingRight || bSlidingLeft || bSlidingDown || bSlidingUp)) {
                    bSliding = true;
                    slideTimer.Enabled = true;
                } else
                    if (bSliding && !(bSlidingRight || bSlidingLeft || bSlidingDown || bSlidingUp)) {
                        bSliding = false;
                        slideTimer.Enabled = false;
                    }



                doDraggingToolMouseMove(e.X, e.Y);
                if (currMapEventInfo.bDirty)
                    Invalidate();
                currMapEventInfo.bDirty = false;

            }

            //Invalidate();
            invalidateCursor();
        }

        private void invalidateCursor() {
            int xScroll = hscrollbar.Value;
            int yScroll = vscrollbar.Value;

            int dx = (PointToClient(Control.MousePosition).X * 16 - xScroll);
            int dy = (PointToClient(Control.MousePosition).Y * 16 - yScroll);

            if (dx != last_cursorX || dy != last_cursorY)
                Invalidate();

            last_cursorX = dx;
            last_cursorY = dy;
        }

        protected override void OnMouseUp(MouseEventArgs e) {
            if (!live || ParentMap.IsBrush)
                return;

            mdown = false;

            if (bDragging) {
                Cursor = Cursors.Default;
                Capture = false;
                bDragging = false;

                if (bSliding) {
                    bSliding = bSlidingLeft = bSlidingRight = bSlidingDown = bSlidingUp = false;
                    slideTimer.Enabled = false;
                }
            }

            if (bDraggingTool) {
                setupMapCursorLocation(ref currMapEventInfo.end, e.X, e.Y);
                currMapDragTool.MouseUp(currMapEventInfo);
                bDraggingTool = false;

                if (currMapEventInfo.bDirty)
                    Invalidate();
                currMapEventInfo.bDirty = false;
            }

            ParentMap.bUpdating = false;

            currMapPlugin = null;
            currMapTool = null;
            currMapDragTool = null;

        }

        private void setupMapCursorLocation(ref Plugins.MapCursorLocation mcl, int mx, int my) {
            mcl.m.X = mcl.mx = mx;
            mcl.m.Y = mcl.my = my;
            mcl.p.X = mcl.px = mx / ZoomLevel + hscrollbar.Value;
            mcl.p.Y = mcl.py = my / ZoomLevel + vscrollbar.Value;
            mcl.t.X = mcl.tx = mcl.px / 16;
            mcl.t.Y = mcl.ty = mcl.py / 16;
            Global.InvokeCursorMove(true, mcl.tx, mcl.ty);
        }

        private void doDraggingToolMouseMove(int x, int y) {
            if (bDraggingTool) {
                if (x >= ClientSize.Width)
                    x = ClientSize.Width;
                if (y >= ClientSize.Height)
                    y = ClientSize.Height;
                if (x < 0)
                    x = 0;
                if (y < 0)
                    y = 0;

                Point tOld = currMapEventInfo.current.t;
                currMapEventInfo.previous = currMapEventInfo.current;
                setupMapCursorLocation(ref currMapEventInfo.current, x, y);
                currMapDragTool.MouseMove(currMapEventInfo);
                if (currMapDragTool is Plugins.IMapDragTileTool && !tOld.Equals(currMapEventInfo.current.t))
                    ((Plugins.IMapDragTileTool)currMapDragTool).MouseMoveTile(currMapEventInfo);

            }
        }
        public void SeekTo(int tx, int ty) {
            int px = tx * 16, py = ty * 16;
            int sx = px - (Width / 2 / ZoomLevel), sy = py - (Height / 2 / ZoomLevel);


            int ssx = Math.Max(Math.Min(sx, hscrollbar.Maximum - hscrollbar.LargeChange + 1), 0);
            int ssy = Math.Max(Math.Min(sy, vscrollbar.Maximum - vscrollbar.LargeChange + 1), 0);

            hscrollbar.Value = ssx;
            vscrollbar.Value = ssy;
        }


        protected override Size DefaultSize { get { return new Size(640, 480); } }

        public void ForceRedraw() {
            Invalidate();
        }

        public void OnScroll(object sender, EventArgs e) {
            Invalidate();
            //Global.MiniMap.Invalidate();
        }

        public void CalculateScrollValues() {
            if (ParentMap == null || ParentMap.Layers.Count < 1) {
                return;
            }
            Map map = ParentMap;
            MapLayer bottom = ((MapLayer)map.Layers[0]);


            hscrollbar.Maximum = bottom.Width * 16;
            vscrollbar.Maximum = bottom.Height * 16;
            hscrollbar.LargeChange = Size.Width / ZoomLevel;
            vscrollbar.LargeChange = Size.Height / ZoomLevel;
        }

        public void UpdateSize() {
            if (bmp != null)
                bmp.Dispose();

            viewSize = Size;

            CalculateScrollValues();

            if (viewSize.Width == 0 || viewSize.Height == 0)
                return;

            int w = viewSize.Width / ZoomLevel;
            int h = viewSize.Height / ZoomLevel;
            if (w * ZoomLevel < viewSize.Width)
                w++;
            if (h * ZoomLevel < viewSize.Height)
                h++;

            bmp = new Bitmap(w, h, PixelFormat.Format32bppArgb);

            Invalidate();
        }

        public Point CursorPosition {
            get {
                //return new Point(((cursorX+scrollOffsetH)/16),(cursorY+scrollOffsetV)/16);
                return new Point(0, 0);
            }
        }

        private void invalidateEvent(object sender, EventArgs e) {
            Invalidate();
        }

        private void slideTimer_Tick(object sender, System.Timers.ElapsedEventArgs e) {
            if (bSlidingRight) {
                hscrollbar.Invoke((MethodInvoker)delegate() {
                    hscrollbar.Value = Math.Min(hscrollbar.Value + 16, hscrollbar.Maximum - hscrollbar.LargeChange + 1);
                }
                );
            }
            if (bSlidingLeft) {
                hscrollbar.Invoke((MethodInvoker)delegate() {
                    hscrollbar.Value = Math.Max(hscrollbar.Value - 16, 0);
                }
                );
            }
            if (bSlidingDown) {
                vscrollbar.Invoke((MethodInvoker)delegate() {
                    vscrollbar.Value = Math.Min(vscrollbar.Value + 16, vscrollbar.Maximum - vscrollbar.LargeChange + 1);
                }
                );
            }
            if (bSlidingUp) {
                vscrollbar.Invoke((MethodInvoker)delegate() {
                    vscrollbar.Value = Math.Max(vscrollbar.Value - 16, 0);
                }
                );
            }

            Invoke((MethodInvoker)delegate() {
                doDraggingToolMouseMove(PointToClient(Control.MousePosition).X, PointToClient(Control.MousePosition).Y);
            }
            );
        }
    }
}
