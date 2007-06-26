using System;
using System.Collections;
using System.Windows.Forms;

namespace winmaped2
{
	public class VSPController : Panel
	{
		/**************************************************/
		/* Static, const, etc                             */
		/**************************************************/
		public enum ControllerMode { ViewOnly, SelectorSingle, SelectorDual };
		public enum ControllerType { VSP, Obstruction };

		

		/**************************************************/
		/* Controls                                       */
		/**************************************************/
		VScrollBar	c_ScrollBar = new VScrollBar();
		vspView		c_VspView;

		public vspView VspView { get { return c_VspView; } }


		/**************************************************/
		/* Constructors                                   */
		/**************************************************/
		public VSPController() : this(ControllerMode.ViewOnly,ControllerType.VSP) {}
		public VSPController(ControllerMode controller_mode, ControllerType controller_type) : base()
		{
			BorderStyle = BorderStyle.Fixed3D;
			//Size = new System.Drawing.Size(340,164);

			c_VspView = new vspView(c_ScrollBar,controller_mode,controller_type);
			c_VspView.Size = new System.Drawing.Size(320,Height-4);
			c_VspView.Location = new System.Drawing.Point(0,0);
			
			c_ScrollBar.Size = new System.Drawing.Size(16,Height-4);
			c_ScrollBar.Location = new System.Drawing.Point(320,0);
			c_ScrollBar.ValueChanged += new EventHandler(c_VspView.OnScroll);

			Controls.Add(c_VspView);
			Controls.Add(c_ScrollBar);
		}

		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged (e);
			c_VspView.Size = new System.Drawing.Size(320,Height-4);
			c_VspView.Location = new System.Drawing.Point(0,0);
			c_ScrollBar.Size = new System.Drawing.Size(16,Height-4);
			c_ScrollBar.Location = new System.Drawing.Point(320,0);
			c_VspView.CalculateScrollValues();
		}


		/**************************************************/
		/* Functions                                      */
		/**************************************************/
		public void ResetView()
		{
			c_VspView.ResetView();
		}
		public void SetControllerMode(ControllerMode controller_mode)
		{
			c_VspView.ControllerMode = controller_mode;
		}
		public void SetControllerType(ControllerType controller_type)
		{
			c_VspView.ControllerType = controller_type;
		}
		public void SetTileViewers(TileViewer a)
		{
			SetTileViewers(a,null);
		}
		public void SetTileViewers(TileViewer a, TileViewer b)
		{
			c_VspView.PrimaryTileViewer = a;
			c_VspView.SecondaryTileViewer = b;
		}
		public void SetActiveVsp(Vsp24 vsp)
		{
			c_VspView.ActiveVsp = vsp;
		}
	}
	public delegate void SEventHandler();
}
