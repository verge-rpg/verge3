using System;
using System.IO;
using System.Text;
using System.Collections;



namespace winmaped2 {
    public class InputOutput {
        public unsafe static Map ReadMap3(FileInfo fi) {
            byte[] m3s = new byte[] { (byte)'V', (byte)'3', (byte)'M', (byte)'A', (byte)'P', (byte)0 };

            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);

            Map map = new Map();

            map.FileOnDisk = fi;

            //			Directory.SetCurrentDirectory(map.FileOnDisk.Directory.FullName);


            byte[] sig = br.ReadBytes(m3s.Length);
            for (int i = 0; i < m3s.Length; i++)
                if (sig[i] != m3s[i]) return null;

            int version = br.ReadInt32();
            int vcofs = br.ReadInt32();

            byte[] formal = br.ReadBytes(256);
            byte[] vspname = br.ReadBytes(256);
            byte[] musicname = br.ReadBytes(256);
            byte[] renderstring = br.ReadBytes(256);
            byte[] aexec = br.ReadBytes(256);

            string vspf = Helper.BytesToString(vspname);
            FileInfo vspfile;
            try {

                vspfile = new FileInfo(vspf);
            } catch (ArgumentException) {
                throw new Exception("VSP file is inaccessible. Requested file was '" + vspf + "'");
            }
            if (!vspfile.Exists) {
                System.Windows.Forms.DialogResult dr = System.Windows.Forms.MessageBox.Show("Unable to load requested VSP file.  Create a blank vsp?", "Load Error", System.Windows.Forms.MessageBoxButtons.YesNoCancel, System.Windows.Forms.MessageBoxIcon.Information);
                if (dr == System.Windows.Forms.DialogResult.OK)
                    map.vsp = new Vsp24();
                else if (dr == System.Windows.Forms.DialogResult.Cancel)
                    return null;
                else return null;
            } else {
                map.vsp = ReadVsp(vspfile.FullName);
            }

            map.FormalName = Helper.BytesToString(formal);
            map.MusicFileName = Helper.BytesToString(musicname);
            //map.RenderString = Helper.BytesToString(renderstring);
            string rs = Helper.BytesToString(renderstring);
            map.AutoExecEvent = Helper.BytesToString(aexec);

            map.PlayerStartX = br.ReadInt16();
            map.PlayerStartY = br.ReadInt16();

            int layercount = br.ReadInt32();

            for (int i = 0; i < layercount; i++) {
                MapLayer ml = new MapLayer(map);
                ml.name = Helper.BytesToString(br.ReadBytes(256));
                ml.type = LayerType.Tile;
                ml.parallaxInfo = new ParallaxInfo();
                ml.parallaxInfo.MultipleX = br.ReadDouble();
                ml.parallaxInfo.MultipleY = br.ReadDouble();
                int w = br.ReadInt16();
                int h = br.ReadInt16();
                ml.size(w, h);
                ml.Translucency = br.ReadByte();
                int len = br.ReadInt32();
                int zlen = br.ReadInt32();
                ml.Data = ZLIB.DecodeShorts(br.ReadBytes(zlen), len);

                map.Layers.Add(ml);
            }




            MapLayer ol = new MapLayer(map);
            ol.type = LayerType.Obs;
            ol.name = "Obstructions";
            ol.size(((MapLayer)map.Layers[0]).Width, ((MapLayer)map.Layers[0]).Height);
            int ol_lenn = br.ReadInt32();
            int ol_len = br.ReadInt32();
            byte[] obsdata = ZLIB.Decode(br.ReadBytes(ol_len), ol_lenn);
            for (int i = 0; i < obsdata.Length; i++)
                ol.Data[i] = obsdata[i];
            map.ObsLayer = ol;


            int zl_lenn = br.ReadInt32();
            int zl_len = br.ReadInt32();
            MapLayer zl = new MapLayer(map);
            zl.type = LayerType.Zone;
            zl.name = "Zones";
            zl.size(((MapLayer)map.Layers[0]).Width, ((MapLayer)map.Layers[0]).Height);
            zl.Data = ZLIB.DecodeShorts(br.ReadBytes(zl_len), zl_lenn);
            map.ZoneLayer = zl;


            MapLayer el = new MapLayer(map);
            el.type = LayerType.Entity;
            el.name = "Entities";
            map.EntLayer = el;
            map.Layers.Add(el);

            MapLayerSpecial rl = new MapLayerSpecial(map);
            rl.type = LayerType.Special_Retrace;


            map.Layers.Add(rl);
            map.Layers.Add(ol);
            map.Layers.Add(zl);


