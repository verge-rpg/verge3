using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for MapedPreferences.
	/// </summary>
	public class MapedPreferences : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.ComboBox cb_zoomlevel;
		private System.Windows.Forms.NumericUpDown n_asint;
		private System.Windows.Forms.NumericUpDown n_asbackups;
		private System.Windows.Forms.Panel p_obs;
		private System.Windows.Forms.Panel p_zones;
		private System.Windows.Forms.Panel p_ents;
		private System.Windows.Forms.CheckBox cb_autosave;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Panel p_autosaveoptions;

		string[] zoomlevels = new string[] { "1x","2x","3x","4x" };

		public MapedPreferences()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			cb_zoomlevel.Text = zoomlevels[UserPrefs.DefaultZoomLevel-1];
			cb_autosave.Checked = UserPrefs.AutoSave;
			n_asint.Value = UserPrefs.AutoSaveDelay;
			n_asbackups.Value = UserPrefs.AutoSaveLimit;
			p_obs.BackColor = Color.FromArgb(UserPrefs.ObsColor);
			p_zones.BackColor = Color.FromArgb(UserPrefs.ZonesColor);
			p_ents.BackColor = Color.FromArgb(UserPrefs.EntsColor);


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
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.cb_zoomlevel = new System.Windows.Forms.ComboBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.cb_autosave = new System.Windows.Forms.CheckBox();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.n_asint = new System.Windows.Forms.NumericUpDown();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.n_asbackups = new System.Windows.Forms.NumericUpDown();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.p_obs = new System.Windows.Forms.Panel();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.p_zones = new System.Windows.Forms.Panel();
			this.p_ents = new System.Windows.Forms.Panel();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.p_autosaveoptions = new System.Windows.Forms.Panel();
			this.groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_asint)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_asbackups)).BeginInit();
			this.tabControl1.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.p_autosaveoptions.SuspendLayout();
			this.SuspendLayout();
			// 
			// b_ok
			// 
			this.b_ok.Location = new System.Drawing.Point(216, 192);
			this.b_ok.Name = "b_ok";
			this.b_ok.Size = new System.Drawing.Size(88, 23);
			this.b_ok.TabIndex = 1;
			this.b_ok.Text = "OK";
			this.b_ok.Click += new System.EventHandler(this.b_ok_Click);
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(312, 192);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.Size = new System.Drawing.Size(88, 24);
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(144, 23);
			this.label1.TabIndex = 0;
			this.label1.Text = "Default Zoom Level";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cb_zoomlevel
			// 
			this.cb_zoomlevel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cb_zoomlevel.Items.AddRange(new object[] {
															  "1x",
															  "2x",
															  "3x",
															  "4x"});
			this.cb_zoomlevel.Location = new System.Drawing.Point(168, 8);
			this.cb_zoomlevel.Name = "cb_zoomlevel";
			this.cb_zoomlevel.Size = new System.Drawing.Size(121, 21);
			this.cb_zoomlevel.TabIndex = 1;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.cb_autosave);
			this.groupBox2.Controls.Add(this.p_autosaveoptions);
			this.groupBox2.Location = new System.Drawing.Point(8, 8);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(360, 100);
			this.groupBox2.TabIndex = 2;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Auto-Backup Options";
			// 
			// cb_autosave
			// 
			this.cb_autosave.Location = new System.Drawing.Point(120, 16);
			this.cb_autosave.Name = "cb_autosave";
			this.cb_autosave.Size = new System.Drawing.Size(144, 16);
			this.cb_autosave.TabIndex = 4;
			this.cb_autosave.Text = "Auto-Backup Enabled";
			this.cb_autosave.CheckedChanged += new System.EventHandler(this.cb_autosave_CheckedChanged);
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(216, 32);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(24, 16);
			this.label5.TabIndex = 3;
			this.label5.Text = "files";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(216, 8);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(48, 16);
			this.label4.TabIndex = 2;
			this.label4.Text = "minutes";
			// 
			// n_asint
			// 
			this.n_asint.Location = new System.Drawing.Point(160, 8);
			this.n_asint.Name = "n_asint";
			this.n_asint.Size = new System.Drawing.Size(56, 20);
			this.n_asint.TabIndex = 1;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 8);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(136, 16);
			this.label2.TabIndex = 0;
			this.label2.Text = "Backup Interval";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(32, 32);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(112, 16);
			this.label3.TabIndex = 0;
			this.label3.Text = "Maximum Backups";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_asbackups
			// 
			this.n_asbackups.Location = new System.Drawing.Point(160, 32);
			this.n_asbackups.Name = "n_asbackups";
			this.n_asbackups.Size = new System.Drawing.Size(56, 20);
			this.n_asbackups.TabIndex = 1;
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabPage1);
			this.tabControl1.Controls.Add(this.tabPage2);
			this.tabControl1.Location = new System.Drawing.Point(8, 8);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(392, 176);
			this.tabControl1.TabIndex = 3;
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.groupBox1);
			this.tabPage1.Controls.Add(this.label1);
			this.tabPage1.Controls.Add(this.cb_zoomlevel);
			this.tabPage1.Location = new System.Drawing.Point(4, 22);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Size = new System.Drawing.Size(384, 150);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "   Display   ";
			this.tabPage1.Click += new System.EventHandler(this.tabPage1_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.p_obs);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.p_zones);
			this.groupBox1.Controls.Add(this.p_ents);
			this.groupBox1.Location = new System.Drawing.Point(16, 40);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(352, 100);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Colors";
			// 
			// p_obs
			// 
			this.p_obs.BackColor = System.Drawing.Color.White;
			this.p_obs.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.p_obs.Location = new System.Drawing.Point(128, 16);
			this.p_obs.Name = "p_obs";
			this.p_obs.Size = new System.Drawing.Size(72, 16);
			this.p_obs.TabIndex = 1;
			this.p_obs.Click += new System.EventHandler(this.p_obs_Click);
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(16, 16);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(104, 16);
			this.label6.TabIndex = 0;
			this.label6.Text = "Obstructions:";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(16, 40);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(104, 16);
			this.label7.TabIndex = 0;
			this.label7.Text = "Zones:";
			this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label8
			// 
			this.label8.Location = new System.Drawing.Point(16, 64);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(104, 16);
			this.label8.TabIndex = 0;
			this.label8.Text = "Entities:";
			this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// p_zones
			// 
			this.p_zones.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(0)), ((System.Byte)(0)), ((System.Byte)(192)));
			this.p_zones.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.p_zones.Location = new System.Drawing.Point(128, 40);
			this.p_zones.Name = "p_zones";
			this.p_zones.Size = new System.Drawing.Size(72, 16);
			this.p_zones.TabIndex = 1;
			this.p_zones.Click += new System.EventHandler(this.p_zones_Click);
			// 
			// p_ents
			// 
			this.p_ents.BackColor = System.Drawing.Color.Yellow;
			this.p_ents.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.p_ents.Location = new System.Drawing.Point(128, 64);
			this.p_ents.Name = "p_ents";
			this.p_ents.Size = new System.Drawing.Size(72, 16);
			this.p_ents.TabIndex = 1;
			this.p_ents.Click += new System.EventHandler(this.p_ents_Click);
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.groupBox2);
			this.tabPage2.Location = new System.Drawing.Point(4, 22);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Size = new System.Drawing.Size(384, 150);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "   Auto-Backup   ";
			// 
			// p_autosaveoptions
			// 
			this.p_autosaveoptions.Controls.Add(this.n_asbackups);
			this.p_autosaveoptions.Controls.Add(this.label5);
			this.p_autosaveoptions.Controls.Add(this.label4);
			this.p_autosaveoptions.Controls.Add(this.n_asint);
			this.p_autosaveoptions.Controls.Add(this.label2);
			this.p_autosaveoptions.Controls.Add(this.label3);
			this.p_autosaveoptions.Location = new System.Drawing.Point(8, 32);
			this.p_autosaveoptions.Name = "p_autosaveoptions";
			this.p_autosaveoptions.Size = new System.Drawing.Size(336, 56);
			this.p_autosaveoptions.TabIndex = 5;
			// 
			// MapedPreferences
			// 
			this.AcceptButton = this.b_ok;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.b_cancel;
			this.ClientSize = new System.Drawing.Size(410, 229);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.tabControl1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MapedPreferences";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "MapedPreferences";
			this.groupBox2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_asint)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_asbackups)).EndInit();
			this.tabControl1.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.tabPage2.ResumeLayout(false);
			this.p_autosaveoptions.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void cb_autosave_CheckedChanged(object sender, System.EventArgs e)
		{
			if(cb_autosave.Checked)
				p_autosaveoptions.Enabled=true;
			else p_autosaveoptions.Enabled=false;
		}

		private void p_obs_Click(object sender, System.EventArgs e)
		{
			ColorDialog cd = new ColorDialog();
			cd.SelectedColor = p_obs.BackColor;
			cd.ShowDialog();
            p_obs.BackColor = cd.SelectedColor;	
		}

		private void tabPage1_Click(object sender, System.EventArgs e)
		{
					
		}

		private void p_zones_Click(object sender, System.EventArgs e)
		{
			ColorDialog cd = new ColorDialog();
			cd.SelectedColor = p_zones.BackColor;
			cd.ShowDialog();
			p_zones.BackColor = cd.SelectedColor;
		}

		private void p_ents_Click(object sender, System.EventArgs e)
		{
			ColorDialog cd = new ColorDialog();
			cd.SelectedColor = p_ents.BackColor;
			cd.ShowDialog();
			p_ents.BackColor = cd.SelectedColor;		
		}

		private void b_ok_Click(object sender, System.EventArgs e)
		{
			// save prefs

			UserPrefs.DefaultZoomLevel = cb_zoomlevel.SelectedIndex + 1;
			UserPrefs.ObsColor = p_obs.BackColor.ToArgb();
			UserPrefs.ZonesColor = p_zones.BackColor.ToArgb();
			UserPrefs.EntsColor = p_ents.BackColor.ToArgb();
			UserPrefs.AutoSave = cb_autosave.Checked;
			UserPrefs.AutoSaveDelay = (int)n_asint.Value;
			UserPrefs.AutoSaveLimit = (int)n_asbackups.Value;

			DialogResult = DialogResult.OK;
			Close();
		}
		
	}
}
