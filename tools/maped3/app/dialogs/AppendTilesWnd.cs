using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for ApendTilesWnd.
	/// </summary>
	public class AppendTilesWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.NumericUpDown n_tiles;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public int NumTiles { get { return (int)n_tiles.Value; } }
		public AppendTilesWnd()
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
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label1 = new System.Windows.Forms.Label();
			this.n_tiles = new System.Windows.Forms.NumericUpDown();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_tiles)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.n_tiles);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(288, 48);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(120, 23);
			this.label1.TabIndex = 0;
			this.label1.Text = "Number of Blank Tiles";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_tiles
			// 
			this.n_tiles.Location = new System.Drawing.Point(160, 16);
			this.n_tiles.Maximum = new System.Decimal(new int[] {
																	10000,
																	0,
																	0,
																	0});
			this.n_tiles.Name = "n_tiles";
			this.n_tiles.Size = new System.Drawing.Size(88, 20);
			this.n_tiles.TabIndex = 1;
			this.n_tiles.Value = new System.Decimal(new int[] {
																  100,
																  0,
																  0,
																  0});
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(128, 64);
			this.b_ok.Name = "b_ok";
			this.b_ok.TabIndex = 1;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(216, 64);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// AppendTilesWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(304, 93);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AppendTilesWnd";
			this.Text = "Select number of tiles";
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_tiles)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion
	}
}