            int zonecount = br.ReadInt32();
            for (int i = 0; i < zonecount; i++) {
                MapZone mz = new MapZone();
                byte[] zname = br.ReadBytes(256);
                byte[] pscript = br.ReadBytes(256);
                //				byte[] escript = br.ReadBytes(256);
                mz.Name = Helper.BytesToString(zname);
                mz.PlayerScript = Helper.BytesToString(pscript);
                //				mz.EntityScript = Helper.BytesToString(escript);
                mz.Rate = br.ReadByte();
                mz.Delay = br.ReadByte();
                mz.AdjAct = br.ReadByte();
                mz.ID = i;
                map.Zones.Add(mz);
            }
            if (zonecount == 0) {
                MapZone mz = new MapZone();
                mz.ID = 0;
                mz.Name = "NULL_ZONE";
                map.Zones.Add(mz);
            }

            int entcount = br.ReadInt32();
            for (int i = 0; i < entcount; i++) {
                MapEntity me = new MapEntity();
                me.TileX = br.ReadInt16();
                me.TileY = br.ReadInt16();
                me.Facing = br.ReadByte();

                me.ObeyObstruction = br.ReadByte();
                me.IsObstruction = br.ReadByte();
                me.AutoFace = br.ReadByte();
                me.Speed = br.ReadInt16();
                me.ActivationMode = br.ReadByte();

                me.MoveType = br.ReadByte();
                me.WanderRectangle.x0 = br.ReadInt16();
                me.WanderRectangle.y0 = br.ReadInt16();
                me.WanderRectangle.x1 = br.ReadInt16();
                me.WanderRectangle.y1 = br.ReadInt16();
                me.WanderDelay = br.ReadInt16();

                int expand = br.ReadInt32();

                me.MoveScript = Helper.BytesToString(br.ReadBytes(256));
                me.ChrName = Helper.BytesToString(br.ReadBytes(256));
                me.Description = Helper.BytesToString(br.ReadBytes(256));
                me.onActivate = Helper.BytesToString(br.ReadBytes(256));
                me.ID = map.Entities.Add(me);

            }
            br.Close();
            string rs2 = "";
            if (version == 1) {
                foreach (char c in rs) {
                    if (rs2.Length != 0)
                        rs2 += ",";
                    rs2 += c;
                }
                map.RenderString = rs2;
            } else
                map.RenderString = rs;
            return map;
        }

