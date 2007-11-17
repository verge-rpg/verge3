using System;
using System.Windows.Forms;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;

namespace winmaped2
{

	

	namespace Plugins
	{

		public interface IPlugin
		{
			Guid guid { get; }
		}

		public interface ITileTool {}

		public class PluginManager
		{
			public ArrayList plugins = new ArrayList();
			public void addPlugin(IPlugin plugin) { plugins.Add(plugin); }

			public void tilesetSave(Stream s) { foreach(IPlugin p in plugins) if (p is ITilesetPersist) ((ITilesetPersist)p).tilesetPersistSave(s); }
			public void tilesetLoad(Stream s) { foreach(IPlugin p in plugins) if (p is ITilesetPersist) ((ITilesetPersist)p).tilesetPersistLoad(s); }
			public void tilesetNew() { foreach(IPlugin p in plugins) if (p is ITilesetPersist) ((ITilesetPersist)p).tilesetPersistNew(); }
		}
		

		public interface ITileClickTool : ITileTool
		{
			void MouseClick(TileEventInfo tei);
		}
		public interface ITileDragTool : ITileTool
		{
			void MouseDown(TileEventInfo tei);
			void MouseMove(TileEventInfo tei);
			void MouseUp(TileEventInfo tei);
		}


		public interface IMapPlugin : IPlugin
		{
			//string name { get; }
			IMapTool getTool(bool lb, bool rb, bool shift, bool ctrl, bool alt);
		}

		public interface IMapPluginCursor
		{
			Cursor GetCursor(bool shift, bool ctrl, bool alt);
		}

		public interface IMapTool
		{
			LayerType[] layerTypes { get; }
		}

		public interface IConfigPanel
		{
			void attach(Control c);
			void detach();
		}

		public interface ITilesetPersist
		{
			void tilesetPersistNew();
			void tilesetPersistSave(Stream s);
			void tilesetPersistLoad(Stream s);
		}

		public interface IMapClickTool : IMapTool
		{
			void MouseClick(MapEventInfo mei);
		}
		public interface IMapDragTool : IMapTool
		{
			void MouseDown(MapEventInfo mei);
			void MouseMove(MapEventInfo mei);
			void MouseUp(MapEventInfo mei);
		}
		public interface IMapDragTileTool : IMapDragTool
		{
			void MouseMoveTile(MapEventInfo mei);
		}

		public interface IMapPainter
		{
			void tweakLayer(MapEventInfo mei);
			void paintMap(MapEventInfo mei, pr2.IRenderImage img);
			void paintWindow(MapEventInfo mei, Graphics g);
		}

		public interface IMapTweaker
		{
			void tweakMap(MapEventInfo mei);
		}

		public struct MapCursorLocation
		{
			public int mx,my,tx,ty,px,py;
			public Point m,t,p;
		}

		public class BaseEventInfo
		{
			public delegate void signal();
		}


		public interface ITilePainter
		{
			void tweakLayer(TileEventInfo tei);
		}

		public struct TileCursorLocation
		{
			public int mx,my,px,py;
			public Point m,p;
		}

		public class TileEventInfo : BaseEventInfo
		{
		/*	public Tile tile;
			public int tileIndex;
			public TileCursorLocation start, current, previous, end;
			public bool lb,rb;
			public int button;
			public int[] colors = new int[2];

			public void setColor(int button, int color)
			{
				Global.setCurrColor(button,color);
			}


			public void tileChanged()
			{
				Global.tileChanged(tileIndex);
			}

			public signal invalidate;

			public int getPixel(int x, int y)
			{
				if(x<8&&x>=0&&y>=0&&y<8)
					return (int)tile.pixels[x,y];
				else return -1;
			}

			public void setPixel(int x, int y, int p, bool bUndo)
			{
				if(x>=0&&x<8&&y>=0&&y<8)
				{
					if(bUndo)
						undo.add(new Tile.UndoRec(tileIndex));

					if(getPixel(x,y) != p)
						tile.pixels[x,y] = (byte)p;
				}
			}*/
		}

		public class MapEventInfo : BaseEventInfo
		{
			public MapCursorLocation start, current, previous, end;
			public MapLayer editedLayer;
			public Map editedMap;
			public int editedLayerIndex;
			public Operations.OperationManager opManager;
			public bool lb,rb;
			//public bool recordSetTileOperations = true;
			public int xScroll { get { return Global.MainMapController.hScrollBar.Value; } }
			public int yScroll { get { return Global.MainMapController.vScrollBar.Value; } }

