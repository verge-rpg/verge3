using System;
using System.Windows.Forms;
using System.Drawing;

namespace winmaped2
{
	public class LPanel : System.Windows.Forms.Panel
	{
		ContextMenu cmenu;
		public bool	Render;
		public bool	Write;
		

		public int LayerRef;
		public MapLayer mLayerRef;

		Font nameFont = new System.Drawing.Font("Verdana", 8.5F, FontStyle.Regular);
		Font nameFontS = new Font("Verdana", 8.5F, FontStyle.Bold | FontStyle.Italic);
		Font plaxFont = new System.Drawing.Font("Verdana", 6.5F, FontStyle.Regular);
		SolidBrush sbspec = new SolidBrush(Color.FromArgb(198,244,230));

	

		private void DrawFrame(PaintEventArgs e, int x, int y, int w, int h)
		{
			e.Graphics.DrawLine(SystemPens.ControlLightLight, x, y, x+w, y);
			e.Graphics.DrawLine(SystemPens.ControlLightLight, x, y, x, y+h);
			e.Graphics.DrawLine(SystemPens.ControlDark, x+w,y+1,x+w, y+h);
			e.Graphics.DrawLine(SystemPens.ControlDark, x, y+h, x+w, y+h);
		}
		private void DrawFrameInv(PaintEventArgs e, int x, int y, int w, int h)
		{
			e.Graphics.DrawLine(SystemPens.ControlDark, x, y, x+w, y);
			e.Graphics.DrawLine(SystemPens.ControlDark, x, y, x, y+h);
			e.Graphics.DrawLine(SystemPens.ControlLightLight, x+w,y+1,x+w, y+h);
			e.Graphics.DrawLine(SystemPens.ControlLightLight, x, y+h, x+w, y+h);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			if(mLayerRef==null||Global.ActiveMap==null) return;
			if(mLayerRef is MapLayerSpecial)
			{
				e.Graphics.FillRectangle(sbspec, 0,0,Width,Height);
			}
			else
			{
				e.Graphics.FillRectangle(SystemBrushes.Control, 0, 0, Width, Height);
			}
			e.Graphics.DrawLine(Pens.Black, 0, 0, Width-1, 0);
			if(! (mLayerRef is MapLayerSpecial) )
			{
				DrawFrame(e, 0, 1, 44, Height-3);
				DrawFrameInv(e, 4, 4, 16, 16);
				if ( Render ) e.Graphics.DrawImage(Images.BmpRender, 5,5, 16,16);

				DrawFrameInv(e, 24, 4, 16, 16);
				
				if ( Write ) e.Graphics.DrawImage(Images.BmpWrite, 25,5, 16,16);
				DrawFrame(e, 46, 1, Width - 48, Height-3);
				e.Graphics.DrawLine(Pens.Black, 45, 0, 45, Height);
				if( Write )
					e.Graphics.FillRectangle(Brushes.White, 46,2,Width-46,Height-4);
			}
			else
			{
				DrawFrame(e, 1, 1, Width, Height-3);
				if( Write )
					e.Graphics.FillRectangle(Brushes.White, 0,2,Width,Height-4);
			}
			

			
			e.Graphics.DrawLine(Pens.Black, 0, Height-1, Width-1, Height-1);
			e.Graphics.DrawLine(Pens.Black, Width-1, 0, Width-1, Height-1);
			

			Font f=null;
			int h = e.Graphics.MeasureString(mLayerRef.name, nameFont).ToSize().Height;
			if(Global.ActiveMap.Layers.IndexOf(mLayerRef)==0)
				f=nameFontS;
			else f=nameFont;
				
			string output = mLayerRef.name;
			if(mLayerRef.type == LayerType.Tile)
				output = mLayerRef.ID.ToString() + ": "+output;
			e.Graphics.DrawString(output, f, SystemBrushes.ControlText, 50, Height/2 - h/2);
			int w=0;

            
			if(mLayerRef.type!=LayerType.Tile)return;
			
			string spl = (100-mLayerRef.Translucency).ToString() + "%";

			Size s = e.Graphics.MeasureString(spl, plaxFont).ToSize();
			w = s.Width;
			h = s.Height;
			e.Graphics.DrawString(spl, plaxFont, SystemBrushes.ControlText, Width - 60, Height/2 - h/2);

			// draw opacity slider

			Graphics g = e.Graphics;

			PaintFunctions.PaintFrame(g, SliderRect.X, SliderRect.Y, 104, 8);
			g.FillRectangle(Brushes.DarkBlue, SliderRect.X+2, SliderRect.Y+2, 100-mLayerRef.Translucency,4);
			
		}
		Rectangle SliderRect = new Rectangle(160, 8, 104, 8);
		bool SliderHasFocus = false;

		bool _adjusted=false;
		public void AdjustForScroll()
		{
			if(!_adjusted)
				SliderRect.X -= SystemInformation.VerticalScrollBarWidth-2;
			_adjusted=true;
		}

