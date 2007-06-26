using System;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;


namespace winmaped2
{
	public class ColorTextbox : TextBox
	{
		public ColorTextbox() : base()
		{
//			SetStyle(ControlStyles.UserPaint,true);
//			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
//			SetStyle(ControlStyles.DoubleBuffer,true);
		}
		public void update()
		{
			Graphics g = CreateGraphics();
			paint(g);
			g.Dispose();
		}
		private void paint(Graphics g)
		{
			Rectangle r = new Rectangle(Location.X,Location.Y,Location.X+Width,Location.Y+Height);
			PaintEventArgs pea = new PaintEventArgs(g,r);
			base.OnPaint(pea);
		}
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

	}
}
