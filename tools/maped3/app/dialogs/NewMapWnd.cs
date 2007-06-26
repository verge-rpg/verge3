using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for NewMapWnd.
	/// </summary>
	public class NewMapWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.NumericUpDown n_w;
		private System.Windows.Forms.NumericUpDown n_h;
		private System.Windows.Forms.NumericUpDown n_layers;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.RadioButton rb_new;
		private System.Windows.Forms.RadioButton rb_existing;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox t_vspfile;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Panel g_vspfile;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public NewMapWnd()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		public Map CreateMap()
		{
			Map m = new Map();
			int l_count = (int)n_layers.Value;
			int l_w = (int)n_w.Value;
			int l_h = (int)n_h.Value;
			bool bNewvsp = rb_new.Checked;
			for(int i=0;i<l_count;i++)
			{
				MapLayer ml = new MapLayer(m);
				ml.size(l_w,l_h);
				ml.name = "Layer " + i;
				ml.type = LayerType.Tile;
				ml.parallaxInfo = new ParallaxInfo();
				m.Layers.Add(ml);
			}
			MapLayer zl = new MapLayer(m);
			zl.type = LayerType.Zone;
			zl.name = "Zones";
			zl.size(l_w,l_h);
			
			m.ZoneLayer=zl;

			MapLayer ol = new MapLayer(m);
			ol.type = LayerType.Obs;
			ol.name = "Obstructions";
			ol.size(l_w,l_h);
			m.ObsLayer = ol;
			


			MapLayer el = new MapLayer(m);
			el.type = LayerType.Entity;
			el.name = "Entities";
			MapLayerSpecial rl = new MapLayerSpecial(m);
			rl.type=LayerType.Special_Retrace;
	
			
			m.Layers.Add(rl);
			m.Layers.Add(el);
			m.Layers.Add(ol);
			m.Layers.Add(zl);

			m.EntLayer = el;
			if(bNewvsp)
			{
				Vsp24 v = new Vsp24();
            
				//v.AddTiles(100);
				v.Tiles.AddRange(v.GetTiles(100));
				m.vsp = v;
	
			}
			else
			{
				Vsp24 v = InputOutput.ReadVsp(t_vspfile.Text);
				if(v==null)
				{
					Errors.Error("Warning", "An error occured when attempting to load the vsp file.  A blank VSP has been created.");
					v = new Vsp24();
            
					v.Tiles.Add(v.GetTiles(100));
					m.vsp = v;
				}
				else
					m.vsp = v;
			}

			MapZone mz = new MapZone();
			mz.ID = 0;
			mz.Name = "NULL_ZONE";
			m.Zones.Add(mz);
			
			
			m.Init();
			m.RenderString = "";
			return m;
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
			this.g_vspfile = new System.Windows.Forms.Panel();
			this.label6 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.t_vspfile = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.rb_existing = new System.Windows.Forms.RadioButton();
			this.rb_new = new System.Windows.Forms.RadioButton();
			this.label4 = new System.Windows.Forms.Label();
			this.n_layers = new System.Windows.Forms.NumericUpDown();
			this.n_h = new System.Windows.Forms.NumericUpDown();
			this.n_w = new System.Windows.Forms.NumericUpDown();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.groupBox1.SuspendLayout();
			this.g_vspfile.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_layers)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_h)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_w)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.g_vspfile);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.rb_existing);
			this.groupBox1.Controls.Add(this.rb_new);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.n_layers);
			this.groupBox1.Controls.Add(this.n_h);
			this.groupBox1.Controls.Add(this.n_w);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(384, 192);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Properties";
			// 
			// g_vspfile
			// 
			this.g_vspfile.Controls.Add(this.label6);
			this.g_vspfile.Controls.Add(this.button1);
			this.g_vspfile.Controls.Add(this.t_vspfile);
			this.g_vspfile.Enabled = false;
			this.g_vspfile.Location = new System.Drawing.Point(8, 144);
			this.g_vspfile.Name = "g_vspfile";
			this.g_vspfile.Size = new System.Drawing.Size(368, 40);
			this.g_vspfile.TabIndex = 12;
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(16, 8);
			this.label6.Name = "label6";
			this.label6.TabIndex = 9;
			this.label6.Text = "VSP File:";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(280, 8);
			this.button1.Name = "button1";
			this.button1.TabIndex = 11;
			this.button1.Text = "Browse...";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// t_vspfile
			// 
			this.t_vspfile.Location = new System.Drawing.Point(128, 8);
			this.t_vspfile.Name = "t_vspfile";
			this.t_vspfile.Size = new System.Drawing.Size(144, 20);
			this.t_vspfile.TabIndex = 10;
			this.t_vspfile.Text = "";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(232, 48);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(16, 16);
			this.label5.TabIndex = 8;
			this.label5.Text = "X";
			// 
			// rb_existing
			// 
			this.rb_existing.Location = new System.Drawing.Point(192, 120);
			this.rb_existing.Name = "rb_existing";
			this.rb_existing.Size = new System.Drawing.Size(64, 24);
			this.rb_existing.TabIndex = 7;
			this.rb_existing.Text = "Existing";
			this.rb_existing.CheckedChanged += new System.EventHandler(this.rb_existing_CheckedChanged);
			// 
			// rb_new
			// 
			this.rb_new.Checked = true;
			this.rb_new.Location = new System.Drawing.Point(136, 120);
			this.rb_new.Name = "rb_new";
			this.rb_new.Size = new System.Drawing.Size(48, 24);
			this.rb_new.TabIndex = 6;
			this.rb_new.TabStop = true;
			this.rb_new.Text = "New";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(24, 120);
			this.label4.Name = "label4";
			this.label4.TabIndex = 5;
			this.label4.Text = "VSP:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_layers
			// 
			this.n_layers.Location = new System.Drawing.Point(136, 80);
			this.n_layers.Minimum = new System.Decimal(new int[] {
																	 1,
																	 0,
																	 0,
																	 0});
			this.n_layers.Name = "n_layers";
			this.n_layers.Size = new System.Drawing.Size(56, 20);
			this.n_layers.TabIndex = 4;
			this.n_layers.Value = new System.Decimal(new int[] {
																   2,
																   0,
																   0,
																   0});
			// 
			// n_h
			// 
			this.n_h.Location = new System.Drawing.Point(264, 40);
			this.n_h.Maximum = new System.Decimal(new int[] {
																65535,
																0,
																0,
																0});
			this.n_h.Name = "n_h";
			this.n_h.Size = new System.Drawing.Size(80, 20);
			this.n_h.TabIndex = 3;
			this.n_h.Value = new System.Decimal(new int[] {
															  100,
															  0,
															  0,
															  0});
			// 
			// n_w
			// 
			this.n_w.Location = new System.Drawing.Point(136, 40);
			this.n_w.Maximum = new System.Decimal(new int[] {
																65535,
																0,
																0,
																0});
			this.n_w.Name = "n_w";
			this.n_w.Size = new System.Drawing.Size(80, 20);
			this.n_w.TabIndex = 2;
			this.n_w.Value = new System.Decimal(new int[] {
															  100,
															  0,
															  0,
															  0});
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 80);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(104, 23);
			this.label2.TabIndex = 1;
			this.label2.Text = "Number of Layers:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 40);
			this.label1.Name = "label1";
			this.label1.TabIndex = 0;
			this.label1.Text = "Size:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(16, 16);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(304, 16);
			this.label3.TabIndex = 3;
			this.label3.Text = "Note: These parameters can all be changed later";
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(232, 208);
			this.b_ok.Name = "b_ok";
			this.b_ok.TabIndex = 1;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(312, 208);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// openFileDialog
			// 
			this.openFileDialog.Filter = "VSP Files (*.vsp)|*.vsp";
			this.openFileDialog.RestoreDirectory = true;
			// 
			// NewMapWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(402, 239);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "NewMapWnd";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "New Map";
			this.groupBox1.ResumeLayout(false);
			this.g_vspfile.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_layers)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_h)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_w)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void rb_existing_CheckedChanged(object sender, System.EventArgs e)
		{
			if(rb_existing.Checked)
				g_vspfile.Enabled=true;
			else g_vspfile.Enabled=false;
		}

		private void button1_Click(object sender, System.EventArgs e)
		{
			if(openFileDialog.ShowDialog()!=DialogResult.OK) return;
            t_vspfile.Text = openFileDialog.FileName;			
		}
	}
}
