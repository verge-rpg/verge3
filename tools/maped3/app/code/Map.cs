using System;
using System.Collections;
using System.IO;

namespace winmaped2
{


	public class ParallaxInfo
	{
	
		public double MultipleX = 1.0;
		public double MultipleY = 1.0;
	}

	/*public interface IMapLayer
	{
		public int Width { get; }
		public int Height { get; }
		public LayerType type { get; set; }
		public void size(int w, int h);

		public void setTile(int x, int y, int val);
		public int getTile(int x, int y);
	}*/


	public enum LayerType
	{
		Null, Tile, Zone, Obs, Entity, Special_Retrace
	}

	public class MapLayer
	{
		public int _Width;
		public int _Height;
		public short[] Data;
		public LayerType _type;

		public ParallaxInfo parallaxInfo = new ParallaxInfo();
		public int Translucency;
		public int HLine;
		public string _name = "";
		public virtual string name
		{
			get { return _name; }
			set { _name = value; }
		}

		public int Width { get { return _Width; } }
		public int Height { get { return _Height; } }
		public LayerType type { get { return _type; } set { _type = value; } }

		public int ID=0;

		public Map parentmap;
		public string RenderStringComponent
		{
			get
			{
				if(type==LayerType.Tile)
					return (ID+1).ToString();
				else if(type==LayerType.Entity)
					return "E";
				else if(type==LayerType.Special_Retrace)
					return "R";
				return "";
			}
		}


		public MapLayer(Map parent)
		{
			parentmap=parent;
		}
		public void size(int w, int h)
		{
			Data = new short[w*h];
			_Width = w;
			_Height = h;
		}
		public void resize(int w, int h)
		{
			int ow = _Width, oh = _Height;
			short[] ndata = new short[w*h];
			for(int y=0;y<h&&y<oh;y++)
				for(int x=0;x<w&&x<ow;x++)
					ndata[y*w+x] = Data[y*ow+x];
			_Width = w;
			_Height = h;
			Data = ndata;
		}

		public void setTile(int x, int y, int val)
		{
			if(x>=0&&x<_Width&&y>=0&&y<_Height) 
			{
				Data[_Width*y+x] = (short)val;
			}
		}

		public int getTile(int x, int y)
		{
			if(x>=0&&x<_Width&&y>=0&&y<_Height)
				return Data[_Width*y+x];
			else return -1;
		}

		public MapLayer copy()
		{
			MapLayer ml = new MapLayer(parentmap);
			ml.type = type;
			ml._Width = Width;
			ml._Height = Height;
			ml.parallaxInfo = parallaxInfo;
			ml.HLine = HLine;
			ml.name = name;
			ml.Translucency = Translucency;

			if(Data != null)
				ml.Data = (short[])Data.Clone();
			return ml;
		}

		public MapLayer copyRange(int x0, int y0, int w, int h)
		{
			MapLayer ml = new MapLayer(parentmap);
			ml.type = type;
			ml.parallaxInfo = parallaxInfo;
			ml.HLine = HLine;
			ml.name = name;
			ml.Translucency = Translucency;
			ml._Width = 0;
			ml._Height = 0;

			if(Data != null)
			{
				ml._Width = w;
				ml._Height = h;
				ml.Data = new short[w*h];
				for(int y=0;y<h;y++)
					for(int x=0;x<w;x++)
						if(x+x0 < Width && y+y0 < Height && x+x0 >= 0 && y+y0 >= 0)
							ml.Data[w*y+x] = Data[Width*(y+y0)+x+x0];
						else
							ml.Data[w*y+x] = 0;
			}

			return ml;			
				
		}
	}
	public class MapLayerSpecial : MapLayer
	{
		public override string name
		{
			get
			{
				switch(type)
				{
					case LayerType.Special_Retrace: return "Special: Retrace";
					default: return base.name;
				}
			}
			set
			{
				base.name = value;
			}
		}


		public MapLayerSpecial(Map p) : base(p) {}
	}

	public class MapZone
	{
		public int ID;
        public string Name="";

        public int Rate;
		public int Delay;
        public int AdjAct;
		
		public string PlayerScript="";
		public string EntityScript="";



