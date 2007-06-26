using System;
using System.Collections;
using System.Windows.Forms;

namespace winmaped2
{
	public class ToggleButton : Button
	{
		public ToggleButton() : base( )
		{
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.UserMouse,true);
		}
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);
		}
		protected override void OnMouseHover(EventArgs e)
		{
//			base.OnMouseHover (e);
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
//			base.OnMouseMove (e);
		}
		protected override void OnMouseEnter(EventArgs e)
		{
//			base.OnMouseEnter (e);
		}


	}
}