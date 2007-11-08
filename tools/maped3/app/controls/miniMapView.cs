using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace winmaped2
{

	public class MiniMapControl : Control
	{
		protected override Size DefaultSize
		{
			get
			{
				return new Size(200,200);
			}
		}

		private Bitmap bmp;
		private Rectangle rect;
		public bool bNeedsRedraw = true;

		private Pen rectPen;
		private Pen rectPenW;

		MapController _Controller;
		public MapController Controller 
		{
			get 
			{
				return _Controller;
			}
			set
			{
				_Controller = value;
			}
		}

		public MiniMapControl()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
			SetStyle(ControlStyles.Opaque,true);
			SetStyle(ControlStyles.UserMouse,true);
			

			bmp = new Bitmap(200,200,PixelFormat.Format32bppArgb);
			rect = new Rectangle(0,0,200,200);
			rectPen = new Pen(Brushes.Red, 2.0f);
			rectPenW = new Pen(Brushes.White,2.0f);
			
		}

		private bool lDown = false;

		protected override void WndProc(ref Message m)
		{
			//
			
			base.WndProc (ref m);
		}


		private void MoveToCursor(MouseEventArgs e)
		{
			if ( Global.ActiveMap != null && lDown == true && Controller != null )
			{
				// get map
				Map map = Global.ActiveMap;
				// get base layer
				MapLayer BaseLayer = (MapLayer) Global.ActiveMap.Layers[0];
				// get the vsp
				Vsp24 mapVsp = Global.ActiveVsp;
				// get the mapview
				mapView mv = Global.MainMapController.MapView;


				int vw = 200, vh = 200;
				if(BaseLayer.Width>BaseLayer.Height)
					vh =  (int)(vh * ((float)BaseLayer.Height / (float)BaseLayer.Width));
				else
					vw =  (int)(vw * ((float)BaseLayer.Width / (float)BaseLayer.Height));


				float ratio_x = (float) BaseLayer.Width /vw;
				float ratio_y = (float) BaseLayer.Height /vh;
				
				int xx = (200 - vw) /2;
				int yy = (200 - vh) /2;

				int nx = (int) (ratio_x * (e.X-xx) * 16);
				int ny = (int) (ratio_y * (e.Y-yy) * 16);

				nx -= (mv.Width / Controller.ZoomLevel /2);
				ny -= (mv.Height / Controller.ZoomLevel /2);


				nx += xx;
				ny += yy;

				if ( nx < 0 )
					nx = 0;
				if ( ny < 0 )
					ny = 0;
				if ( nx > Controller.hScrollBar.Maximum-mv.Width /Controller.ZoomLevel )
					nx = Controller.hScrollBar.Maximum-mv.Width/Controller.ZoomLevel;
				if ( ny > Controller.vScrollBar.Maximum-mv.Height/Controller.ZoomLevel )
					ny = Controller.vScrollBar.Maximum-mv.Height/Controller.ZoomLevel;

				if(nx >= Controller.hScrollBar.Minimum)
					Controller.hScrollBar.Value = nx;
				//Controller.hScrollBar.Value = nx;
				if(ny >= Controller.vScrollBar.Minimum)
					Controller.vScrollBar.Value = ny;
				//Controller.vScrollBar.Value = ny;
				repaint();

			}
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			MoveToCursor(e);
			if ( e.Button == MouseButtons.Left ) lDown = false;
		}


		protected override void OnMouseDown(MouseEventArgs e)
		{
			MoveToCursor(e);
			if ( e.Button == MouseButtons.Left ) lDown = true;			
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			MoveToCursor(e);
		}

		public void repaint()
		{
			Graphics g = this.CreateGraphics();
			paint(g);
			g.Dispose();
		}
		

		private unsafe void paint(Graphics g)
		{
			g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
			g.InterpolationMode = InterpolationMode.NearestNeighbor;
			g.CompositingMode = CompositingMode.SourceCopy;
			g.CompositingQuality = CompositingQuality.HighSpeed;
			
			Rectangle r = new Rectangle(0,0,100,100);

			if ( Global.ActiveMap != null&&Controller!=null )
			{
				// get map
				Map map = Global.ActiveMap;
				// get base layer
				MapLayer BaseLayer = (MapLayer) Global.ActiveMap.Layers[0];
				// get the vsp
				Vsp24 mapVsp = Global.ActiveVsp;

				int vw = 200, vh = 200;
				if(BaseLayer.Width>BaseLayer.Height)
					vh =  (int)(vh * ((float)BaseLayer.Height / (float)BaseLayer.Width));
				else
					vw =  (int)(vw * ((float)BaseLayer.Width / (float)BaseLayer.Height));
		

				float ratio_x = (float) BaseLayer.Width /vw;
				float ratio_y = (float) BaseLayer.Height /vh;

				float ratio_xn = (float) vw / (BaseLayer.Width * 16);
				float ratio_yn = (float) vh / (BaseLayer.Height * 16);

				mapView mv = Controller.MapView;

				int pw = BaseLayer.Width * 16;
				int ph = BaseLayer.Height * 16;

				//zero 1/18/03 - note the zoom alterations here
				int nw = mv.Width * vw / Controller.ZoomLevel / pw;
				int nh = mv.Height * vh / Controller.ZoomLevel / ph;

				r.Width = nw;
				r.Height = nh;

				int ox = (int) (Controller.hScrollBar.Value * ratio_xn);
				int oy = (int) (Controller.vScrollBar.Value * ratio_yn);

				r.X = ox;
				r.Y = oy;



				
				if ( bNeedsRedraw )
				{
					BitmapData bmpd = bmp.LockBits(rect, ImageLockMode.WriteOnly, PixelFormat.Format32bppPArgb);
					int* imgptr = (int*)bmpd.Scan0.ToPointer();
					if(mapVsp!=null&&mapVsp.tileCount>0)
					for(int l = 0; l < map.Layers.Count; l++)
					{
						MapLayer layer = (MapLayer)map.Layers[l];
						if(layer.type!=LayerType.Tile) continue;

						for(int i = 0; i < vh; i++)
						{
							for(int j = 0; j < vw; j++)
							{
								int tx = (int) (ratio_x * j);
								int ty = (int) (ratio_y * i);
								if ( tx >= layer.Width || tx < 0 || ty >= layer.Height || ty < 0 )
									continue;
								int tidx = layer.Data[ty * layer.Width + tx];
                                                                if ((l == 0 || tidx != 0) && 0 < tidx && tidx < mapVsp.Tiles.Count)
                                                                {
                                                                    imgptr[i * 200 + j] = ((Vsp24Tile)mapVsp.Tiles[tidx]).ColorAverage;
                                                                }
							}
						}
					}
					bmp.UnlockBits(bmpd);
					bNeedsRedraw = false;
				}
				int xx = (200 - vw) /2;
				int yy = (200 - vh) /2;
				r.X += xx+1;
				r.Y += yy+1;



				// flicker reduce hack
				g.Clip = new Region(new Rectangle(xx,yy,vw+1,vh+1));
				g.DrawImageUnscaled(bmp,xx,yy,vw,vh);
				g.Clip = new Region(new Rectangle(0,0,200,200));
				//


				g.DrawRectangle(rectPen,r);
				g.DrawRectangle(rectPenW, xx-2,yy-2,vw+4,vh+4);
			}
			else
			{
				g.FillRectangle(Brushes.Black, 0, 0, Width, Height);
			}
		}
	

		protected override void OnPaint(PaintEventArgs e)
		{
			paint(e.Graphics);
		}
	

	}

}
