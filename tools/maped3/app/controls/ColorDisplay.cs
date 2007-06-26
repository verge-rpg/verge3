using System;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;

namespace winmaped2
{
	public class ColorDisplaySolid : Control
	{
		ColorInfo controller;
		SolidBrush selected_color;


		public ColorDisplaySolid()
		{
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.UserMouse,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
			selected_color = new SolidBrush(Color.Black);
		}
		public void repaint()
		{
			Graphics g = CreateGraphics();
			paint(g);
			g.Dispose();
		}
		private void paint(Graphics g)
		{
			g.FillRectangle(selected_color,2,2,Width-4,Height-4);
			PaintFunctions.PaintFrame(g, 0,0,Width,Height);
		}
		public void SetController(ColorInfo ci)
		{
			controller = ci;
			ci.ColorChanged += new ColorInfo.CEvent(ColorChanged);
		}
		protected override void OnPaint(PaintEventArgs e)
		{
            paint(e.Graphics);
		}

		private void ColorChanged()
		{
			selected_color = new SolidBrush(Color.FromArgb(
				controller.R,
				controller.G,
				controller.B));
		}


	}
	public class ColorDisplay : RadioButton
	{
		ColorPicker controller;
		SolidBrush selected_color;
		public ColorDisplay()
		{
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.UserMouse,true);
			selected_color = new SolidBrush(Color.Black);
		}
		public void SetController(ColorPicker cp)
		{
			controller = cp;
			controller.colorInfo.ColorChanged += new ColorInfo.CEvent(ColorChanged);
		}
		protected override void OnPaint(PaintEventArgs e)
		{
			//			base.OnPaint (e);
			e.Graphics.FillRectangle(selected_color,0,0,Width,Height);

			if(Checked)
			{
				e.Graphics.DrawRectangle(Pens.Black,0,0,Width-1,Height-1);
				e.Graphics.DrawRectangle(Pens.White,1,1,Width-3,Height-3);
				e.Graphics.DrawRectangle(Pens.Black,2,2,Width-5,Height-5);
			}
		}

		private void ColorChanged()
		{
			selected_color = new SolidBrush(Color.FromArgb(
				Math.Min(controller.colorInfo.R,255),
				Math.Min(controller.colorInfo.G,255),
				Math.Min(controller.colorInfo.B,255)));
			Invalidate();
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if ( (e.Clicks == 1||e.Clicks==2) && e.Button == MouseButtons.Left )
			{
				if (!Checked) Checked=true;
				else
				{
					ColorDialog cd = new ColorDialog(controller.colorInfo.ToColor());
					if (cd.ShowDialog()==DialogResult.OK)
						controller.colorInfo.GrabFromColor(cd.SelectedColor);
				}
			}
		}

	}
}
