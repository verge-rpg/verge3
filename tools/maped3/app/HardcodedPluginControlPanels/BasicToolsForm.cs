using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2.map_plugins
{

	/// <summary>
	/// Summary description for BasicToolsForm.
	/// </summary>
	public class BasicToolsForm : System.Windows.Forms.Form
	{
		public System.Windows.Forms.CheckBox lucent;
		private System.Windows.Forms.Panel panel1;
		public System.Windows.Forms.CheckBox overwrite;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public BasicToolsForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			TopLevel = false;
			Visible = true;

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
			this.lucent = new System.Windows.Forms.CheckBox();
			this.panel1 = new System.Windows.Forms.Panel();
			this.overwrite = new System.Windows.Forms.CheckBox();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lucent
			// 
			this.lucent.Location = new System.Drawing.Point(8, 8);
			this.lucent.Name = "lucent";
			this.lucent.Size = new System.Drawing.Size(56, 24);
			this.lucent.TabIndex = 1;
			this.lucent.Text = "lucent";
			this.lucent.CheckedChanged += new System.EventHandler(this.lucent_CheckedChanged);
			// 
			// panel1
			// 
			this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel1.Controls.Add(this.overwrite);
			this.panel1.Controls.Add(this.lucent);
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(80, 64);
			this.panel1.TabIndex = 2;
			// 
			// overwrite
			// 
			this.overwrite.Location = new System.Drawing.Point(8, 32);
			this.overwrite.Name = "overwrite";
			this.overwrite.Size = new System.Drawing.Size(72, 24);
			this.overwrite.TabIndex = 2;
			this.overwrite.Text = "overwrite";
			// 
			// BasicToolsForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(80, 64);
			this.ControlBox = false;
			this.Controls.Add(this.panel1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "BasicToolsForm";
			this.ShowInTaskbar = false;
			this.Text = "BasicToolsForm";
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void lucent_CheckedChanged(object sender, System.EventArgs e)
		{
	
		}
	}

}