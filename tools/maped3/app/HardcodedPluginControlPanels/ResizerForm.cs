using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2.map_plugins
{

	/// <summary>
	/// Summary description for ResizerForm.
	/// </summary>
	public class ResizerForm : System.Windows.Forms.Form
	{
		public System.Windows.Forms.Button resize;
		public System.Windows.Forms.TextBox heightSetting;
		public System.Windows.Forms.TextBox widthSetting;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ResizerForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
			this.resize = new System.Windows.Forms.Button();
			this.heightSetting = new System.Windows.Forms.TextBox();
			this.widthSetting = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// resize
			// 
			this.resize.Location = new System.Drawing.Point(8, 56);
			this.resize.Name = "resize";
			this.resize.Size = new System.Drawing.Size(48, 23);
			this.resize.TabIndex = 9;
			this.resize.Text = "Resize";
			// 
			// heightSetting
			// 
			this.heightSetting.Location = new System.Drawing.Point(8, 32);
			this.heightSetting.Name = "heightSetting";
			this.heightSetting.Size = new System.Drawing.Size(48, 20);
			this.heightSetting.TabIndex = 8;
			this.heightSetting.Text = "";
			// 
			// widthSetting
			// 
			this.widthSetting.Location = new System.Drawing.Point(8, 8);
			this.widthSetting.Name = "widthSetting";
			this.widthSetting.Size = new System.Drawing.Size(48, 20);
			this.widthSetting.TabIndex = 7;
			this.widthSetting.Text = "";
			// 
			// ResizerForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(64, 85);
			this.ControlBox = false;
			this.Controls.Add(this.resize);
			this.Controls.Add(this.heightSetting);
			this.Controls.Add(this.widthSetting);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.TopLevel = false;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ResizerForm";
			this.ShowInTaskbar = false;
			this.Text = "ResizerForm";
			this.ResumeLayout(false);

		}
		#endregion
	}

}