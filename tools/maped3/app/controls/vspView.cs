using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace winmaped2 {

    public class vspView : Control {
        /**************************************************/
        /* sub controls                                   */
        /**************************************************/
        VScrollBar scrollbar;
        VSPController.ControllerMode controller_mode;
        VSPController.ControllerType controller_type;
        TileViewer viewer_a, viewer_b;

        /**************************************************/
        /* variables                                      */
        /**************************************************/
        //int	TilesWide = 20, TilesHigh = 10;
        int TilesWide {
            get {
                return Width / 16;
            }
        }
        int TilesHigh {
            get {
                return Height / 16;
            }
        }
        int st0, st1;
        int scrollOffset;
        Vsp24 vsp;

        /**************************************************/
        /* public properties                              */
        /**************************************************/
        public int SelectedTileF {
            get {
                return st0;
            }
            set {
                if (value < 0) {
                    return;
                }
                st0 = value;
                Invalidate();
                if (viewer_a == null) {
                    return;
                }
                viewer_a.ActiveTile = (Vsp24Tile)vsp.Tiles[st0];
                if (SelectionChanged != null) {
                    SelectionChanged();
                }
            }
        }
        public int SelectedTileB {
            get {
                return st1;
            }
            set {
                if (value < 0) {
                    return;
                }
                st1 = value;
                Invalidate();
                if (viewer_b == null) {
                    return;
                }
                viewer_b.ActiveTile = (Vsp24Tile)vsp.Tiles[st1];
                if (SelectionChanged != null) {
                    SelectionChanged();
                }
            }
        }
        public VSPController.ControllerMode ControllerMode {
            get {
                return controller_mode;
            }
            set {
                controller_mode = value;
            }
        }
        public VSPController.ControllerType ControllerType {
            get {
                return controller_type;
            }
            set {
                controller_type = value;
            }
        }
        protected override Size DefaultSize {
            get {
                return new Size(320, 160);
            }
        }
        public TileViewer PrimaryTileViewer {
            get {
                return viewer_a;
            }
            set {
                viewer_a = value;
            }
        }
        public TileViewer SecondaryTileViewer {
            get {
                return viewer_b;
            }
            set {
                viewer_b = value;
            }
        }
        public Vsp24 ActiveVsp {
            get {
                return vsp;
            }
            set {
                vsp = value;
            }
        }

        public event SEventHandler SelectionChanged;

        /**************************************************/
        /* constructors                                   */
        /**************************************************/
        public vspView(VScrollBar scroller, VSPController.ControllerMode controller_mode, VSPController.ControllerType controller_type, TileViewer primaryTileViewer)
            : this(scroller, controller_mode, controller_type) {

        }
        public vspView(VScrollBar scroller, VSPController.ControllerMode controller_mode, VSPController.ControllerType controller_type, TileViewer primaryTileViewer, TileViewer secondaryTileViewer)
            : this(scroller, controller_mode, controller_type) {
        }
        public vspView(VScrollBar scroller, VSPController.ControllerMode controller_mode, VSPController.ControllerType controller_type)
            : this() {
            this.scrollbar = scroller;
            this.controller_mode = controller_mode;
        }
        public vspView() {
            Size = new Size(TilesWide * 16, TilesHigh * 16);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.Opaque, true);
            SetStyle(ControlStyles.UserMouse, true);
        }
        public void ResetView() {
            st0 = 0;
            st1 = 0;
            scrollOffset = 0;
            Invalidate();
        }
        protected override void OnPaint(PaintEventArgs e) {
            if (vsp == null) {
                return;
            }

            const int WHITE = unchecked((int)0xFFFFFFFF);

            e.Graphics.PixelOffsetMode = PixelOffsetMode.Half;
            e.Graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;

            CalculateScrollValues();

            Bitmap bmp = new Bitmap(TilesWide * 16, TilesHigh * 16, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            using (Render.Image qimg = Render.Image.lockBitmap(bmp)) {
                Renderer ren = new Renderer(qimg);

                if (ControllerType == VSPController.ControllerType.VSP) {
                    int row = 0, col = 0;
                    for (int i = scrollOffset / 16 * 20; i < vsp.Tiles.Count; i++) {
                        ren.render(vsp.GetTile(i).Image, col * 16, row * 16, true);
                        if (i == st0) {
                            if (controller_mode != VSPController.ControllerMode.ViewOnly) {
                                ren.renderBox(col * 16, row * 16, 16, 16, WHITE, Render.PixelOp.Src);
                                ren.renderBox(col * 16 + 1, row * 16 + 1, 14, 14, WHITE, Render.PixelOp.Src);
                            }
                        }
                        if (i == st1) {
                            if (controller_mode == VSPController.ControllerMode.SelectorDual) {
                                ren.renderBox(col * 16 + 2, row * 16 + 2, 12, 12, WHITE, Render.PixelOp.Src);
                                ren.renderBox(col * 16 + 3, row * 16 + 3, 10, 10, WHITE, Render.PixelOp.Src);
                            }
                        }
                        col++;
                        if (col == TilesWide) {
                            col = 0;
                            row++;
                            if (row == TilesHigh)
                                break;
                        }
                    }
                } else if (ControllerType == VSPController.ControllerType.Obstruction) {
                    // render obs tiles
                    int row = 0, col = 0;
                    for (int i = scrollOffset / 16 * 20; i < vsp.ObstructionTiles.Count; i++) {
                        VspObstructionTile vot = ((VspObstructionTile)vsp.ObstructionTiles[i]);
                        ren.renderObsTile(vot.Image, col * 16, row * 16, true, UserPrefs.ObsColor);
                        if (i == st0) {
                            if (controller_mode != VSPController.ControllerMode.ViewOnly) {
                                Render.renderBox(qimg, col * 16, row * 16, 16, 16, WHITE, Render.PixelOp.Src);
                                Render.renderBox(qimg, col * 16 + 1, row * 16 + 1, 14, 14, WHITE, Render.PixelOp.Src);
                            }
                        }
                        col++;
                        if (col == TilesWide) {
                            col = 0;
                            row++;
                            if (row == TilesHigh)
                                break;
                        }
                    }
                }
            }
            e.Graphics.DrawImage(bmp, 0, 0, Size.Width, Size.Height);
            bmp.Dispose();
        }

        protected override void OnMouseWheel(MouseEventArgs e) {
            if (scrollbar == null) return;
            int v = scrollbar.Value;
            if (e.Delta != 0) {
                v -= e.Delta;
                if (v < scrollbar.Minimum) v = scrollbar.Minimum;
                if (v > scrollbar.Maximum) v = scrollbar.Maximum;
                scrollbar.Value = v;
            }
        }
        bool trackingMouse = false;
        private void activateMouse(MouseEventArgs e) {
            int mx = e.X, my = e.Y;
            if (mx < 0) mx = 0;
            if (mx >= Width) mx = Width - 1;
            if (my < 0) my = 0;
            if (my >= Height) my = Height - 1;

            if (e.Button == MouseButtons.Left || e.Button == MouseButtons.Right) {
                if (ControllerType == VSPController.ControllerType.VSP) {
                    int tile = ((scrollOffset / 16) + (my / 16)) * TilesWide + (mx / 16);
                    if (tile >= vsp.tileCount) return;
                    Vsp24Tile vtile = (Vsp24Tile)vsp.Tiles[tile];
                    if (e.Button == MouseButtons.Left) {
                        if (viewer_a != null) {
                            viewer_a.ActiveTile = vtile;
                        }
                        st0 = tile;
                    } else if (e.Button == MouseButtons.Right) {
                        if (viewer_b != null) {
                            viewer_b.ActiveTile = vtile;
                        }
                        st1 = tile;
                    }
                } else if (ControllerType == VSPController.ControllerType.Obstruction) {
                    int tile = ((scrollOffset / 16) + (my / 16)) * TilesWide + (mx / 16);
                    if (tile >= vsp.ObstructionTiles.Count) {
                        return;
                    }
                    if (e.Button == MouseButtons.Left) {
                        st0 = tile;
                        if (viewer_a != null) {
                            viewer_a.ActiveObsTile = (VspObstructionTile)vsp.ObstructionTiles[tile];
                        }
                    }
                    if (e.Button == MouseButtons.Right) st1 = tile;
                }
                if (SelectionChanged != null) SelectionChanged();
                Invalidate();
            }
        }
        protected override void OnMouseUp(MouseEventArgs e) {
            trackingMouse = false;
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            if (vsp == null) {
                return;
            }
            if (trackingMouse) {
                activateMouse(e);
            }
        }

        protected override void OnMouseDown(MouseEventArgs e) {
            if (vsp == null) {
                return;
            }
            activateMouse(e);
            trackingMouse = true;
        }

        public void OnScroll(object sender, EventArgs e) {
            Invalidate();
            scrollOffset = scrollbar.Value;
        }

        public void CalculateScrollValues() {
            if (vsp == null) {
                if (scrollbar != null) {
                    scrollbar.Enabled = false;
                    return;
                }
            }

            // set values
            scrollbar.Minimum = 0;
            scrollbar.LargeChange = Height - 4;
            scrollbar.SmallChange = 16;
            if (ControllerType == VSPController.ControllerType.VSP) {
                if (vsp.Tiles.Count / TilesWide * 16 + 16 > Height) {
                    scrollbar.Maximum = vsp.Tiles.Count / TilesWide * 16 + 16;
                    scrollbar.Enabled = true;
                } else {
                    scrollbar.Enabled = false;
                }
            } else if (ControllerType == VSPController.ControllerType.Obstruction) {
                if (vsp.ObstructionTiles.Count / TilesWide * 16 + 16 > Height) {
                    scrollbar.Enabled = true;
                    scrollbar.Maximum = vsp.ObstructionTiles.Count / TilesWide * 16 + 16;
                } else {
                    scrollbar.Enabled = false;
                }
            }
        }
        public void ForceRedraw() {
            Invalidate();
        }
    }
}