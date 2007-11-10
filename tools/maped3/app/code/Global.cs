using System;
using System.Windows.Forms;
using System.Text;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Collections;
using System.Collections.Generic;

namespace winmaped2 {
    /// <summary>
    /// Summary description for Global.
    /// not gonna happen, brother.
    /// </summary>
    public class Global {
        public class Misc {
            public static int[] sliceIntArrayImage(int[] arr, int sw, int x, int y, int w, int h) {
                int[] ret = new int[w * h];
                for (int yy = 0; yy < h; yy++)
                    for (int xx = 0; xx < w; xx++)
                        ret[yy * 16 + xx] = arr[(yy + y) * sw + xx + x];
                return ret;
            }
        }
        public class RenderOptions {
            public static bool bAnimate = false;
            public static bool bTranslucentEffects = true;
        }
        public static void OpenHelp(string url) {
            FileInfo fi = new FileInfo(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);
            string path = fi.Directory.FullName;
            path += "\\help\\" + url;
            try {
                System.Diagnostics.Process.Start(path);
            } catch {
                MessageBox.Show("Unable to start the help file. Perhaps this file is missing?", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

        }
        public static Operations.OperationManager opManager = new Operations.OperationManager();
        public static Plugins.PluginManager pluginManager = new Plugins.PluginManager();

        public static MapController MainMapController;
        public static MapController ClipboardMapController;

        static Map _clipboard;
        public static Map clipboard { get { return _clipboard; } set { _clipboard = value; if (ClipboardChanged != null) ClipboardChanged(); } }
        public static event SimpleEventHandler ClipboardChanged;

        public delegate void SimpleEventHandler();

        public class VERSIONINFO {
            public const string VERSIONSTRING_LONG = "VERGE MapEd 3.0.1";
            public const string VERSIONSTRING = "3.0.1";
            public const double VERSIONNUMBER = 3.0001;

            public const int MAPVERSION = 2;
        }

        static Map _ActiveMap;
        public static Map ActiveMap { get { return _ActiveMap; } set { _ActiveMap = value; if (ActiveMapChanged != null) ActiveMapChanged(); } }
        public static event SimpleEventHandler ActiveMapChanged;
        public static Vsp24 ActiveVsp;
        public static MapZone ActiveZone;
        public static int ActiveZoneIndex;
        //public static vspView VspViewer;

        public class FrameCalc {
            static System.Timers.Timer timer;
            static int ticks = 0;
            static Hashtable lookup = new Hashtable();
            public static event SimpleEventHandler OnTick;

            public static void generate(ArrayList al) {
                lookup.Clear();
                foreach (VspAnimation va in al) {
                    if (!lookup.ContainsKey(va.Start)) {
                        lookup.Add(va.Start, va);
                    }
                }
            }
            public static void init() {
                timer = new System.Timers.Timer(10);
                timer.Elapsed += new System.Timers.ElapsedEventHandler(timer_Elapsed);
                timer.Start();
            }
            public static int getframe(int tidx) {
                if (!lookup.ContainsKey(tidx))
                    return tidx;
                VspAnimation va = (VspAnimation)lookup[tidx];
                if (va.Delay == 0) return va.Start;
                switch (va.Mode) {
                    case Vsp24.ANIMATION_FORWARD:// forward
                        return va.Start + ((ticks / va.Delay) % (va.End - va.Start + 1));
                    case Vsp24.ANIMATION_PINGPONG:
                        int range = va.End - va.Start + 1;
                        range = range * 2 - 2;
                        range = (ticks / va.Delay) % range;
                        range -= (int)((va.End - va.Start + 1) / 2);
                        if (range > va.End - va.Start)
                            range -= 2;
                        return va.Start + Math.Abs(range);
                    case Vsp24.ANIMATION_REVERSE:
                        return va.End - ((ticks / va.Delay) % (va.End - va.Start + 1));

                    default: return tidx;
                }

            }

            static void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e) {
                ticks++;
                if (Global.ActiveMap == null) return;
                foreach (VspAnimation va in Global.ActiveMap.vsp.Animations) {
                    va.currTile = va.Start;
                }
                if (OnTick != null)
                    OnTick();
            }
        }
        public static bool IsBaseLayer(MapLayer ml) {
            return (Global.ActiveMap.Layers.IndexOf(ml) == 0);
        }

        static VSPController _vspc;
        static VSPController _ovspc;
        public static VSPController vspController {
            set { _vspc = value; }
            get { return _vspc; }
        }
        public static vspView VspViewer {
            get { if (_vspc != null) return _vspc.VspView; return null; }
        }
        public static VSPController obsVspController {
            set { _ovspc = value; }
            get { return _ovspc; }
        }
        public static vspView ObsVspViewer {
            get { if (_ovspc != null) return _ovspc.VspView; return null; }
        }

        public static mapView mapViewer {
            get { if (MainMapController != null) return MainMapController.MapView; return null; }
        }
        public static VScrollBar VspScrollBar;

        public static TileViewer TileViewerA;
        public static TileViewer TileViewerB;

        public static VScrollBar MapScrollBarV;
        public static HScrollBar MapScrollBarH;

        public static ToolPalette toolPalette;

        public static void initScrollBars(VScrollBar v, HScrollBar h) {
            MapScrollBarV = v;
            MapScrollBarH = h;
            MapScrollBarV.ValueChanged += new EventHandler(MapScrollBar_ValueChanged);
            MapScrollBarH.ValueChanged += new EventHandler(MapScrollBar_ValueChanged);
        }
        private static void OnScroll() {
            MiniMap.repaint();
            mapViewer.Invalidate();
        }

        public static MiniMapControl MiniMap;

        public static Panel layerTool;

        private static int m_zoom = 2;
        public static int zoom {
            get {
                return m_zoom;
            }
            set {
                m_zoom = value;
                if (mapViewer == null) return;
                mapViewer.UpdateSize();
                mapViewer.CalculateScrollValues();

                ClipboardMapController.MapView.UpdateSize();
                ClipboardMapController.MapView.CalculateScrollValues();

                if (zoomChanged != null)
                    zoomChanged();
            }
        }

        public static int getSelectedTileF(LayerType lt) {
            if (lt == LayerType.Tile)
                return Global.VspViewer.SelectedTileF;
            if (lt == LayerType.Obs)
                return Global.ObsVspViewer.SelectedTileF;
            if (lt == LayerType.Zone) {
                if (Global.SelectedZone != null)
                    return Global.SelectedZone.ID;
                else
                    return 0;
            }
            return 0;
        }
        public static int getSelectedTileB(LayerType lt) {
            if (lt == LayerType.Tile)
                return Global.VspViewer.SelectedTileB;
            return 0;
        }
        public static void setSelectedTileF(LayerType lt, int index) {
            if (lt == LayerType.Tile)
                Global.VspViewer.SelectedTileF = index;
            if (lt == LayerType.Obs)
                Global.ObsVspViewer.SelectedTileF = index;
            if (lt == LayerType.Zone)
                Global.SelectedZone = (MapZone)Global.ActiveMap.Zones[index];
        }
        public static void setSelectedTileB(LayerType lt, int index) {
            if (lt == LayerType.Tile)
                Global.VspViewer.SelectedTileB = index;
            if (lt == LayerType.Obs)
                Global.ObsVspViewer.SelectedTileB = index;
        }

        public static event SimpleEventHandler zoomChanged;

        public static int editedLayerIndex { get { return Global.ActiveMap.Layers.IndexOf(editedLayer); } }
        public static MapLayer editedLayer { get { return Global.lpSelection.mLayerRef; } }
        public static event SimpleEventHandler SelectedEntityChanged;
        public static event SimpleEventHandler SelectedZoneChanged;
        public static MapEntity _SelectedEntity;
        public static MapEntity SelectedEntity {
            set {
                _SelectedEntity = value;
                if (SelectedEntityChanged != null)
                    SelectedEntityChanged();
            }
            get {
                return _SelectedEntity;
            }
        }
        public static MapZone _SelectedZone;
        public static MapZone SelectedZone {
            set {
                _SelectedZone = value;
                if (SelectedZoneChanged != null)
                    SelectedZoneChanged();
            }
            get {
                return _SelectedZone;
            }
        }
        //public static MapLayer getLayer(int index) { return (MapLayer)Global.ActiveMap.Layers[index]; }

        public static MainWindow mainWindow;



        public static bool IsClosing;

        public static bool
                IsMapFocused,
                IsVspFocused,
                IsBrushFocused;

        public static LPanel lpSelection;

        public static void RedrawMinimap() {
            MiniMap.bNeedsRedraw = true;
            MiniMap.Invalidate();
        }

        public static void ForceRedraws() {
            Global.VspViewer.ForceRedraw();
            Global.layerTool.Invalidate();
            MiniMap.bNeedsRedraw = true;
            MiniMap.Invalidate();
            MainMapController.Invalidate(true);
            if (mainWindow != null) mainWindow.ui_update(false);
        }
        static Point _MapCursorLocation;
        public static Point MapCursorLocation {
            get {
                return _MapCursorLocation;
            }
            set {
                _MapCursorLocation = value;
            }
        }
        public static void InvokeCursorMove(bool overmap, int x, int y) {
            _MapCursorLocation = new Point(x, y);
            if (CursorLocationChanged != null)
                CursorLocationChanged(new xyEventArgs(overmap, x, y));
        }
        public class xyEventArgs { public xyEventArgs(bool overmap, int x, int y) { this.overmap = overmap; this.x = x; this.y = y; } public bool overmap; public int x; public int y; }
        public delegate void xyEventHandler(xyEventArgs e);
        public static event xyEventHandler CursorLocationChanged;
        public class LEventArgs { public LEventArgs(int lindex) { LayerIndex = lindex; } public int LayerIndex; }
        public delegate void LEventHandler(LEventArgs e);
        public static event LEventHandler WriteDestChanged;
        public static void FireWriteEvent(LEventArgs e) {
            if (WriteDestChanged != null) WriteDestChanged(e);
        }

        private static void MapScrollBar_ValueChanged(object sender, EventArgs e) {
            OnScroll();
        }


        public static bool bInvokeRGBSliders = true;
        public static bool bInvokeRGBText = true;
        public static bool bInvokeMinimapPaint = false;

    }
    public class Helper {
        public static byte[] StringToPaddedByteArray(string s, int maxlength) {
            byte[] ba = new byte[maxlength];
            for (int i = 0; i < maxlength; i++) {
                if (i < s.Length) ba[i] = (byte)s[i];
                else ba[i] = 0;
            }
            return ba;
        }
        public static string StringFromBytePool(byte[] ba, int offset) {
            StringBuilder sb = new StringBuilder();
            for (int i = offset; i < ba.Length; i++) {
                if (ba[i] != 0)
                    sb.Append((char)ba[i]);
                else break;
            }
            return sb.ToString();

        }
        public static byte[] StringToByteArray(string s) {
            byte[] ba = new byte[s.Length + 1];
            for (int i = 0; i < ba.Length; i++) {
                if (i < s.Length) ba[i] = (byte)s[i];
                else ba[i] = 0;
            }
            return ba;
        }
        public static string CharsToString(char[] ca) {
            StringBuilder sb = new StringBuilder();
            foreach (char c in ca) {
                if (c != (char)0)
                    sb.Append(c);
                else break;
            }
            return sb.ToString();
        }
        public static string BytesToString(byte[] ba) {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in ba) {
                if (b != 0)
                    sb.Append((char)b);
                else break;
            }
            return sb.ToString();
        }
        public static ushort[] BytesToWords(byte[] ba) {
            if ((ba.Length & 0x01) == 0x01) { Errors.Error("INTERNAL ERROR", "Unaligned byte array can not be converted!"); return null; }
            ushort[] words = new ushort[ba.Length / 2];
            for (int i = 0; i < ba.Length; i += 2) {
                words[i / 2] = (ushort)((ba[i + 1] << 8) | ba[i]);
            }
            return words;
        }
        public static byte[] WordsToBytes(ushort[] wa) {
            byte[] bytes = new byte[wa.Length * 2];
            for (int i = 0; i < wa.Length; i++) {
                bytes[i * 2] = (byte)((wa[i] & 0x00FF) >> 8);
                bytes[i * 2 + 1] = (byte)(wa[i] & 0xFF00);
            }
            return bytes;
        }
        public static string GetRelativePath(string root, string fname) {
            if (root.Length > fname.Length) return null;
            if (root != fname.Substring(0, root.Length))
                return null;
            string rel = fname.Substring(root.Length);
            if (rel[0] == '\\') rel = rel.Substring(1);
            return rel;
        }
    }

