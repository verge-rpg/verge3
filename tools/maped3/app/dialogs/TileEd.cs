using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for TileEd.
	/// </summary>
	public class TileEd : System.Windows.Forms.Form
	{
		private winmaped2.VSPController vspController;
		private System.Windows.Forms.TabPage tpBrush;
		private winmaped2.ToolPalette toolPalette;
		private System.Windows.Forms.TabPage tpBrushProperties;
		private System.Windows.Forms.TabPage tabPage1;
		private winmaped2.ToolPalette toolPalette1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.TabPage tabPage3;
		private winmaped2.ToolPalette toolPalette2;
		private System.Windows.Forms.TabPage tabPage4;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private ColorDisplay gForegroundColor;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tpColorSliders;
		private System.Windows.Forms.TabPage tpColorSwatch;
		private winmaped2.ColorPicker colorPickerFront;
		private winmaped2.ColorPicker colorPickerBack;
		private winmaped2.ColorDisplay gBackgroundColor;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBox2;
		private winmaped2.TileEditor tve_a;
		private winmaped2.TileEditor tve_b;

		Vsp24 working_vsp;
		ThrottleBuffer throttleDisplay;
		public delegate void VspDataChangedHandler(Vsp24 vsp, int tileIndex);
		public event VspDataChangedHandler VspDataChanged;

		public TileEd()
		{
			InitializeComponent();

			throttleDisplay = new ThrottleBuffer(500, new EventHandler(postRedisplay));
		}

		private void postRedisplay(object sender, EventArgs args)
		{
			Invoke((MethodInvoker)delegate()
			{
				Invalidate(true);
			}
			);
		}

		public void init(Vsp24 vsp)
		{
			init(vsp, 0, 0);
		}

		public void init(Vsp24 vsp, int leftSelection, int rightSelection)
		{
			working_vsp=vsp;
			gForegroundColor.SetController(colorPickerFront);
			gBackgroundColor.SetController(colorPickerBack);
			vspController.SetControllerMode(VSPController.ControllerMode.SelectorDual);
			vspController.SetActiveVsp(vsp);
			vspController.SetTileViewers(tve_a,tve_b);
			vspController.VspView.SelectedTileF = leftSelection;
			vspController.VspView.SelectedTileB = rightSelection;

			gForegroundColor.CheckedChanged += new EventHandler(gForegroundColor_CheckedChanged);
			gBackgroundColor.CheckedChanged += new EventHandler(gBackgroundColor_CheckedChanged);

			tve_a.TileDataChanged += new TileEditor.TileDataChangedEventHandler(tve_a_TileDataChanged);
			tve_b.TileDataChanged += new TileEditor.TileDataChangedEventHandler(tve_b_TileDataChanged);

			tve_a.SourceLeft = colorPickerFront;
			tve_a.SourceRight = colorPickerBack;
			tve_b.SourceLeft = colorPickerFront;
			tve_b.SourceRight = colorPickerBack;
		}

		void tve_b_TileDataChanged(int TileIndex)
		{
			//throw new Exception("The method or operation is not implemented.");
			throttleDisplay.signal();
			if (VspDataChanged != null)
				VspDataChanged(working_vsp, TileIndex);
		}

		void tve_a_TileDataChanged(int TileIndex)
		{
			//throw new Exception("The method or operation is not implemented.");
			throttleDisplay.signal();
			if (VspDataChanged != null)
				VspDataChanged(working_vsp, TileIndex);
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
			this.colorPickerFront = new winmaped2.ColorPicker();
			this.vspController = new winmaped2.VSPController();
			this.tpBrush = new System.Windows.Forms.TabPage();
			this.tpBrushProperties = new System.Windows.Forms.TabPage();
			this.toolPalette = new winmaped2.ToolPalette();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.toolPalette1 = new winmaped2.ToolPalette();
			this.tabPage3 = new System.Windows.Forms.TabPage();
			this.tabPage4 = new System.Windows.Forms.TabPage();
			this.toolPalette2 = new winmaped2.ToolPalette();
			this.gForegroundColor = new winmaped2.ColorDisplay();
			this.gBackgroundColor = new winmaped2.ColorDisplay();
			this.panel2 = new System.Windows.Forms.Panel();
			this.colorPickerBack = new winmaped2.ColorPicker();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tpColorSliders = new System.Windows.Forms.TabPage();
			this.tpColorSwatch = new System.Windows.Forms.TabPage();
			this.tve_a = new winmaped2.TileEditor();
			this.tve_b = new winmaped2.TileEditor();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.panel2.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tpColorSliders.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// colorPickerFront
			// 
			this.colorPickerFront.Location = new System.Drawing.Point(72, 8);
			this.colorPickerFront.Name = "colorPickerFront";
			this.colorPickerFront.Size = new System.Drawing.Size(176, 88);
			this.colorPickerFront.TabIndex = 2;
			// 
			// vspController
			// 
			this.vspController.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.vspController.Location = new System.Drawing.Point(8, 160);
			this.vspController.Name = "vspController";
			this.vspController.Size = new System.Drawing.Size(340, 436);
			this.vspController.TabIndex = 1;
			// 
			// tpBrush
			// 
			this.tpBrush.Location = new System.Drawing.Point(0, 0);
			this.tpBrush.Name = "tpBrush";
			this.tpBrush.TabIndex = 0;
			// 
			// tpBrushProperties
			// 
			this.tpBrushProperties.Location = new System.Drawing.Point(0, 0);
			this.tpBrushProperties.Name = "tpBrushProperties";
			this.tpBrushProperties.TabIndex = 0;
			// 
			// toolPalette
			// 
			this.toolPalette.Location = new System.Drawing.Point(0, 0);
			this.toolPalette.Name = "toolPalette";
			this.toolPalette.TabIndex = 0;
			// 
			// tabPage1
			// 
			this.tabPage1.Location = new System.Drawing.Point(0, 0);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.TabIndex = 0;
			// 
			// tabPage2
			// 
			this.tabPage2.Location = new System.Drawing.Point(0, 0);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.TabIndex = 0;
			// 
			// toolPalette1
			// 
			this.toolPalette1.Location = new System.Drawing.Point(0, 0);
			this.toolPalette1.Name = "toolPalette1";
			this.toolPalette1.TabIndex = 0;
			// 
			// tabPage3
			// 
			this.tabPage3.Location = new System.Drawing.Point(0, 0);
			this.tabPage3.Name = "tabPage3";
			this.tabPage3.TabIndex = 0;
			// 
			// tabPage4
			// 
			this.tabPage4.Location = new System.Drawing.Point(0, 0);
			this.tabPage4.Name = "tabPage4";
			this.tabPage4.TabIndex = 0;
			// 
			// toolPalette2
			// 
			this.toolPalette2.Location = new System.Drawing.Point(0, 0);
			this.toolPalette2.Name = "toolPalette2";
			this.toolPalette2.TabIndex = 0;
			// 
			// gForegroundColor
			// 
			this.gForegroundColor.Checked = true;
			this.gForegroundColor.Location = new System.Drawing.Point(16, 8);
			this.gForegroundColor.Name = "gForegroundColor";
			this.gForegroundColor.Size = new System.Drawing.Size(24, 24);
			this.gForegroundColor.TabIndex = 3;
			this.gForegroundColor.TabStop = true;
			// 
			// gBackgroundColor
			// 
			this.gBackgroundColor.Location = new System.Drawing.Point(32, 24);
			this.gBackgroundColor.Name = "gBackgroundColor";
			this.gBackgroundColor.Size = new System.Drawing.Size(24, 24);
			this.gBackgroundColor.TabIndex = 3;
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.colorPickerBack);
			this.panel2.Controls.Add(this.gForegroundColor);
			this.panel2.Controls.Add(this.gBackgroundColor);
			this.panel2.Controls.Add(this.colorPickerFront);
			this.panel2.Location = new System.Drawing.Point(0, 0);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(256, 104);
			this.panel2.TabIndex = 4;
			// 
			// colorPickerBack
			// 
			this.colorPickerBack.Location = new System.Drawing.Point(72, 8);
			this.colorPickerBack.Name = "colorPickerBack";
			this.colorPickerBack.Size = new System.Drawing.Size(176, 88);
			this.colorPickerBack.TabIndex = 4;
			this.colorPickerBack.Visible = false;
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tpColorSliders);
			this.tabControl1.Controls.Add(this.tpColorSwatch);
			this.tabControl1.Location = new System.Drawing.Point(8, 16);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(264, 128);
			this.tabControl1.TabIndex = 5;
			// 
			// tpColorSliders
			// 
			this.tpColorSliders.Controls.Add(this.panel2);
			this.tpColorSliders.Location = new System.Drawing.Point(4, 22);
			this.tpColorSliders.Name = "tpColorSliders";
			this.tpColorSliders.Size = new System.Drawing.Size(256, 102);
			this.tpColorSliders.TabIndex = 0;
			this.tpColorSliders.Text = "    Color    ";
			// 
			// tpColorSwatch
			// 
			this.tpColorSwatch.Location = new System.Drawing.Point(4, 22);
			this.tpColorSwatch.Name = "tpColorSwatch";
			this.tpColorSwatch.Size = new System.Drawing.Size(256, 102);
			this.tpColorSwatch.TabIndex = 1;
			this.tpColorSwatch.Text = "    Common Colors    ";
			// 
			// tve_a
			// 
			this.tve_a.ActiveObsTile = null;
			this.tve_a.ActiveTile = null;
			this.tve_a.ActiveTileIndex = 0;
			this.tve_a.Location = new System.Drawing.Point(8, 16);
			this.tve_a.Name = "tve_a";
			this.tve_a.Size = new System.Drawing.Size(288, 288);
			this.tve_a.TabIndex = 0;
			// 
			// tve_b
			// 
			this.tve_b.ActiveObsTile = null;
			this.tve_b.ActiveTile = null;
			this.tve_b.ActiveTileIndex = 0;
			this.tve_b.Location = new System.Drawing.Point(8, 312);
			this.tve_b.Name = "tve_b";
			this.tve_b.Size = new System.Drawing.Size(288, 288);
			this.tve_b.TabIndex = 1;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.tve_a);
			this.groupBox1.Controls.Add(this.tve_b);
			this.groupBox1.Location = new System.Drawing.Point(376, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(312, 608);
			this.groupBox1.TabIndex = 7;
			this.groupBox1.TabStop = false;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.vspController);
			this.groupBox2.Controls.Add(this.tabControl1);
			this.groupBox2.Location = new System.Drawing.Point(8, 8);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(360, 608);
			this.groupBox2.TabIndex = 8;
			this.groupBox2.TabStop = false;
			// 
			// TileEd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(698, 623);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "TileEd";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "TileEd";
			this.panel2.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.tpColorSliders.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void gForegroundColor_CheckedChanged(object sender, EventArgs e)
		{
            if (gForegroundColor.Checked)
				colorPickerFront.Visible=true;
			else colorPickerFront.Visible=false;
		}

		private void gBackgroundColor_CheckedChanged(object sender, EventArgs e)
		{
            if (gBackgroundColor.Checked)
				colorPickerBack.Visible=true;
			else colorPickerBack.Visible=false;
		}
	}
}
