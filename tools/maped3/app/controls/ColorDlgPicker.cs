using System;
using System.Windows.Forms;
using System.Drawing;

namespace winmaped2
{
	public class ColorDlgPicker : Control
	{
        /**************************************************/
		/* variables                                      */
		/**************************************************/
		Rectangle active_rect;
		bool mouseIsDragging = false;
		int _bhue;
		bool redraw_image = false;
		float _sat=1.0f,_bri=0.5f;
		int mx=255, my=128;
		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged (e);
			active_rect = new Rectangle(2,2,Width-4,Height-4);
		}

		public delegate void CEvent();
		public event CEvent ColorChanged;
		Bitmap bmpDisplay;
		ColorDisplaySolid cds;
		ColorDialog cdlg;

		
		/**************************************************/
		/* properties                                     */
		/**************************************************/
		public int BaseHue { get { return _bhue; } set { _bhue=value; redraw_image=true; SetVals(mx,my); Invalidate(); } }
		public float Saturation { get { return _sat; } set { _sat=value; mx=(int)(_sat*255); if(ColorChanged!=null)ColorChanged(); Invalidate(); } }
		public float Brightness { get { return _bri; } set { _bri=value; my=(int)(_bri*255); if(ColorChanged!=null)ColorChanged(); Invalidate(); } }
        
		/**************************************************/
		/* constructors                                   */
		/**************************************************/
		public ColorDlgPicker()
		{
			SetStyle(ControlStyles.UserMouse,true);
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
			bmpDisplay = Render.createBitmap(256,256);
			Cursor = Cursors.Cross;
		}

		/**************************************************/
		/* painting                                       */
		/**************************************************/
		public void SetDisplayTarget(ColorDisplaySolid cd)
		{
			cds=cd;
		}
		public void SetDlg(ColorDialog dlg)
		{
			cdlg=dlg;
		}
		protected override void OnPaint(PaintEventArgs e)
		{
			if(cds!=null)cds.repaint();
			if(cdlg!=null)cdlg.update();
			if(redraw_image)
			{
				pr2.Render.Image img = Render.Image.lockBitmap(bmpDisplay);
				Render.renderColorPicker(img, _bhue);
				img.Dispose();
				redraw_image=false;
			}

			e.Graphics.DrawImage(bmpDisplay,0,0,Width,Height);
			e.Graphics.DrawEllipse(Pens.Black,mx-5,my-5,10,10);
			e.Graphics.DrawEllipse(Pens.White,mx-4,my-4,8,8);
			e.Graphics.DrawEllipse(Pens.Black,mx-3,my-3,6,6);
			
			PaintFunctions.PaintFrame(e.Graphics,0,0,Width,Height);			
		}

		/**************************************************/
		/* mouse                                          */
		/**************************************************/
		protected void SetVals(int x, int y)
		{			
			Saturation = (float)x/255;
			Brightness = (float)y/255;
		}
		protected void SeekTo(int x, int y)
		{
			x -= 2;
			if(x<0)x=0;
			if(x>255)x=255;
			
			y -= 2;
			if(y<0)y=0;
			if(y>255)y=255;

			mx=x; my=y;

			SetVals(x,y);
		}
		protected override void OnMouseDown(MouseEventArgs e)
		{
			if(e.Button!=MouseButtons.Left)return;
			if (!active_rect.Contains(new Point(e.X,e.Y))) return;
			SeekTo(e.X,e.Y);
			mouseIsDragging=true;
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
			if(mouseIsDragging&&e.Button==MouseButtons.Left)
			{
				SeekTo(e.X,e.Y);
			}
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			if(mouseIsDragging&&e.Button==MouseButtons.Left)
			{
				SeekTo(e.X,e.Y);
				mouseIsDragging=false;
			}
		}

	}
}