    public class Images {
        public static Bitmap BmpRender = new Bitmap(typeof(Global), "resources.bitmaps.render.bmp");
        public static Bitmap BmpWrite = new Bitmap(typeof(Global), "resources.bitmaps.write.bmp");
        public static void ImagesInit() {
            Images.BmpRender.MakeTransparent();
            Images.BmpWrite.MakeTransparent();
        }
    }


    class Ops {
        public class AddRemoveTilesGroup : Operations.OperationBase, Operations.IOperation {
            class Rec {
                public int t;
                public int op;
                public Rec(int t, int op) { this.t = t; this.op = op; }
                public Vsp24Tile oldTile;
            }

            int which;
            ArrayList recs = new ArrayList();

            public AddRemoveTilesGroup(int which) { this.which = which; }
            public void addRecord(int t, int op) {
                recs.Add(new Rec(t, op));
            }

            public void exec() {
                foreach (Rec r in recs) {
                    if (r.op == 0)
                        Global.ActiveVsp.insertAt(r.t);
                    else {
                        r.oldTile = ((Vsp24Tile)Global.ActiveVsp.Tiles[r.t]).Clone();
                        Global.ActiveVsp.removeAt(r.t);
                    }
                }
            }

            public void undo() {
                for (int i = 0; i < recs.Count; i++) {
                    Rec r = (Rec)recs[recs.Count - i - 1];
                    if (r.op == 0)
                        Global.ActiveVsp.removeAt(r.t);
                    else {
                        Global.ActiveVsp.insertAt(r.t);
                        Global.ActiveVsp.Tiles[r.t] = r.oldTile.Clone();
                    }
                }

            }
        }

