using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for GetVspNameWnd.
	/// </summary>
	public class GetVspNameWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button b_OK;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox t_name;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public string VspName { get { return t_name.Text; } set { t_name.Text = value; } }
		public GetVspNameWnd()
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
			this.label1 = new System.Windows.Forms.Label();
			this.b_OK = new System.Windows.Forms.Button();
			this.t_name = new System.Windows.Forms.TextBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(40, 23);
			this.label1.TabIndex = 0;
			this.label1.Text = "Name";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// b_OK
			// 
			this.b_OK.Location = new System.Drawing.Point(112, 72);
			this.b_OK.Name = "b_OK";
			this.b_OK.TabIndex = 1;
			this.b_OK.Text = "OK";
			this.b_OK.Click += new System.EventHandler(this.b_OK_Click);
			// 
			// t_name
			// 
			this.t_name.Location = new System.Drawing.Point(80, 24);
			this.t_name.Name = "t_name";
			this.t_name.Size = new System.Drawing.Size(184, 20);
			this.t_name.TabIndex = 2;
			this.t_name.Text = "";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.t_name);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(280, 56);
			this.groupBox1.TabIndex = 3;
			this.groupBox1.TabStop = false;
			// 
			// GetVspNameWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 101);
			this.ControlBox = false;
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.b_OK);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Name = "GetVspNameWnd";
			this.Text = "Enter Name for VSP";
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void b_OK_Click(object sender, System.EventArgs e)
		{
			string c = VspName.Trim();
			if(c.Length==0)
			{
				Errors.Error("HEY", "Enter a name.");
				return;
			}
			if(c.Length<4||c.Substring( c.Length-4 ).ToLower() != ".vsp")
				c += ".vsp";
			VspName=c;
			Close();
		}

	
	}
}
