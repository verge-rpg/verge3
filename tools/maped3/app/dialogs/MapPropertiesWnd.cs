using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for MapPropertiesWnd.
	/// </summary>
	public class MapPropertiesWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		public System.Windows.Forms.TextBox t_title;
		public System.Windows.Forms.TextBox t_music;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.Button b_browse;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.Label label3;
		public System.Windows.Forms.NumericUpDown n_px;
		public System.Windows.Forms.NumericUpDown n_py;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		public System.Windows.Forms.TextBox t_rstring;
		private System.Windows.Forms.Label label7;
		public System.Windows.Forms.TextBox t_aescript;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MapPropertiesWnd()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			if(Global.ActiveMap.FileOnDisk==null)
				b_browse.Enabled=false;
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
			this.t_aescript = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.t_rstring = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.n_py = new System.Windows.Forms.NumericUpDown();
			this.n_px = new System.Windows.Forms.NumericUpDown();
			this.label3 = new System.Windows.Forms.Label();
			this.b_browse = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.t_music = new System.Windows.Forms.TextBox();
			this.t_title = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_py)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_px)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.t_aescript);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.t_rstring);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.n_py);
			this.groupBox1.Controls.Add(this.n_px);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.b_browse);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.t_music);
			this.groupBox1.Controls.Add(this.t_title);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(424, 192);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			// 
			// t_aescript
			// 
			this.t_aescript.Location = new System.Drawing.Point(160, 152);
			this.t_aescript.Name = "t_aescript";
			this.t_aescript.Size = new System.Drawing.Size(152, 20);
			this.t_aescript.TabIndex = 12;
			this.t_aescript.Text = "";
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(40, 152);
			this.label7.Name = "label7";
			this.label7.TabIndex = 11;
			this.label7.Text = "Startup Script:";
			this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// t_rstring
			// 
			this.t_rstring.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.t_rstring.Location = new System.Drawing.Point(160, 120);
			this.t_rstring.Name = "t_rstring";
			this.t_rstring.ReadOnly = true;
			this.t_rstring.Size = new System.Drawing.Size(152, 22);
			this.t_rstring.TabIndex = 10;
			this.t_rstring.Text = "";
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(16, 120);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(128, 23);
			this.label6.TabIndex = 9;
			this.label6.Text = "Render String:";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(160, 96);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(16, 16);
			this.label4.TabIndex = 8;
			this.label4.Text = "X";
			// 
			// n_py
			// 
			this.n_py.Location = new System.Drawing.Point(272, 88);
			this.n_py.Maximum = new System.Decimal(new int[] {
																 65535,
																 0,
																 0,
																 0});
			this.n_py.Name = "n_py";
			this.n_py.Size = new System.Drawing.Size(72, 20);
			this.n_py.TabIndex = 7;
			// 
			// n_px
			// 
			this.n_px.Location = new System.Drawing.Point(176, 88);
			this.n_px.Maximum = new System.Decimal(new int[] {
																 65535,
																 0,
																 0,
																 0});
			this.n_px.Name = "n_px";
			this.n_px.Size = new System.Drawing.Size(72, 20);
			this.n_px.TabIndex = 6;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(32, 88);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(112, 23);
			this.label3.TabIndex = 5;
			this.label3.Text = "Player Start Position:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// b_browse
			// 
			this.b_browse.Location = new System.Drawing.Point(336, 56);
			this.b_browse.Name = "b_browse";
			this.b_browse.TabIndex = 4;
			this.b_browse.Text = "Browse..";
			this.b_browse.Click += new System.EventHandler(this.b_browse_Click);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(32, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(112, 23);
			this.label2.TabIndex = 3;
			this.label2.Text = "Music File:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(32, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(112, 16);
			this.label1.TabIndex = 2;
			this.label1.Text = "Map Title:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// t_music
			// 
			this.t_music.Location = new System.Drawing.Point(160, 56);
			this.t_music.Name = "t_music";
			this.t_music.Size = new System.Drawing.Size(168, 20);
			this.t_music.TabIndex = 1;
			this.t_music.Text = "";
			// 
			// t_title
			// 
			this.t_title.Location = new System.Drawing.Point(160, 24);
			this.t_title.Name = "t_title";
			this.t_title.Size = new System.Drawing.Size(248, 20);
			this.t_title.TabIndex = 0;
			this.t_title.Text = "";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(256, 96);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(16, 16);
			this.label5.TabIndex = 8;
			this.label5.Text = "Y";
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(264, 224);
			this.b_ok.Name = "b_ok";
			this.b_ok.TabIndex = 1;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(352, 224);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// openFileDialog
			// 
			this.openFileDialog.Title = "Music File...";
			// 
			// MapPropertiesWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(442, 255);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MapPropertiesWnd";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Map Properties";
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_py)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_px)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void b_browse_Click(object sender, System.EventArgs e)
		{
			openFileDialog.Filter = "Music Files (*.s3m,*.xm,*.it,*.mod,*.mp3)|*.s3m;*.xm;*.it;*.mod;*.mp3|All Files (*.*)|*.*";
			DialogResult dr = openFileDialog.ShowDialog();
			if(dr==DialogResult.Cancel)return;
			string root = Global.ActiveMap.FileOnDisk.Directory.FullName;
			string path = Helper.GetRelativePath(root,openFileDialog.FileName);
			if(path==null) 
			{
				Errors.Error("Oops", "You selected a music file that wasn't in the same directory tree as your map. " + root + " : " + openFileDialog.FileName);
				return;
			}
			t_music.Text = path;
		}
	}
}