        public class SetTiledataGroup : Operations.OperationBase, Operations.IOperation {
            class Rec {
                public int[] newData;
                public int[] oldData;
                public int index;
                public Rec(int index, int[] newData) { this.index = index; this.newData = newData; }
            }

            int which;

            ArrayList recs = new ArrayList();

            public SetTiledataGroup(int which) { this.which = which; }

            public void exec() {
                foreach (Rec r in recs) {
                    if (Global.ActiveVsp.Tiles.Count > r.index && r.index >= 0) {
                        r.oldData = (int[])((Vsp24Tile)Global.ActiveVsp.Tiles[r.index]).Image.Pixels.Clone();
                        ((Vsp24Tile)Global.ActiveVsp.Tiles[r.index]).Pixels = (int[])r.newData.Clone();
                    }
                }
                Global.ActiveVsp.touch();
            }

            public void undo() {
                for (int i = 0; i < recs.Count; i++) {
                    Rec r = (Rec)recs[recs.Count - i - 1];

                    ((Vsp24Tile)Global.ActiveVsp.Tiles[r.index]).Pixels = (int[])r.oldData.Clone();
                }
                Global.ActiveVsp.touch();
            }

            public void addRecord(int index, int[] newData) {
                recs.Add(new Rec(index, (int[])newData.Clone()));
            }
        }