		public override string ToString()
		{
			return "Zone "+ID.ToString() +": "+ Name;
		}
		public MapZone Clone()
		{
			MapZone mz = new MapZone();
			mz.ID = ID;
			mz.Name = Name;
			mz.Rate = Rate;
			mz.Delay = Delay;
			mz.AdjAct = AdjAct;
			mz.PlayerScript = PlayerScript;
			mz.EntityScript = EntityScript;
			return mz;
		}
	}

	public class MapChr
	{
		

		public bool bImageAvailable=false;
		// -- graphical stuff. Just loaded versions of the CHR files.  
		// May be null if the editor cant locate the file -- //

		public int FrameWidth, FrameHeight;
		public int HotspotX, HotspotY;
		public int HotspotWidth, HotspotHeight;
		public int FrameCount;
		public byte[] ImageData;
		public int IdleFrameDown, IdleFrameLeft, IdleFrameUp, IdleFrameRight;
		public string[] MoveScripts = new string[] { "","","","" };


		// ----- //
		
		
		public int ID;
		// -- Everything below this line belongs in the Map file -- //
		public string Name;
		public override string ToString()
		{
			return "Char " + ID + ": " + Name;
		}
		public MapChr Clone()
		{
			MapChr mc = new MapChr();
			if(bImageAvailable)
			{
				// copy actual chr data
				mc.bImageAvailable = this.bImageAvailable;
				mc.FrameWidth = this.FrameWidth;
				mc.FrameHeight = this.FrameHeight;
				mc.HotspotX = this.HotspotX;
				mc.HotspotY = this.HotspotY;
				mc.HotspotWidth = this.HotspotWidth;
				mc.HotspotHeight = this.HotspotHeight;
				mc.FrameCount = this.FrameCount;
				mc.ImageData = (byte[])this.ImageData.Clone();
				mc.IdleFrameDown = this.IdleFrameDown;
				mc.IdleFrameLeft = this.IdleFrameLeft;
				mc.IdleFrameUp = this.IdleFrameUp;
				mc.IdleFrameRight = this.IdleFrameRight;
				for(int i=0;i<4;i++)
					mc.MoveScripts[i] = string.Copy(this.MoveScripts[i]);
			}
			// copy map relevant info
			//mc.ID = this.ID;
			//mc.Name = this.Name;
			return mc;
		}
	}
	public class Rect
	{
		public int x0=0,y0=0,x1=0,y1=0;
	}

	public class MapEntity
	{
		public int TileX;
		public int TileY;

		public int Facing = 2;
		public int Moving;
		public int BottomLineFrame; // ??
		public int SpecialFrameSet;
		public int MapChrIndex; // v2
		public int Reset; // ??
		public int ObeyObstruction = 1;
		public int IsObstruction = 1;
		public int Speed = 100;
		public int AutoFace = 1;
		public int ActivationMode;

		public string onActivate = "";

		public int WanderSteps,WanderDelay=0;

		public int[] UserData = new int[6];

		// WANDER DATA
		public int MoveType;
		public Rect	WanderRectangle = new Rect();
		public string MoveScript="";
		//

		public string Description="";

		public int __movescript; //v2 compatibility

		public int ID;

		public MapChr Chr; // only exists if we've loaded image data
		public string ChrName="";
		public override string ToString()
		{
			return "Entity "+ID+": "+Description;
		}
		public MapEntity Clone()
		{
			MapEntity m = new MapEntity();
			m.TileX = TileX;
			m.TileY = TileY;
			m.Facing = Facing;
			m.Moving = Moving;
			m.BottomLineFrame = BottomLineFrame;
			m.SpecialFrameSet = this.SpecialFrameSet;
			m.MapChrIndex = this.MapChrIndex;
			m.Reset = this.Reset;
			m.ObeyObstruction = this.ObeyObstruction;
			m.IsObstruction = this.IsObstruction;
			m.Speed = this.Speed;
			m.AutoFace = this.AutoFace;
			m.ActivationMode = this.ActivationMode;
			m.MoveType = this.MoveType;
			m.WanderDelay = this.WanderDelay;
			m.WanderSteps = this.WanderSteps;
			m.Description = this.Description;
			m.MoveScript = this.MoveScript;
			m.__movescript = this.__movescript;
			m.ID = this.ID;
			m.Chr = this.Chr;
			m.ChrName = this.ChrName;
			m.WanderRectangle.x0 = this.WanderRectangle.x0;
			m.WanderRectangle.y0 = this.WanderRectangle.y0;
			m.WanderRectangle.x1 = this.WanderRectangle.x1;
			m.WanderRectangle.y1 = this.WanderRectangle.y1;

			m.onActivate = this.onActivate;

			return m;    
		}

	}
	public class MapMovescript
	{
		public string Script;
	}

