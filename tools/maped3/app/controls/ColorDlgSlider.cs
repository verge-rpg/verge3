using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace winmaped2
{
	public class ColorDlgSlider : Control
	{
		/**************************************************/
		/* variables                                      */
		/**************************************************/
		Rectangle active_rect;
		bool mouseIsDragging = false;
		int _Value;

		/**************************************************/
		/* properties                                     */
		/**************************************************/
		public int SelectedHue 
		{ 
			get { return _Value * 360 /255; } 
			set  
			{
				_Value = value; 
				Invalidate(); 
				
				if(ColorChanged!=null)ColorChanged();
			}
		}
		public delegate void CEvent();
		public event CEvent ColorChanged;
		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged (e);
			active_rect = new Rectangle(2,2,Width-4,Height-4);
		}

		Color[] ColorList = new Color[256];
		/**************************************************/
		/* constructors                                   */
		/**************************************************/
		public ColorDlgSlider()
		{
			SetStyle(ControlStyles.UserMouse,true);
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);

			float s = 1.0f;
			float b = 0.5f;
			float h = 0.0f;
			for(int i=0;i<256;i++)
			{
				h = i * 360 /256;
				ColorList[i] = PaintFunctions.HsbToColor(h,s,b);
			}
		}

		/**************************************************/
		/* painting                                       */
		/**************************************************/
		protected override void OnPaint(PaintEventArgs e)
		{
			for(int i=0;i<256;i++)
			{
				e.Graphics.DrawLine(new Pen(ColorList[i]), 2, 2+i, Width-3,2+i);
			}
			int Value = _Value;
			e.Graphics.DrawLine(Pens.Black,0,2+Value-1, Width,2+Value-1);
			e.Graphics.DrawLine(Pens.White,0,2+Value,Width,2+Value);
			e.Graphics.DrawLine(Pens.Black,0,2+Value+1, Width,2+Value+1);
			PaintFunctions.PaintFrame(e.Graphics, 0, 0, Width, Height);
		}
		
		/**************************************************/
		/* mouse                                          */
		/**************************************************/
		protected override void OnMouseDown(MouseEventArgs e)
		{
			if(e.Button!=MouseButtons.Left)return;
			if (!active_rect.Contains(new Point(e.X,e.Y))) return;
			mouseIsDragging=true;
			int my = e.Y-2;
			if(my<0)my=0;
			if(my>255)my=255;
			SelectedHue = my;
			Invalidate();
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
			if(mouseIsDragging&&e.Button==MouseButtons.Left)
			{
				int my = e.Y-2;
				if(my<0)my=0;
				if(my>255)my=255;
				SelectedHue = my;
				Invalidate();
			}
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			if(mouseIsDragging&&e.Button==MouseButtons.Left)
			{

				mouseIsDragging=false;
			}
		}
	}
}
