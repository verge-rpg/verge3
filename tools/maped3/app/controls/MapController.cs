using System;
using System.Windows.Forms;
using System.Drawing;

namespace winmaped2
{
	public class MapController : Panel
	{
		/**************************************************/
		/* controls                                       */
		/**************************************************/
		HScrollBar	c_hscroll;
		VScrollBar	c_vscroll;
		mapView		c_viewer;
		SizeGrip	c_sizegrip;
		MiniMapControl c_mmc;

		public HScrollBar hScrollBar { get { return c_hscroll; } }
		public VScrollBar vScrollBar { get { return c_vscroll; } }
		public int ZoomLevel { get { return c_viewer.ZoomLevel; } set { c_viewer.ZoomLevel = value; } }
		public Map ParentMap { get { return c_viewer.ParentMap; } set { c_viewer.ParentMap = value; if(value==null) this.Enabled=false; else this.Enabled=true; } }
		public mapView MapView { get { return c_viewer; } }
		

		/**************************************************/
		/* constructors                                   */
		/**************************************************/
		public MapController()
		{
			c_hscroll = new HScrollBar();
			c_hscroll.ValueChanged += new EventHandler(c_hscroll_ValueChanged);
			
			c_vscroll = new VScrollBar();
            c_vscroll.ValueChanged += new EventHandler(c_vscroll_ValueChanged);

			c_viewer = new mapView();
			c_viewer.SetScrollBars(c_vscroll,c_hscroll);
			c_viewer.Size = Size;
			c_viewer.Location = new Point(0,0);

			c_sizegrip = new SizeGrip();

			Controls.Add(c_viewer);
			Controls.Add(c_hscroll);
			Controls.Add(c_vscroll);
			Controls.Add(c_sizegrip);
		}
		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged (e);
			UpdateSize();
		}

		public void UpdateSize()
		{
			Panel p = this;
			c_hscroll.Location = new Point(0,p.ClientRectangle.Height-SystemInformation.HorizontalScrollBarHeight);
			c_vscroll.Location = new Point(p.ClientRectangle.Width-SystemInformation.VerticalScrollBarWidth);
			c_hscroll.Width = p.ClientRectangle.Width - SystemInformation.VerticalScrollBarWidth;
			c_hscroll.Height = SystemInformation.HorizontalScrollBarHeight;
			c_vscroll.Height = p.ClientRectangle.Height - SystemInformation.HorizontalScrollBarHeight;
			c_vscroll.Width = SystemInformation.VerticalScrollBarWidth;
			c_sizegrip.Location = new Point(p.ClientRectangle.Width-SystemInformation.VerticalScrollBarWidth,p.ClientRectangle.Height-SystemInformation.HorizontalScrollBarHeight);
			c_viewer.Size = new Size(p.ClientRectangle.Width-SystemInformation.VerticalScrollBarWidth,p.ClientRectangle.Height-SystemInformation.HorizontalScrollBarHeight);
			c_viewer.UpdateSize();
		}


		private void c_hscroll_ValueChanged(object sender, EventArgs e)
		{
			if(c_mmc!=null) c_mmc.repaint();
            c_viewer.Invalidate();
		}

		private void c_vscroll_ValueChanged(object sender, EventArgs e)
		{
			if(c_mmc!=null) c_mmc.repaint();
			c_viewer.Invalidate();
		}
		public void AssociateMinimap(MiniMapControl mmc)
		{
			c_mmc = mmc;
		}
	}
}