			public bool bDirty;

			public bool bTweak = false;

			public int zoom { get { return Global.zoom; } }

			public int ltile { get { return Global.getSelectedTileF(editedLayer.type); } set { Global.setSelectedTileF(editedLayer.type,value); } }
			public int rtile { get { return Global.getSelectedTileB(editedLayer.type); } set { Global.setSelectedTileB(editedLayer.type,value); } }
			public Vsp24Tile ltileRec { get { return Global.ActiveVsp.GetTile(ltile); } }
			public Vsp24Tile rtileRec { get { return Global.ActiveVsp.GetTile(rtile); } }
			public int tile { get { if(lb) return ltile; else return rtile; } }
			public Vsp24Tile tileRec { get { return Global.ActiveVsp.GetTile(tile); } }

			public void setTile(string layer, int x, int y, int val)
			{
				MapLayer ml = editedMap.findLayer(layer);
				if(ml == null) return;
				int mli = editedMap.findLayerIndex(ml);

				if(x>=0&&x<ml.Width&&y>=0&&y<ml.Height)
				{
					if(bTweak)
					{
						ml.setTile(x,y,val);
					}
					else
					{
						opManager.addExec(new Ops.SetTile(0,mli,x,y,val));
						bDirty = true;
					}
				}
			}

			public void setTile(int x, int y, int val)
			{
				if(x>=0&&x<editedLayer.Width&&y>=0&&y<editedLayer.Height)
				{
					if(bTweak)
					{
						editedLayer.setTile(x,y,val);
					}
					else
					{
						opManager.addExec(new Ops.SetTile(0,editedLayerIndex,x,y,val));
						bDirty = true;
					}
				}
			}

			public int getTile(int x, int y)
			{
				if(x>=0&&x<editedLayer.Width&&y>=0&&y<editedLayer.Height)
					return editedLayer.getTile(x,y);
				else
					return -1;
			}

			//public void invalidate() { if(invalidateEvent != null) invalidateEvent(null,null); }
			//public event EventHandler invalidateEvent;


			/*public MapCursorLocation start, current, previous, end;
			public ILayer editedLayer;
			public bool lb,rb;

			public Metatile currMetatile { get { return Global.currMetatileRec; } }
			public int currMetatileIndex { get { return Global.currMetatile; } set { Global.setCurrMetatile(value); } }

			public void invalidate() { Global.mapEditor.Invalidate(); }
			public bool bMapDirtied = false;
			public int getTile(int x, int y)
			{
				if(x>=0&&x<editedLayer.width&&y>=0&&y<editedLayer.height)
					return (int)editedLayer.getValue(x,y);				
				else return -1;
			}
			public void setTile(int x, int y, object o, bool bUndo)
			{
				if(x>=0&&x<editedLayer.width&&y>=0&&y<editedLayer.height)
				{
					if(bUndo)
						undo.add(new LayerUndoRecs.SetValue(editedLayer,x,y));

					if(editedLayer.getValue(x,y) != o)
					{
						bMapDirtied = true;
						editedLayer.setValue(x,y,o);
					}

				}
			}*/
		}

	/*	public class PluginManager
		{
			public static PluginManager singleton = new PluginManager();
			public IMapPlugin currMapPlugin = null;
		}

		public class PluginHelper
		{
			public static MapEventInfo currMapEventInfo { get { return new MapEventInfo(); } }
			public static IMapPlugin currMapPlugin { get { return PluginManager.singleton.currMapPlugin; } }
		}*/

		/*public class MenuPluginHelper
		{
			public static MenuDocument menuDocument { get { return new MenuDocument(); } }
			public static void addMenuPath(MenuItem menu, MenuItem menuItem, params string[] submenus)
			{
				MainMenu temp = new MainMenu();
				Menu old = (Menu)temp;
				foreach(string s in submenus)
				{
					MenuItem mi = new MenuItem(s);
					mi.MergeType = MenuMerge.MergeItems;
					mi.MergeOrder = s.GetHashCode();
					old.MenuItems.Add(mi);				
					old = (Menu)mi;
				}
				old.MenuItems.Add(menuItem);
				menu.MergeMenu(temp);
			}
		}*/

	}

}