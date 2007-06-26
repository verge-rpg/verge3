using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for LayerPropertiesWnd.
	/// </summary>
	public class LayerPropertiesWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox t_layername;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.LinkLabel linkLabel1;
		private System.Windows.Forms.NumericUpDown n_layerlucency;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.NumericUpDown n_px;
		private System.Windows.Forms.NumericUpDown n_py;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.TextBox t_mw;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.TextBox t_mh;
		private System.Windows.Forms.Button b_resize;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		MapLayer mlayer;

		public LayerPropertiesWnd()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
        
			label6.Text = "0% is opaque\r\n100% is invisible";
		}

		private void updateui()
		{
			MapLayer ml = mlayer;
			t_layername.Text = ml.name;
			n_layerlucency.Value = ml.Translucency;
			n_px.Value = (decimal)ml.parallaxInfo.MultipleX;
			n_py.Value = (decimal)ml.parallaxInfo.MultipleY;
			t_mw.Text = ml.Width.ToString();
			t_mh.Text = ml.Height.ToString();
		}

		public void init(MapLayer ml)
		{
			mlayer = ml;
			updateui();
		}

		public void setvalues(MapLayer ml)
		{
			ml.name = t_layername.Text;
			ml.Translucency = (int)n_layerlucency.Value;
			ml.parallaxInfo.MultipleX = (double)n_px.Value;
			ml.parallaxInfo.MultipleY = (double)n_py.Value;
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
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.b_resize = new System.Windows.Forms.Button();
			this.label8 = new System.Windows.Forms.Label();
			this.t_mw = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.n_px = new System.Windows.Forms.NumericUpDown();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.n_py = new System.Windows.Forms.NumericUpDown();
			this.linkLabel1 = new System.Windows.Forms.LinkLabel();
			this.label3 = new System.Windows.Forms.Label();
			this.n_layerlucency = new System.Windows.Forms.NumericUpDown();
			this.t_layername = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.t_mh = new System.Windows.Forms.TextBox();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_px)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_py)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_layerlucency)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.b_resize);
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.t_mw);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.groupBox2);
			this.groupBox1.Controls.Add(this.linkLabel1);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.n_layerlucency);
			this.groupBox1.Controls.Add(this.t_layername);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.t_mh);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(352, 248);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			// 
			// b_resize
			// 
			this.b_resize.Location = new System.Drawing.Point(264, 192);
			this.b_resize.Name = "b_resize";
			this.b_resize.TabIndex = 103;
			this.b_resize.Text = "Resize";
			this.b_resize.Click += new System.EventHandler(this.b_resize_Click);
			// 
			// label8
			// 
			this.label8.Location = new System.Drawing.Point(168, 200);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(16, 16);
			this.label8.TabIndex = 102;
			this.label8.Text = "X";
			// 
			// t_mw
			// 
			this.t_mw.Location = new System.Drawing.Point(96, 192);
			this.t_mw.Name = "t_mw";
			this.t_mw.ReadOnly = true;
			this.t_mw.Size = new System.Drawing.Size(64, 20);
			this.t_mw.TabIndex = 101;
			this.t_mw.Text = "";
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(16, 192);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(72, 23);
			this.label7.TabIndex = 100;
			this.label7.Text = "Dimensions:";
			this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label6
			// 
			this.label6.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label6.Location = new System.Drawing.Point(216, 64);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(104, 24);
			this.label6.TabIndex = 7;
			this.label6.Text = "0% is Opaque, 100% is Invisible";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.n_px);
			this.groupBox2.Controls.Add(this.label5);
			this.groupBox2.Controls.Add(this.label4);
			this.groupBox2.Controls.Add(this.n_py);
			this.groupBox2.Location = new System.Drawing.Point(16, 96);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(320, 88);
			this.groupBox2.TabIndex = 6;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Parallax Settings";
			// 
			// n_px
			// 
			this.n_px.DecimalPlaces = 4;
			this.n_px.Increment = new System.Decimal(new int[] {
																   25,
																   0,
																   0,
																   262144});
			this.n_px.Location = new System.Drawing.Point(136, 24);
			this.n_px.Name = "n_px";
			this.n_px.TabIndex = 2;
			this.n_px.Value = new System.Decimal(new int[] {
															   1,
															   0,
															   0,
															   0});
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(16, 56);
			this.label5.Name = "label5";
			this.label5.TabIndex = 1;
			this.label5.Text = "Y-Axis Multiplier";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(16, 24);
			this.label4.Name = "label4";
			this.label4.TabIndex = 0;
			this.label4.Text = "X-Axis Multiplier";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_py
			// 
			this.n_py.DecimalPlaces = 4;
			this.n_py.Increment = new System.Decimal(new int[] {
																   25,
																   0,
																   0,
																   262144});
			this.n_py.Location = new System.Drawing.Point(136, 56);
			this.n_py.Name = "n_py";
			this.n_py.TabIndex = 3;
			this.n_py.Value = new System.Decimal(new int[] {
															   1,
															   0,
															   0,
															   0});
			// 
			// linkLabel1
			// 
			this.linkLabel1.ActiveLinkColor = System.Drawing.Color.Blue;
			this.linkLabel1.Cursor = System.Windows.Forms.Cursors.Default;
			this.linkLabel1.Location = new System.Drawing.Point(312, 224);
			this.linkLabel1.Name = "linkLabel1";
			this.linkLabel1.Size = new System.Drawing.Size(32, 16);
			this.linkLabel1.TabIndex = 99;
			this.linkLabel1.TabStop = true;
			this.linkLabel1.Text = "Help";
			this.linkLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.linkLabel1.VisitedLinkColor = System.Drawing.Color.Blue;
			this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label3.Location = new System.Drawing.Point(192, 64);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(24, 24);
			this.label3.TabIndex = 4;
			this.label3.Text = "%";
			// 
			// n_layerlucency
			// 
			this.n_layerlucency.Location = new System.Drawing.Point(136, 64);
			this.n_layerlucency.Name = "n_layerlucency";
			this.n_layerlucency.Size = new System.Drawing.Size(56, 20);
			this.n_layerlucency.TabIndex = 1;
			// 
			// t_layername
			// 
			this.t_layername.Location = new System.Drawing.Point(136, 24);
			this.t_layername.Name = "t_layername";
			this.t_layername.Size = new System.Drawing.Size(192, 20);
			this.t_layername.TabIndex = 0;
			this.t_layername.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 64);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(100, 16);
			this.label2.TabIndex = 1;
			this.label2.Text = "Translucency:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(32, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(96, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Name:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// t_mh
			// 
			this.t_mh.Location = new System.Drawing.Point(192, 192);
			this.t_mh.Name = "t_mh";
			this.t_mh.ReadOnly = true;
			this.t_mh.Size = new System.Drawing.Size(64, 20);
			this.t_mh.TabIndex = 101;
			this.t_mh.Text = "";
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(200, 264);
			this.b_ok.Name = "b_ok";
			this.b_ok.TabIndex = 100;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(288, 264);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.TabIndex = 101;
			this.b_cancel.Text = "Cancel";
			// 
			// LayerPropertiesWnd
			// 
			this.AcceptButton = this.b_ok;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.b_cancel;
			this.ClientSize = new System.Drawing.Size(370, 295);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LayerPropertiesWnd";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Layer Properties";
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_px)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_py)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_layerlucency)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void b_resize_Click(object sender, System.EventArgs e)
		{
			LayerResizerWnd lrw = new LayerResizerWnd();
			lrw.init(mlayer);
			if(lrw.ShowDialog() != DialogResult.OK)
				return;
			// resize from here

			Size s = lrw.NewSize;

			if(Global.ActiveMap.Layers.IndexOf(mlayer)==0)
			{
				Global.ActiveMap.ObsLayer.resize(s.Width,s.Height);
				Global.ActiveMap.ZoneLayer.resize(s.Width,s.Height);
			}
			mlayer.resize(s.Width,s.Height);
			Global.MainMapController.MapView.CalculateScrollValues();
			updateui();

		}

		private void linkLabel1_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
		{
			Global.OpenHelp("index.html");
		}
	}
}
