using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for AddLayer.
	/// </summary>
	public class NewLayerWindow : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button bnCancel;
		private System.Windows.Forms.Button bnOK;
		private System.Windows.Forms.NumericUpDown numWidth;
		private System.Windows.Forms.NumericUpDown numHeight;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public NewLayerWindow()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}
		public void init(int w, int h)
		{
			numWidth.Value = w;
			numHeight.Value = h;
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(NewLayerWindow));
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.numHeight = new System.Windows.Forms.NumericUpDown();
			this.numWidth = new System.Windows.Forms.NumericUpDown();
			this.bnCancel = new System.Windows.Forms.Button();
			this.bnOK = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numHeight)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numWidth)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.numHeight);
			this.groupBox1.Controls.Add(this.numWidth);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(288, 80);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Properties";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(52, 16);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(56, 16);
			this.label2.TabIndex = 6;
			this.label2.Text = "Width";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(52, 48);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(56, 16);
			this.label1.TabIndex = 5;
			this.label1.Text = "Height";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// numHeight
			// 
			this.numHeight.Location = new System.Drawing.Point(116, 48);
			this.numHeight.Maximum = new System.Decimal(new int[] {
																	  65000,
																	  0,
																	  0,
																	  0});
			this.numHeight.Name = "numHeight";
			this.numHeight.Size = new System.Drawing.Size(76, 20);
			this.numHeight.TabIndex = 4;
			this.numHeight.Value = new System.Decimal(new int[] {
																	50,
																	0,
																	0,
																	0});
			// 
			// numWidth
			// 
			this.numWidth.Location = new System.Drawing.Point(116, 16);
			this.numWidth.Maximum = new System.Decimal(new int[] {
																	 65000,
																	 0,
																	 0,
																	 0});
			this.numWidth.Name = "numWidth";
			this.numWidth.Size = new System.Drawing.Size(76, 20);
			this.numWidth.TabIndex = 3;
			this.numWidth.Value = new System.Decimal(new int[] {
																   50,
																   0,
																   0,
																   0});
			// 
			// bnCancel
			// 
			this.bnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.bnCancel.Location = new System.Drawing.Point(216, 96);
			this.bnCancel.Name = "bnCancel";
			this.bnCancel.TabIndex = 1;
			this.bnCancel.Text = "Cancel";
			// 
			// bnOK
			// 
			this.bnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.bnOK.Location = new System.Drawing.Point(136, 96);
			this.bnOK.Name = "bnOK";
			this.bnOK.TabIndex = 2;
			this.bnOK.Text = "OK";
			// 
			// NewLayerWindow
			// 
			this.AcceptButton = this.bnOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.bnCancel;
			this.ClientSize = new System.Drawing.Size(304, 127);
			this.Controls.Add(this.bnOK);
			this.Controls.Add(this.bnCancel);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "NewLayerWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "New Layer...";
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.numHeight)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numWidth)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		public Size SelectedSize
		{
			get
			{
				return new Size((int)numWidth.Value, (int)numHeight.Value);
			}
		}

	}
}