	public class MapRenderManager
	{
		public ArrayList Layers = new ArrayList();
		private Map map;
		public void update(Map m)
		{
			map=m;

		}
		public void MoveUp(MapLayer ml)
		{
			if(ml.type==LayerType.Obs||ml.type==LayerType.Zone)
				return;
			int idx = Layers.IndexOf(ml);
			if(idx==0) 
				return;
			Layers[idx] = Layers[idx-1];
			Layers[idx-1] = ml;
		}
		public void MoveDown(MapLayer ml)
		{
			if(ml.type==LayerType.Obs||ml.type==LayerType.Zone)
				return;
			int idx = Layers.IndexOf(ml);
			MapLayer mlb = (MapLayer)Layers[idx+1];
			if(mlb.type==LayerType.Obs||mlb.type==LayerType.Zone)
				return;
			Layers[idx] = mlb;
			Layers[idx+1] = ml;
		}
		public void AddLayer(MapLayer ml)
		{
			if(ml.type!=LayerType.Tile) return;
			for(int i=Layers.Count-1;i>0;i--)
			{
				MapLayer mld = (MapLayer)Layers[i-1];
				if(mld.type==LayerType.Tile||i==1)
				{
					Layers.Insert(i, ml);
					return;
				}
			}
			//Layers.Insert(0,ml);
		}
		public string ToRenderString()
		{
			// generate layer IDs
			int c=0;
			string rs="";
			foreach(MapLayer ml in map.Layers)
				if(ml.type==LayerType.Tile)
					ml.ID = c++;
			foreach(MapLayer ml in Layers)
			{
				if(ml.type!=LayerType.Obs&&ml.type!=LayerType.Zone)
				{
					if(rs.Length!=0)
						rs += ",";
					rs += ml.RenderStringComponent;
				}
			}
			return rs;
		}
		public void SetRenderString(Map m, string v)
		{ 
			map=m;
			string[] comms = v.Split(',');
			ArrayList layers = new ArrayList();
			foreach(MapLayer ml in m.Layers)
			{
				if(ml.type==LayerType.Tile)
					layers.Add(ml);
			}
			Layers.Clear();
			foreach(string s in comms)
			{
				if(s.Length==0) continue;
				if(char.IsDigit(s[0])) // layer
				{
					Layers.Add(layers[int.Parse(s)-1]);
				}
				else if (s.ToLower()=="e")
				{
					Layers.Add(m.EntLayer);
				}
				else if (s.ToLower()=="r")
				{
					// locate R layer
					foreach(MapLayer ml in m.Layers)
					{
						if(ml.type==LayerType.Special_Retrace)
						{
							Layers.Add(ml);
							break;
						}
					}
				}
			}
			foreach(MapLayer ml in m.Layers)
			{
				if(ml is MapLayerSpecial||ml.type==LayerType.Tile||ml.type==LayerType.Entity)
					if(!Layers.Contains(ml))
						Layers.Add(ml);
			}
			Layers.Add(m.ObsLayer);
			Layers.Add(m.ZoneLayer);

		}
	}
	public class MapUIRenderState
	{
		public class LayerState
		{
			public enum DrawMode { Lucent, None, Hline };
			public DrawMode drawMode;
			public bool bRender;
			public MapLayer mlayer;
		}
		public ArrayList Layers = new ArrayList();
		public void Setup(Map map)
		{
			if(true)//map.RenderString.Length==0)
			{
				foreach(MapLayer ml in map.Layers)
				{
					LayerState ls = new LayerState();

					ls.bRender = true;
					ls.mlayer = ml;
					Layers.Add(ls);
				}
			}
		}
		public void AddLayer(MapLayer ml)
		{
			LayerState ls = new LayerState();

			if ( ml.HLine != 0 ) ls.drawMode = LayerState.DrawMode.Hline;
			else ls.drawMode = LayerState.DrawMode.None;
			ls.bRender = true;
			Layers.Add(ls);
		}
		public void RemoveLayer(MapLayer ml)
		{
			LayerState lsr=null;
			foreach(LayerState ls in Layers)
			{
				if(ls.mlayer==ml)
                    lsr=ls;					
			}
			if(lsr!=null)
				Layers.Remove(lsr);
		}
		public LayerState this[int index]
		{
			get
			{
				return ((LayerState)Layers[index]);
			}
		}
	}
	public class Map
	{
		public Vsp24 vsp;
		public string RenderString
		{
			get
			{
				return RenderManager.ToRenderString();
			}
			set
			{
				RenderManager.SetRenderString(this,value);
			}
		}
		public string MusicFileName = "";
		public string FormalName = "";
		public string AutoExecEvent = "";
		public int PlayerStartX;
		public int PlayerStartY;

