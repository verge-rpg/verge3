using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for LayerTool.
	/// </summary>
	public class LayerTool : System.Windows.Forms.Form
	{
		private System.ComponentModel.IContainer components;
		public System.Windows.Forms.ImageList imageList;

		public ArrayList lwLayers = new ArrayList();

		public LPanel this[int layerRef]
		{
			get
			{
				return (LPanel) lwLayers[layerRef+3]; // first 3 are special
			}
		}
		public LayerTool()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();



		}




		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(LayerTool));
			this.imageList = new System.Windows.Forms.ImageList(this.components);
			// 
			// imageList
			// 
			this.imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth24Bit;
			this.imageList.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
			this.imageList.TransparentColor = System.Drawing.Color.Magenta;
			// 
			// LayerTool
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(234, 166);
			this.ControlBox = false;
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LayerTool";
			this.ShowInTaskbar = false;
			this.Text = "LayerTool";
			this.TopMost = true;

		}
		#endregion
	}
}