		private void SlideTo(int x)
		{
			mLayerRef.Translucency = 100 - Math.Max( Math.Min(x,100), 0 );
			Invalidate();
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if ( e.X >= 4 && e.X <= 20 && e.Y >= 4 && e.Y <= 20 )
			{
				if ( Render == false )
				{
					Render = true;
					if ( Global.ActiveMap != null ) 
					{
						Global.ActiveMap.UIState[LayerRef].bRender = true;
					}
				}
				else
				{
					//if ( Write ) return;
					Render = false;
					if ( Global.ActiveMap != null )
					{
						Global.ActiveMap.UIState[LayerRef].bRender = false;
					}
				}
				Global.ForceRedraws();

			}
			else if ( e.X >= 24 && e.X <= 40 && e.Y >= 4 && e.Y <= 20 )
			{
				SelectForWrite();
				Global.ForceRedraws();
			}
			else if ( e.Button == MouseButtons.Left && SliderRect.Contains(e.X,e.Y)&&mLayerRef.type==LayerType.Tile )
			{
				SliderHasFocus = true;
				SlideTo(e.X - SliderRect.X);
			}
			else if ( e.Clicks==1&&e.Button==MouseButtons.Left&&e.X>45 )
			{
				SelectForWrite();
			}
			else if ( e.Clicks==2&&e.Button==MouseButtons.Left )
			{
				if(mLayerRef.type==LayerType.Tile)
					open_properties();
				else if(mLayerRef.type==LayerType.Zone)
				{
					Global.mainWindow.OpenZoneEditor();
				}
				else if(mLayerRef.type==LayerType.Entity)
				{
					Global.mainWindow.OpenEntityEditor();
				}
			}
			
			Invalidate();
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
			if(e.Button==MouseButtons.Left&&SliderHasFocus)
				SlideTo(e.X - SliderRect.X);
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			if(e.Button==MouseButtons.Left&&SliderHasFocus)
				SliderHasFocus=false;
		}


		public void SelectForWrite()
		{
			if ( Write == true ) return;
			Write = true;
			Render = true;

			if ( Global.ActiveMap != null ) Global.ActiveMap.UIState[LayerRef].bRender = true;

			if ( Global.lpSelection != null )
			{
				LPanel lp = ((LPanel)Global.lpSelection);
				lp.Write = false;
				lp.Invalidate();
			}
			Global.lpSelection = this;
			Global.FireWriteEvent(new Global.LEventArgs(LayerRef));
		
		}
		public LPanel(Panel lTool, MapLayer ml, bool render, bool write, int layer)
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
			SetStyle(ControlStyles.Opaque,true);
			SetStyle(ControlStyles.UserMouse,true);

			LayerRef = layer;
			mLayerRef = ml;

			Cursor = Cursors.Hand;

			if(mLayerRef is MapLayerSpecial)
				Size = new Size(lTool.Width-4,16);
			else
				Size = new Size(lTool.Width-4, 25);
				
			Render=render;
			Write=write;
			if ( Write ) Global.lpSelection = this;	
			if( ml.type != LayerType.Tile )
				return;
			
			cmenu = new ContextMenu();

			MenuItem m_properties = new MenuItem("Properties", new EventHandler(properties_menu));
			cmenu.MenuItems.Add(m_properties);
			cmenu.Popup += new EventHandler(cmenu_Popup);
			ContextMenu = cmenu;
		}
		private void open_properties()
		{	
			LayerPropertiesWnd lpw = new LayerPropertiesWnd();
			lpw.init(mLayerRef);
			if(lpw.ShowDialog()==DialogResult.Cancel) return;
			lpw.setvalues(mLayerRef);
			mLayerRef.parentmap.touch();
			Global.ForceRedraws();

		}
		private void properties_menu(object sender, EventArgs e)
		{
			open_properties();
			Invalidate();
		}
		
		private void cmenu_Popup(object sender, EventArgs e)
		{
		}
	}
	public class LayerPanel : Panel
	{

		bool bMouseContained=false;
		int mx=0,my=0;
		public LayerPanel() : base()
		{
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);

		}
		protected override void OnInvalidated(InvalidateEventArgs e)
		{
			base.OnInvalidated(e);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			//e.Graphics.FillRectangle(SystemBrushes.Control,0,0,Width,Height);
			PaintFunctions.PaintFrame(e.Graphics,0,0,Width,Height);
		}
		protected override void OnMouseEnter(EventArgs e)
		{
			base.OnMouseEnter (e);
			bMouseContained=true;
		}
		protected override void OnMouseLeave(EventArgs e)
		{
			base.OnMouseLeave (e);
			bMouseContained=false;
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove (e);
			if(bMouseContained)
			{
				mx = e.X;
				my = e.Y;
			}
		}


		public void SetControlLayouts()
		{
			int w = Width-4;
			int count = Controls.Count;

			int yofs=0;
			int ysize=4;
			foreach(LPanel lp in Controls)
			{
				ysize += lp.Height-1;
			}
			if(ysize>Height)
				w -= SystemInformation.VerticalScrollBarWidth-2;
			for(int i=0;i<count;i++)
			{
				LPanel lp = (LPanel)Controls[i];
				lp.Size = new Size(w, lp.Size.Height);
				lp.Location = new Point(2, yofs);
				yofs += lp.Size.Height-1;
				if(ysize > Height)
					lp.AdjustForScroll();
				
			}
		}

	}
}