        public unsafe static Vsp24 ReadVsp(string filename) {
            FileInfo fi = new FileInfo(filename);
            if (!fi.Exists) return null;
            if (fi.Length < 8) return null;

            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);
            int sig = br.ReadInt32();
            if (sig == VSP_SIGNATURE) // vsp24
			{
                br.Close();
                return ReadVsp24(fi);
            } else {
                fs.Seek(0, SeekOrigin.Begin);
                ushort version = br.ReadUInt16();
                if (version < 5) {
                    br.Close();
                    return ReadVsp8(fi);
                }
            }
            return null;
        }
        const int VSP_SIGNATURE = 5264214;
        const int VSP_VERSION = 6;
        const int VSP_FORMAT = 1;
        const int VSP_COMPRESSION = 1;
        const int VSP_TILESIZE = 16;

        public unsafe static Vsp24 ReadVsp24(FileInfo fi) {
            //Errors.Error("loading vsp");
            Vsp24 vsp = new Vsp24();

            vsp.FileOnDisk = fi;

            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);

            int sig = br.ReadInt32();
            int version = br.ReadInt32();
            if (sig != VSP_SIGNATURE || version != VSP_VERSION)
                return null;


            // assume 16
            int tilesize = br.ReadInt32();

            // assume 24-bit
            int format = br.ReadInt32();

            int tilecount = br.ReadInt32();

            // assume zlib
            int compression = br.ReadInt32();

            int datalen = br.ReadInt32();
            int zdatalen = br.ReadInt32();

            byte[] zdata = br.ReadBytes(zdatalen);

            byte[] data = ZLIB.Decode(zdata, datalen);

            for (int tile = 0; tile < tilecount; tile++) {
                int[] px = new int[256];
                for (int i = 0; i < 256; i++) {
                    int idx = tile * 256 + i;
                    int c = unchecked((int)0xFF000000);
                    c = c | (data[idx * 3] << 16);
                    c = c | (data[idx * 3 + 1] << 8);
                    c = c | data[idx * 3 + 2];
                    px[i] = c;
                }
                vsp.Tiles.Add(new Vsp24Tile(vsp, new pr2.BufferImage(16, 16, px)));
            }

            int animcount = br.ReadInt32();
            for (int i = 0; i < animcount; i++) {
                VspAnimation va = new VspAnimation();
                va.Name = Helper.BytesToString(br.ReadBytes(256));
                va.Start = br.ReadInt32();
                va.End = br.ReadInt32();
                va.Delay = br.ReadInt32();
                va.Mode = br.ReadInt32();
                va.ID = i;

                vsp.Animations.Add(va);
            }
            int obscount = br.ReadInt32();
            int od_len = br.ReadInt32();
            int od_zlen = br.ReadInt32();
            byte[] zd = br.ReadBytes(od_zlen);
            byte[] od = ZLIB.Decode(zd, od_len);

            for (int i = 0; i < obscount; i++) {
                int[] tile = new int[256];
                for (int j = 0; j < 256; j++)
                    tile[j] = od[i * 256 + j];
                vsp.ObstructionTiles.Add(new VspObstructionTile(vsp, tile));
            }

            br.Close();

            return vsp;
        }
        public unsafe static int WriteVsp(FileInfo fi, Vsp24 vsp) {
            if (fi.Exists) fi.Delete();
            FileStream fs = fi.OpenWrite();
            BinaryWriter bw = new BinaryWriter(fs);

            vsp.FileOnDisk = fi;


            bw.Write(VSP_SIGNATURE);
            bw.Write(VSP_VERSION);


            bw.Write(VSP_TILESIZE);
            bw.Write(VSP_FORMAT);
            bw.Write(vsp.Tiles.Count);
            bw.Write(VSP_COMPRESSION);

            // build byte array of all tiles in our vsp
            MemoryStream ms_tiles = new MemoryStream();
            BinaryWriter bw_tiles = new BinaryWriter(ms_tiles);
            foreach (Vsp24Tile tile in vsp.Tiles) {
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 16; x++) {
                        int p = tile.Image.GetPixel(x, y);
                        bw_tiles.Write((byte)((p & 0x00FF0000) >> 16));
                        bw_tiles.Write((byte)((p & 0x0000FF00) >> 8));
                        bw_tiles.Write((byte)((p & 0x000000FF)));
                    }
                }
            }
            bw_tiles.Close();
            byte[] zdata = ZLIB.Encode(ms_tiles.ToArray());
            bw.Write((int)16 * 16 * 3 * vsp.Tiles.Count);

            bw.Write(zdata.Length);
            bw.Write(zdata);

            bw.Write(vsp.Animations.Count);

            foreach (VspAnimation va in vsp.Animations) {
                bw.Write(Helper.StringToPaddedByteArray(va.Name, 256));
                bw.Write(va.Start);
                bw.Write(va.End);
                bw.Write(va.Delay);
                bw.Write(va.Mode);
            }

            bw.Write(vsp.ObstructionTiles.Count);

            byte[] odata = new byte[vsp.ObstructionTiles.Count * 256];
            for (int i = 0; i < vsp.ObstructionTiles.Count; i++) {
                int[] pixels = ((VspObstructionTile)vsp.ObstructionTiles[i]).Image.GetArray();
                for (int j = 0; j < 256; j++) {
                    odata[i * 256 + j] = (byte)pixels[j];
                }
            }
            byte[] ozdata = ZLIB.Encode(odata);
            bw.Write((int)odata.Length);
            bw.Write((int)ozdata.Length);
            bw.Write(ozdata);

            Global.pluginManager.tilesetSave(fs);

            bw.Close();
            return 0;
        }
        public unsafe static int WriteMap(FileInfo fi, Map map) {
            byte[] map3_signature = new byte[] { (byte)'V', (byte)'3', (byte)'M', (byte)'A', (byte)'P', (byte)0 };
            if (map == null) return -1;

            string vspname = Helper.GetRelativePath(map.FileOnDisk.Directory.FullName, map.vsp.FileOnDisk.FullName);
            if (vspname == null)
                throw new Exception("Unable to resolve VSP path.");

            fi = new FileInfo(fi.Name);
            if (fi.Exists) fi.Delete();
            FileStream fs = fi.OpenWrite();
            MemoryStream ms_outbuf = new MemoryStream();
            BinaryWriter bw_o = new BinaryWriter(ms_outbuf);

            MemoryStream ms_outbuf2 = new MemoryStream();
            BinaryWriter bw = new BinaryWriter(ms_outbuf2);

            bw_o.Write(map3_signature);
            bw_o.Write(Global.VERSIONINFO.MAPVERSION);



            bw.Write(Helper.StringToPaddedByteArray(map.FormalName, 256));
            bw.Write(Helper.StringToPaddedByteArray(vspname, 256));
            bw.Write(Helper.StringToPaddedByteArray(map.MusicFileName, 256));
            bw.Write(Helper.StringToPaddedByteArray(map.RenderString, 256));
            bw.Write(Helper.StringToPaddedByteArray(map.AutoExecEvent, 256));


            bw.Write((short)map.PlayerStartX);
            bw.Write((short)map.PlayerStartY);


            int special_count = 0;
            foreach (MapLayer ml in map.Layers)
                if (ml.type != LayerType.Tile)
                    special_count++;
            bw.Write(map.Layers.Count - special_count);

            foreach (MapLayer ml in map.Layers) {
                if (ml.type != LayerType.Tile) continue;
                bw.Write(Helper.StringToPaddedByteArray(ml.name, 256));
                bw.Write(ml.parallaxInfo.MultipleX);
                bw.Write(ml.parallaxInfo.MultipleY);
                bw.Write((short)ml.Width);
                bw.Write((short)ml.Height);
                bw.Write((byte)ml.Translucency);

                fixed (short* ptr = ml.Data) {
                    byte[] zdata = ZLIB.Encode((byte*)ptr, ml.Data.Length * 2);
                    bw.Write(ml.Data.Length * 2);
                    bw.Write(zdata.Length);
                    bw.Write(zdata);
                }
            }

            MapLayer zl = map.ZoneLayer, ol = map.ObsLayer;

            byte[] obsdata = new byte[ol.Data.Length];
            for (int j = 0; j < ol.Data.Length; j++)
                obsdata[j] = (byte)ol.Data[j];


            fixed (byte* ptr = obsdata) {
                byte[] zdata = ZLIB.Encode(ptr, obsdata.Length);
                bw.Write(obsdata.Length);
                bw.Write(zdata.Length);
                bw.Write(zdata);
            }
            fixed (short* ptr = zl.Data) {
                byte[] zdata = ZLIB.Encode((byte*)ptr, zl.Data.Length * 2);
                bw.Write(zl.Data.Length * 2);
                bw.Write(zdata.Length);
                bw.Write(zdata);
            }

            bw.Write(map.Zones.Count);
            foreach (MapZone mz in map.Zones) {
                bw.Write(Helper.StringToPaddedByteArray(mz.Name, 256));
                bw.Write(Helper.StringToPaddedByteArray(mz.PlayerScript, 256));
                //				bw.Write(Helper.StringToPaddedByteArray(mz.EntityScript,256));
                bw.Write((byte)mz.Rate);
                bw.Write((byte)mz.Delay);
                bw.Write((byte)mz.AdjAct);
            }


            bw.Write(map.Entities.Count);
            foreach (MapEntity me in map.Entities) {
                bw.Write((short)me.TileX);
                bw.Write((short)me.TileY);
                bw.Write((byte)me.Facing);
                bw.Write((byte)me.ObeyObstruction);
                bw.Write((byte)me.IsObstruction);
                bw.Write((byte)me.AutoFace);
                bw.Write((short)me.Speed);
                bw.Write((byte)me.ActivationMode);
                bw.Write((byte)me.MoveType);
                bw.Write((short)me.WanderRectangle.x0);
                bw.Write((short)me.WanderRectangle.y0);
                bw.Write((short)me.WanderRectangle.x1);
                bw.Write((short)me.WanderRectangle.y1);
                bw.Write((short)me.WanderDelay);
                bw.Write((int)0);//expand
                bw.Write(Helper.StringToPaddedByteArray(me.MoveScript, 256));
                bw.Write(Helper.StringToPaddedByteArray(me.ChrName, 256));
                bw.Write(Helper.StringToPaddedByteArray(me.Description, 256));
                bw.Write(Helper.StringToPaddedByteArray(me.onActivate, 256));
            }


            bw_o.Write((int)14 + (int)ms_outbuf2.Length);
            bw.Close();
            bw_o.Close();
            bw = new BinaryWriter(fs);
            bw.Write(ms_outbuf.ToArray());
            bw.Write(ms_outbuf2.ToArray());

            //write number of compiled vc functions
            bw.Write((int)0);

            bw.Close();
            return 0;

        }
        public static Map ReadMap(string filename) {
            // open file and examine it to determine if we support this format
            byte[] m3s = new byte[] { (byte)'V', (byte)'3', (byte)'M', (byte)'A', (byte)'P', (byte)0 };
            byte[] m2s = new byte[] { (byte)77, (byte)65, (byte)80, (byte)249, (byte)53, (byte)0 };

            FileInfo fi = new FileInfo(filename);
            if (!fi.Exists) return null;
            if (fi.Length < 10) return null;

            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);


            byte[] sig = br.ReadBytes(6);
            int version = br.ReadInt32();

            int mtype = 0;

            br.Close();

            for (int i = 0; i < 6; i++) {
                if (sig[i] != m2s[i]) {
                    mtype = 0;
                    break;
                } else mtype = 1;
            }
            if (mtype == 0)
                for (int i = 0; i < 6; i++) {
                    if (sig[i] != m3s[i]) {
                        mtype = 0;
                        break;
                    } else mtype = 2;
                }
            //			map.FileOnDisk=fi;
            System.IO.Directory.SetCurrentDirectory(fi.Directory.FullName);
            Map m = null;
            try {
                switch (mtype) {
                    case 1:
                        m = ReadMap2(fi);
                        break;
                    case 2:
                        m = ReadMap3(fi);
                        break;
                    default: Errors.Error("I/O", "Unsupported file format."); return null;
                }
            } catch (System.IO.EndOfStreamException) {
                Errors.Error("I/O Error", "Unable to load map: Unexpected end of file");
                return null;
            }
            /*			catch(Exception e)
                                    {
                                            Errors.Error("I/O Error", "Unable to load map: Unknown read error. File is probably corrupt.\r\n" + e.Message + "\r\nMAP FILE: " + fi.FullName);
                                            return null;
                                    }			*/
            return m;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="fi"></param>
        /// <param name="map"></param>
        public static Map ReadMap2(FileInfo fi) {
            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);

            byte[] Buffer = br.ReadBytes((int)fi.Length);

            br.Close();

            Map map = new Map();

            map.FileOnDisk = fi;

            MemoryStream ms = new MemoryStream(Buffer);
            br = new BinaryReader(ms);

            if (Buffer[0] != 77 || Buffer[1] != 65 || Buffer[2] != 80 || Buffer[3] != 249 || Buffer[4] != 53 || Buffer[5] != 0) {
                Errors.Error("InputOutput", "File is not a VERGE2 Map");
                return null;
            }


            // 6 bytes signature
            // 4 bytes unknown??
            // ---
            // advance 10 bytes
            br.ReadBytes(10);


            // 60 bytes VSP FileName
            FileInfo vspfile = new FileInfo(Helper.BytesToString(br.ReadBytes(60)));
            if (!vspfile.Exists) {
                // TODO:  give the option to create a vsp in this case
                Errors.Error("InputOutput", "VSP was not found. New VSP created.");
                map.vsp = new Vsp24();
            } else {
                map.vsp = ReadVsp(vspfile.FullName);
            }

            // 60 bytes music filename
            map.MusicFileName = Helper.BytesToString(br.ReadBytes(60));

            // 20 bytes render string
            string dontcare = Helper.BytesToString(br.ReadBytes(20));


            // 4 bytes, start position
            map.PlayerStartX = (int)br.ReadInt16();
            map.PlayerStartY = (int)br.ReadInt16();

            // 51 bytes, unknown
            br.ReadBytes(51);

            // 1 byte, number of layers
            int layerCount = (int)br.ReadByte();


            for (int i = 0; i < layerCount; i++) {
                // 12 bytes per layer discriptor
                MapLayer ml = new MapLayer(map);
                ml.type = LayerType.Tile;
                ml.name = "Layer " + i.ToString();


                int mx, dx, my, dy;
                mx = (int)br.ReadByte();
                dx = (int)br.ReadByte();
                my = (int)br.ReadByte();
                dy = (int)br.ReadByte();

                double dmx, dmy;

                dmx = 1.0 * mx / dx;
                dmy = 1.0 * my / dy;
                ml.parallaxInfo.MultipleX = dmx;
                ml.parallaxInfo.MultipleY = dmy;
                /*
                                                ml.parallaxInfo.MultiplyX = (int)br.ReadByte();
                                                ml.parallaxInfo.DivideX = (int)br.ReadByte();
                                                ml.parallaxInfo.MultiplyY = (int)br.ReadByte();
                                                ml.parallaxInfo.DivideY = (int)br.ReadByte(); */

                int w = (int)br.ReadInt16();
                int h = (int)br.ReadInt16();
                ml.size(w, h);

                ml.Translucency = (int)br.ReadByte();
                ml.HLine = (int)br.ReadByte();

                // padding
                br.ReadInt16();

                map.Layers.Add(ml);
            }

            for (int i = 0; i < layerCount; i++) {
                int rleLength = br.ReadInt32();

                ushort[] layerdata = InputOutput.DecompressData16(Helper.BytesToWords(br.ReadBytes(rleLength)));
                MapLayer ml = (MapLayer)map.Layers[i];
                ml.Data = new short[ml.Width * ml.Height];
                for (int j = 0; j < ml.Width * ml.Height; j++)
                    ml.Data[j] = (short)layerdata[j];


            }

            int obsDataLength = br.ReadInt32();
            byte[] obsdata = InputOutput.DecompressData8(br.ReadBytes(obsDataLength));
            map.ObsLayer = new MapLayer(map);
            map.ObsLayer.type = LayerType.Obs;
            map.ObsLayer.size(((MapLayer)map.Layers[0]).Width, ((MapLayer)map.Layers[0]).Height);
            map.ObsLayer.name = "Obstructions";
            map.ObsLayer.Data = new short[map.ObsLayer.Width * map.ObsLayer.Height];
            for (int i = 0; i < map.ObsLayer.Width * map.ObsLayer.Height; i++)
                map.ObsLayer.Data[i] = obsdata[i];


            int zoneDataLength = br.ReadInt32();
            byte[] zonedata = InputOutput.DecompressData8(br.ReadBytes(zoneDataLength));
            map.ZoneLayer = new MapLayer(map);
            map.ZoneLayer.type = LayerType.Zone;
            map.ZoneLayer.size(((MapLayer)map.Layers[0]).Width, ((MapLayer)map.Layers[0]).Height);
            map.ZoneLayer.name = "Zones";
            map.ZoneLayer.Data = new short[map.ZoneLayer.Width * map.ZoneLayer.Height];
            for (int i = 0; i < map.ZoneLayer.Width * map.ZoneLayer.Height; i++)
                map.ZoneLayer.Data[i] = zonedata[i];


            MapLayer el = new MapLayer(map);
            el.type = LayerType.Entity;
            el.name = "Entities";
            map.Layers.Add(el);
            map.EntLayer = el;


            MapLayerSpecial rl = new MapLayerSpecial(map);
            rl.type = LayerType.Special_Retrace;


            map.Layers.Add(rl);
            map.Layers.Add(map.ObsLayer);
            map.Layers.Add(map.ZoneLayer);
            int zoneCount = br.ReadInt32();

            // 50 bytes per zone
            for (int i = 0; i < zoneCount; i++) {
                MapZone mz = new MapZone();

                mz.ID = i;

                mz.Name = Helper.CharsToString(br.ReadChars(40));
                //Errors.Error(mz.Name);

                mz.PlayerScript = "";
                br.ReadInt16();
                mz.Rate = (int)br.ReadInt16();
                mz.Delay = (int)br.ReadInt16();


                mz.AdjAct = (int)br.ReadInt16();
                mz.EntityScript = "";
                br.ReadInt16();

                map.Zones.Add(mz);
            }
            if (zoneCount == 0) {
                MapZone mz = new MapZone();
                mz.ID = 0;
                mz.Name = "NULL_ZONE";
                map.Zones.Add(mz);
            }


            int chrCount = (int)br.ReadByte();
            ArrayList al_chrs = new ArrayList();

            for (int i = 0; i < chrCount; i++) {
                MapChr mc = new MapChr();

                try
                {
                    mc.Name = Helper.BytesToFileString(br.ReadBytes(60));
                }
                catch (Exception e)
                {
                    mc.Name = "";

                    /// there's no error log in maped3 yet.  I'll discuss this with the boys.  For now, ANNOYING MESSAGE BOX!
                    /// -gru

                    Errors.Error( "I/O", "Bad filename for MapChr("+i+"): " + e.Message +"\nDefaulting to empty string for file name." );
                }

                mc.ID = i;

                if( mc.Name.Length > 0 ) 
                {
                    FileInfo mcfi = new FileInfo(mc.Name);
                    if (ReadCHR(mcfi, mc) == 0) mc.bImageAvailable = true;
                }
                al_chrs.Add(mc);
            }





            int entCount = (int)br.ReadByte();
            //Errors.Error(entCount.ToString() + ", "+br.BaseStream.Position.ToString());

            for (int i = 0; i < entCount; i++) 
            {
                MapEntity me = new MapEntity();

                me.TileX = (int)br.ReadInt32();
                me.TileY = (int)br.ReadInt32();

                int xx = (int)br.ReadInt16();
                int xy = (int)br.ReadInt16();

                //Errors.Error(""+ me.TileX+","+me.TileY+":"+xx+","+xy);
                // ignore
                //br.ReadBytes(4);

                me.Facing = (int)br.ReadByte();
                if (me.Facing == 0)
                    me.Facing = 2;
                me.Moving = (int)br.ReadByte();

                // ignore
                br.ReadByte();

                me.BottomLineFrame = (int)br.ReadByte();
                me.SpecialFrameSet = (int)br.ReadByte();
                int cidx = (int)br.ReadByte();
                // assign chr
                // dont do this
                //me.Chr = ((MapChr)al_chrs[cidx]).Clone();
                me.ChrName = string.Copy(((MapChr)al_chrs[cidx]).Name);

                me.Reset = (int)br.ReadByte();
                me.ObeyObstruction = (int)br.ReadByte();
                me.IsObstruction = (int)br.ReadByte();
                me.Speed = (int)br.ReadByte();
                switch (me.Speed) {
                    case 1: me.Speed = 25; break;
                    case 2: me.Speed = 33; break;
                    case 3: me.Speed = 50; break;
                    case 4: me.Speed = 100; break;
                    case 5: me.Speed = 200; break;
                    case 6: me.Speed = 300; break;
                    case 7: me.Speed = 400; break;
                }

                // ignore
                br.ReadBytes(10); // entspeedcnt(1), anim frame delay(1), anim script(4), move script(4)

                //32

                me.AutoFace = (int)br.ReadByte();
                me.ActivationMode = (int)br.ReadByte();
                me.MoveType = (int)br.ReadByte();
                if (me.MoveType > 1) me.MoveType--;
                me.__movescript = (int)br.ReadByte();

                // ignore
                br.ReadBytes(2); // subtile move ctr, mode flag

                me.WanderSteps = (int)br.ReadInt16();
                me.WanderDelay = (int)br.ReadInt16();

                // ignore
                br.ReadBytes(4); // step ctr, delay ctr

                me.ID = i;

                br.ReadBytes(2); // WTF IS THIS!!

                for (int j = 0; j < 6; j++) {
                    me.UserData[j] = br.ReadInt16();
                }

                me.WanderRectangle.x0 = me.UserData[1];
                me.WanderRectangle.y0 = me.UserData[2];
                me.WanderRectangle.x1 = me.UserData[4];
                me.WanderRectangle.y1 = me.UserData[5];

                // ignore
                br.ReadBytes(20); // a lot of shiznit

                me.Description = Helper.BytesToString(br.ReadBytes(20));

                //Errors.Error(me.Description);

                //br.ReadBytes(2); // WTF IS THIS!!

                me.ID = i;
                map.Entities.Add(me);
            }

            int movescriptCount = (int)br.ReadByte();
            int movescriptBuffersize = (int)br.ReadInt32();


            ArrayList al_scripts = new ArrayList();
            // ignore
            br.ReadBytes(4 * movescriptCount);

            string script = "";
            while (movescriptBuffersize > 0) {
                char c = br.ReadChar();

                if (c == (char)0) {
                    al_scripts.Add(string.Copy(script));
                    script = "";
                } else script += c;
                movescriptBuffersize--;
            }
            // add all the scripts to their respective entities and trash the movescript array

            foreach (MapEntity me in map.Entities) {
                me.MoveScript = (string)al_scripts[me.__movescript];
            }

            br.Close();

            string rs2 = "";
            foreach (char c in dontcare) {
                if (rs2.Length != 0)
                    rs2 += ",";
                rs2 += c;
            }
            map.RenderString = rs2;
            return map;
        }

        /// <summary>
        /// reads data from file into vsp data structure
        /// </summary>
        /// <param name="fi">FileInfo of the file to be opened</param>
        /// <param name="vsp">Vsp structure to load into</param>
        public static Vsp24 ReadVsp8(FileInfo fi) {
            if (!fi.Exists) return null;
            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);

            Vsp8 vsp = new Vsp8();

            vsp.FileOnDisk = fi;

            ushort version = br.ReadUInt16();
            byte[] pal = br.ReadBytes(768);
            ushort tilecount = br.ReadUInt16();

            for (int i = 0; i < 256; i++) {
                vsp.Palette[i] = new Vsp8PaletteEntry();
                vsp.Palette[i].r = pal[i * 3];
                vsp.Palette[i].g = pal[i * 3 + 1];
                vsp.Palette[i].b = pal[i * 3 + 2];
            }
            byte[] tiles;

            if (version == 2) {
                tiles = br.ReadBytes(tilecount * 256);
            } else {
                // tile data is rle compressed
                int compressedlength = br.ReadInt32();
                tiles = DecompressData8(br.ReadBytes(compressedlength));
            }
            byte[] animations = br.ReadBytes(800);
            br.Close();

            MemoryStream ms;

            ms = new MemoryStream(tiles);
            br = new BinaryReader(ms);
            for (int i = 0; i < tilecount; i++) {
                Vsp8Tile vt = new Vsp8Tile(vsp, br.ReadBytes(256));
                vt.doAvg();
                vsp.Tiles.Add(vt);
            }
            br.Close();

            ms = new MemoryStream(animations);

            br = new BinaryReader(ms);
            for (int i = 0; i < 100; i++) {
                VspAnimation va = new VspAnimation();

                va.Start = (int)br.ReadUInt16();
                va.End = (int)br.ReadUInt16();
                va.Delay = (int)br.ReadUInt16();
                va.Mode = (int)br.ReadUInt16();

                va.ID = i;

                vsp.Animations.Add(va);
            }
            br.Close();

            return Vsp24.FromVsp8(vsp);
        }


        /// <summary>
        /// loads the data from 'fi' into 'chr'.
        ///
        /// </summary>
        /// <param name="fi"></param>
        /// <param name="chr"></param>
        /// <returns>-1 on failure, 0 on success</returns>
        public static int ReadCHR(FileInfo fi, MapChr chr) {
            if (!fi.Exists) return -1;

            FileStream fs = fi.OpenRead();
            BinaryReader br = new BinaryReader(fs);

            // pump the file into a memory stream
            int length = (int)fs.Length;

            MemoryStream ms = new MemoryStream(br.ReadBytes(length));

            br.Close();
            br = new BinaryReader(ms);

            byte version = br.ReadByte();

            if (version != 2) {
                Errors.Error("ReadChr", "INPUTOUTPUT::READCHR(): INCORRECT FILE VERSION");
                return -1;
            }

            chr.FrameWidth = (int)br.ReadInt16();
            chr.FrameHeight = (int)br.ReadInt16();

            chr.HotspotX = (int)br.ReadInt16();
            chr.HotspotY = (int)br.ReadInt16();

            chr.HotspotWidth = (int)br.ReadInt16();
            chr.HotspotHeight = (int)br.ReadInt16();

            chr.FrameCount = (int)br.ReadInt16();

            int CompressedLength = br.ReadInt32();
            chr.ImageData = DecompressData8(br.ReadBytes(CompressedLength));

            chr.IdleFrameLeft = (int)br.ReadInt32();
            chr.IdleFrameRight = (int)br.ReadInt32();
            chr.IdleFrameUp = (int)br.ReadInt32();
            chr.IdleFrameDown = (int)br.ReadInt32();

            for (int i = 0; i < 4; i++) {
                int msLength = br.ReadInt32();
                chr.MoveScripts[i] = Helper.BytesToString(br.ReadBytes(msLength));
            }



            br.Close();
            return 0;
        }

        /// <summary>
        /// 8-bit RLE decompression 
        /// </summary>
        /// <param name="compressed"></param>
        /// <returns></returns>
        public static byte[] DecompressData8(byte[] compressed) {
            MemoryStream mstream = new MemoryStream();
            BinaryWriter bw = new BinaryWriter(mstream);

            int run = 0;
            for (int i = 0; i < compressed.Length; i++) {
                if (run == 0) {
                    if (compressed[i] < 255) {
                        bw.Write(compressed[i]);
                    } else if (compressed[i] == 255) {
                        run = compressed[++i];
                    }
                } else {
                    while (run > 0) {
                        bw.Write(compressed[i]);
                        run--;
                    }
                }
            }
            byte[] decompressed = mstream.ToArray();
            bw.Close();
            return decompressed;
        }
        /// <summary>
        /// 16-bit RLE decompression
        /// </summary>
        /// <param name="compressed"></param>
        /// <returns></returns>
        public static ushort[] DecompressData16(ushort[] compressed) {
            MemoryStream ms = new MemoryStream();
            BinaryWriter bw = new BinaryWriter(ms);

            int run = 0;
            for (int i = 0; i < compressed.Length; i++) {
                if (run == 0) {
                    if ((compressed[i] & 0xFF00) == 0xFF00)
                        run = (compressed[i] & 0x00FF);
                    else
                        bw.Write(compressed[i]);
                } else {
                    while (run > 0) {
                        bw.Write(compressed[i]);
                        run--;
                    }
                }
            }

            bw.Close();
            return Helper.BytesToWords(ms.ToArray());
        }
    }

}