		public ArrayList Layers = new ArrayList();

		public MapLayer ObsLayer;
		public MapLayer ZoneLayer;
		public MapLayer EntLayer;

		public ArrayList Zones = new ArrayList();
        public ArrayList Chars = new ArrayList();
		public ArrayList MoveScripts = new ArrayList();
		public ArrayList Entities = new ArrayList();

		public bool IsBrush = false;

		public MapRenderManager RenderManager = new MapRenderManager();


		public MapLayer getLayer(int index)
		{
			return (MapLayer)Layers[index];
		}

		public MapLayer findLayer(string name)
		{
			foreach(MapLayer ml in Layers)
				if(ml.name == name)
					return ml;
			return null;
		}

		public int findLayerIndex(string name)
		{
			for(int i=0;i<Layers.Count;i++)
				if(((MapLayer)Layers[i]).name == name)
					return i;
			return -1;
		}

		public int findLayerIndex(MapLayer ml)
		{
			for(int i=0;i<Layers.Count;i++)
				if(Layers[i] == ml)
					return i;
			return -1;
		}


		public Map tileCopy()
		{
			Map m = new Map();
			foreach(MapLayer ml in Layers)
			{
				MapLayer ml2 = ml.copy();
				ml2.parentmap = m;
				m.AddLayer(ml2);
			}
			return m;
		}

		public Map tileCopyRange(int x0, int y0, int w, int h)
		{
			Map m = new Map();
			foreach(MapLayer ml in Layers)
			{
				MapLayer ml2 = ml.copyRange(x0,y0,w,h);
				ml2.parentmap = m;
				m.AddLayer(ml2);
			}
			return m;
		}


		public MapUIRenderState UIState = new MapUIRenderState();

		public FileInfo FileOnDisk;

		bool _bUpdating = false;
		public bool bUpdating
		{
			set
			{
				_bUpdating = value;
				bAltered = _bAltered;
			}
			get { return _bUpdating; }
		}

		bool _bAltered;
		public bool bAltered
		{
			set
			{
				_bAltered=value;
				if(_bAltered && !_bUpdating)
				{
					Global.RedrawMinimap();
					Global.mainWindow.ui_update();
				}
			}
			get
			{
				return _bAltered;
			}
		}


		public void Init()
		{
			UIState.Setup(this);
			RenderManager.update(this);
		}
		public void touch()
		{
			bAltered=true;
		}
		public Map()
		{
		}
		public static Map LoadMap(string filename)
		{
			Map map = InputOutput.ReadMap(filename);
			if(map!=null)
				map.Init();	
			return map;
		}
		public int AddLayer(MapLayer ml)
		{
			if(this == Global.ActiveMap)
				UIState.AddLayer(ml);

            RenderManager.AddLayer(ml);
			return Layers.Add(ml);
		}
		public void DeleteLayer(MapLayer ml)
		{
			if(ml.type!=LayerType.Tile) return;
			UIState.RemoveLayer(ml);
			RenderManager.Layers.Remove(ml);
			Layers.Remove(ml);
		}
		public int WriteMap()
		{
            int r = InputOutput.WriteMap(FileOnDisk,this);
			if(r==0) bAltered=false;
			return r;
		}
		public int WriteMap(string fname)
		{
			FileInfo fi = new FileInfo(fname);
            FileOnDisk=fi;
			return WriteMap();
		}
		public void ModifyTile(MapLayer ml, int x, int y, int newtile)
		{
			ml.setTile(x,y,newtile);
		}
	}
}
