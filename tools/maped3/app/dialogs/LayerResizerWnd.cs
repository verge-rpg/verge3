using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for LayerResizerWnd.
	/// </summary>
	public class LayerResizerWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.NumericUpDown n_w;
		private System.Windows.Forms.NumericUpDown n_h;
		private System.Windows.Forms.Label l_dims;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Size NewSize { get { return new Size((int)n_w.Value,(int)n_h.Value); } }

		public LayerResizerWnd()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

		}

		public void init(MapLayer ml)
		{
			n_w.Value = ml.Width;
			n_h.Value = ml.Height;
			l_dims.Text = "Original Size - " + ml.Width.ToString() + " x " + ml.Height.ToString();
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
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.n_w = new System.Windows.Forms.NumericUpDown();
			this.n_h = new System.Windows.Forms.NumericUpDown();
			this.l_dims = new System.Windows.Forms.Label();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_w)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_h)).BeginInit();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 40);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(88, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Width:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 72);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(88, 16);
			this.label2.TabIndex = 0;
			this.label2.Text = "Height:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.l_dims);
			this.groupBox1.Controls.Add(this.n_w);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.n_h);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(320, 104);
			this.groupBox1.TabIndex = 1;
			this.groupBox1.TabStop = false;
			this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(160, 120);
			this.b_ok.Name = "b_ok";
			this.b_ok.TabIndex = 2;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(248, 120);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// n_w
			// 
			this.n_w.Location = new System.Drawing.Point(120, 40);
			this.n_w.Maximum = new System.Decimal(new int[] {
																65000,
																0,
																0,
																0});
			this.n_w.Name = "n_w";
			this.n_w.TabIndex = 1;
			// 
			// n_h
			// 
			this.n_h.Location = new System.Drawing.Point(120, 72);
			this.n_h.Maximum = new System.Decimal(new int[] {
																65000,
																0,
																0,
																0});
			this.n_h.Name = "n_h";
			this.n_h.TabIndex = 1;
			// 
			// l_dims
			// 
			this.l_dims.Location = new System.Drawing.Point(24, 16);
			this.l_dims.Name = "l_dims";
			this.l_dims.Size = new System.Drawing.Size(272, 23);
			this.l_dims.TabIndex = 2;
			this.l_dims.Text = "Original Size - 0 x 0";
			this.l_dims.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// LayerResizerWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(338, 151);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.b_cancel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LayerResizerWnd";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Layer Resize";
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_w)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_h)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void groupBox1_Enter(object sender, System.EventArgs e)
		{
		
		}
	}
}