        public class SetTileGroup : Operations.OperationBase, Operations.IOperation {
            class Rec {
                public int l, x, y, v,old;
                public Rec(int l, int x, int y, int v) { this.l = l; this.x = x; this.y = y; this.v = v; }
            }

            int which;
            Map whichMap { get { return Global.ActiveMap; } }
            ArrayList recs = new ArrayList();

            public SetTileGroup(int which) {
                this.which = which;
            }

            public void exec() {
                foreach (Rec r in recs) {
                    MapLayer ml = (MapLayer)whichMap.Layers[r.l];
                    r.old = ml.getTile(r.x, r.y);
                    ml.setTile(r.x, r.y, r.v);
                }
                if (whichMap == Global.ActiveMap)
                    whichMap.touch();
            }

            public void undo() {
                for (int i = 0; i < recs.Count; i++) {
                    Rec r = (Rec)recs[recs.Count - i - 1];
                    MapLayer ml = (MapLayer)whichMap.Layers[r.l];
                    ml.setTile(r.x, r.y, r.old);
                }

                if (whichMap == Global.ActiveMap)
                    whichMap.touch();
            }

            public void addRecord(int l, int x, int y, int v) {
                recs.Add(new Rec(l, x, y, v));
            }

        }
        public class SetTile : Operations.OperationBase, Operations.IOperation {
            int old;
            int l, x, y,v;
            int which;

            Map whichMap { get { return Global.ActiveMap; } }


            public SetTile(int which, int l, int x, int y, int v) {
                this.which = which; this.l = l; this.x = x; this.y = y; this.v = v;
            }
            public void exec() {
                MapLayer ml = (MapLayer)whichMap.Layers[l];
                old = ml.getTile(x, y);
                ml.setTile(x, y, v);
                if (whichMap == Global.ActiveMap)
                    whichMap.touch();
            }
            public void undo() {
                MapLayer ml = (MapLayer)whichMap.Layers[l];
                ml.setTile(x, y, old);
                Global.mapViewer.Invalidate();
                //?
                //				if(whichMap == Global.ActiveMap)
                //					whichMap.touch();

            }
        }
    }



}
