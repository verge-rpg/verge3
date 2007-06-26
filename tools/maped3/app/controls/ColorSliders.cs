using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for ColorSliders.
	/// </summary>
	public class ColorSlider : Control
	{
		/**************************************************/
		/* Data structures                                */
		/**************************************************/
		struct MouseInfo 
		{
			public bool LButton, RButton;
			public Point LDownAt, RDownAt;
		}

		public enum ColorChannel { Red, Green, Blue, Alpha }

		/**************************************************/
		/* Variables/Properties                           */
		/**************************************************/
		private int _maxval = 255;
		private int _minval = 0;
		private int _value = 0;
		private ColorChannel _channel = ColorChannel.Red;
		private ColorInfo _pcolor;
		private bool _ignoreci;
		
		public int Value { set { _value=value;ValueChanged(); } get { return _value; } }
		public int Minimum { set { _minval=value; } get { return _minval; } }
		public int Maximum { set { _maxval=value; } get { return _maxval; } }
		public ColorChannel PrimaryColorChannel { set { _channel = value; } get { return _channel; } }
		public ColorInfo ParentColor { set { _pcolor=value; _pcolor.ColorChanged += new ColorInfo.CEvent(colchanged); } }
		void colchanged()
		{
			if(!_ignoreci)
			{
				switch(PrimaryColorChannel)
				{
					case ColorChannel.Red: _value=_pcolor.R;break;
					case ColorChannel.Green: _value=_pcolor.G;break;
					case ColorChannel.Blue: _value=_pcolor.B;break;
					case ColorChannel.Alpha: _value=_pcolor.A;break;
					default:break;
				}
			}
		}
		MouseInfo MouseData = new MouseInfo();


		/**************************************************/
		/* Constructors                                   */
		/**************************************************/
		public ColorSlider()
		{
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.UserMouse,true);
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
		}


		/**************************************************/
		/* Painting                                       */
		/**************************************************/
		protected override void OnPaint(PaintEventArgs e)
		{
			int cx = 2 + Value * (Width-6) / _maxval;
			switch(_channel)
			{
				// draw gradient stuffs
				case ColorChannel.Red:
					for(int i=0;i<Width;i++)
					{
						int red = i * _maxval /Width;
						Pen p = new Pen(Color.FromArgb(red,_pcolor.G,_pcolor.B));
						e.Graphics.DrawLine(p,i,0,i,Height-1);
					}
					break;
				case ColorChannel.Green:
					for(int i=0;i<Width;i++)
					{
						int green = i * _maxval /Width;
						Pen p = new Pen(Color.FromArgb(_pcolor.R,green,_pcolor.B));
						e.Graphics.DrawLine(p,i,0,i,Height-1);
					}
					break;
				case ColorChannel.Blue:
					for(int i=0;i<Width;i++)
					{
						int blue = i * _maxval /Width;
						Pen p = new Pen(Color.FromArgb(_pcolor.R,_pcolor.G,blue));
						e.Graphics.DrawLine(p,i,0,i,Height-1);
					}
					break;
				default:break;
			}
			Rectangle r = e.ClipRectangle;
			//e.Graphics.DrawLine(Pens.Red, cx, 0, cx, Height-1);
			e.Graphics.DrawLine(Pens.Black, cx-1,0,cx-1,Height-1);
			e.Graphics.DrawLine(SystemPens.ControlLightLight, cx,0,cx,Height-1);
			e.Graphics.DrawLine(Pens.Black, cx+1,0,cx+1,Height-1);
/*			e.Graphics.DrawRectangle(Pens.Black, 0, 0, Width-1,Height-1);
			e.Graphics.DrawRectangle(SystemPens.ControlLight,1,1,Width-3,Height-3);
			e.Graphics.DrawRectangle(SystemPens.ControlLight,2,2,Width-5,Height-5);
			e.Graphics.DrawRectangle(Pens.Black, 3,3, Width-7, Height-7);*/
			PaintFunctions.PaintFrame(e.Graphics,0,0,Width,Height);
		}

		/**************************************************/
		/* Mouse Events                                   */
		/**************************************************/
		void TargetCursor(int mx)
		{
			mx-=2;
			if (mx<0) mx = 0;
			else if (mx>(Width-6))mx=(Width-6);
//			mx-=4;
				
			Value = (_maxval-_minval)*(mx) /(Width-6);
			_ignoreci = true;
			if (_pcolor!=null)
			{
				switch(_channel)
				{
					case ColorChannel.Red: _pcolor.R=(byte)Value;break;
					case ColorChannel.Green: _pcolor.G=(byte)Value;break;
					case ColorChannel.Blue: _pcolor.B=(byte)Value;break;
					default:break;
				}
			}
			_ignoreci = false;
			Invalidate();
		}
		protected override void OnMouseDown(MouseEventArgs e)
		{
			if ( e.Button == MouseButtons.Left )
			{
				MouseData.LButton = true;
				MouseData.LDownAt = new Point(e.X,e.Y);
				TargetCursor(e.X);
			}
			else if ( e.Button == MouseButtons.Right )
			{
				MouseData.RButton = true;
				MouseData.RDownAt = new Point(e.X,e.Y);
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			if ( e.Button == MouseButtons.Left )
			{
				MouseData.LButton = false;
			}
			else if ( e.Button == MouseButtons.Right )
			{
				MouseData.RButton = false;
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			if ( MouseData.LButton )
			{
				TargetCursor(e.X);
			}
		}

		/**************************************************/
		/* Other events                                   */
		/**************************************************/
		void ValueChanged()
		{
			Invalidate();
		}
	}
	public class ColorInfo
	{
		int r,g,b,a;
		public Color ToColor()
		{
			return Color.FromArgb(r,g,b);
		}
		public void GrabFromColor(Color c)
		{
			r=c.R;
			g=c.G;
			b=c.B;
			a=c.A;
			if(ColorChanged!=null)
				ColorChanged();
		}
		public int R
		{
			set
			{
				r=value;
				if(ColorChanged!=null)
					ColorChanged();
			}
			get	{ return r; }
		}
		public int G
		{
			set
			{
				g=value;
				if(ColorChanged!=null)
					ColorChanged();
			}
			get { return g; }
		}
		public int B
		{
			set
			{
				b=value;
				if(ColorChanged!=null)
					ColorChanged();
			}
			get { return b; }
		}
		public int A
		{
			set
			{
				a=value;
				if(ColorChanged!=null)
					ColorChanged();
			}
			get { return a; }
		}
		public delegate void CEvent();
		public event CEvent ColorChanged;
	}
}
