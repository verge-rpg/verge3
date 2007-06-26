using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for ColorDialog.
	/// </summary>
	public class ColorDialog : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private ColorDlgSlider cdSlider;
		private ColorDlgPicker cdPicker;
		private ColorInfo colorInfo;
		private ColorDisplaySolid colorDisplay;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label9;
		private ColorTextbox t_hval;
		private ColorTextbox t_sval;
		private ColorTextbox t_lval;
		private ColorTextbox t_rval;
		private ColorTextbox t_gval;
		private ColorTextbox t_bval;
		private Color selected_color;
		public Color SelectedColor 
		{ 
			get 
			{ 
				return selected_color; 
			} 
			set 
			{ 
				selected_color=value;
				HSBColor hsbc = PaintFunctions.ColorToHsb(value);
				cdSlider.SelectedHue = (int)hsbc.H * 256 /360;
				cdPicker.Saturation = hsbc.S;
				cdPicker.Brightness = hsbc.B;
			}
		}
		public ColorDialog() : this(Color.Black){}
		public ColorDialog(Color c)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();


			

			cdSlider.ColorChanged += new winmaped2.ColorDlgSlider.CEvent(cdSlider_ColorChanged);
			cdPicker.ColorChanged += new winmaped2.ColorDlgPicker.CEvent(cdPicker_ColorChanged);
			colorInfo = new ColorInfo();
//			colorInfo.ColorChanged += new winmaped2.ColorInfo.CEvent(colorInfo_ColorChanged);
			colorDisplay.SetController(colorInfo);
			cdPicker.SetDisplayTarget(colorDisplay);
			cdPicker.SetDlg(this);
			SelectedColor = c;
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
			this.cdSlider = new winmaped2.ColorDlgSlider();
			this.cdPicker = new winmaped2.ColorDlgPicker();
			this.colorDisplay = new winmaped2.ColorDisplaySolid();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.t_hval = new winmaped2.ColorTextbox();
			this.t_sval = new winmaped2.ColorTextbox();
			this.t_lval = new winmaped2.ColorTextbox();
			this.t_rval = new winmaped2.ColorTextbox();
			this.t_gval = new winmaped2.ColorTextbox();
			this.t_bval = new winmaped2.ColorTextbox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// cdSlider
			// 
			this.cdSlider.Location = new System.Drawing.Point(296, 24);
			this.cdSlider.Name = "cdSlider";
			this.cdSlider.SelectedHue = 0;
			this.cdSlider.Size = new System.Drawing.Size(24, 260);
			this.cdSlider.TabIndex = 0;
			// 
			// cdPicker
			// 
			this.cdPicker.BaseHue = 0;
			this.cdPicker.Brightness = 0F;
			this.cdPicker.Cursor = System.Windows.Forms.Cursors.Cross;
			this.cdPicker.Location = new System.Drawing.Point(16, 24);
			this.cdPicker.Name = "cdPicker";
			this.cdPicker.Saturation = 0F;
			this.cdPicker.Size = new System.Drawing.Size(260, 260);
			this.cdPicker.TabIndex = 1;
			// 
			// colorDisplay
			// 
			this.colorDisplay.Location = new System.Drawing.Point(336, 24);
			this.colorDisplay.Name = "colorDisplay";
			this.colorDisplay.Size = new System.Drawing.Size(72, 80);
			this.colorDisplay.TabIndex = 2;
			// 
			// button1
			// 
			this.button1.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.button1.Location = new System.Drawing.Point(424, 24);
			this.button1.Name = "button1";
			this.button1.TabIndex = 3;
			this.button1.Text = "OK";
			// 
			// button2
			// 
			this.button2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.button2.Location = new System.Drawing.Point(424, 56);
			this.button2.Name = "button2";
			this.button2.TabIndex = 4;
			this.button2.Text = "Cancel";
			// 
			// t_hval
			// 
			this.t_hval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_hval.Location = new System.Drawing.Point(352, 112);
			this.t_hval.Name = "t_hval";
			this.t_hval.Size = new System.Drawing.Size(40, 23);
			this.t_hval.TabIndex = 5;
			this.t_hval.Text = "0";
			// 
			// t_sval
			// 
			this.t_sval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_sval.Location = new System.Drawing.Point(352, 136);
			this.t_sval.Name = "t_sval";
			this.t_sval.Size = new System.Drawing.Size(40, 23);
			this.t_sval.TabIndex = 5;
			this.t_sval.Text = "0";
			// 
			// t_lval
			// 
			this.t_lval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_lval.Location = new System.Drawing.Point(352, 160);
			this.t_lval.Name = "t_lval";
			this.t_lval.Size = new System.Drawing.Size(40, 23);
			this.t_lval.TabIndex = 5;
			this.t_lval.Text = "0";
			// 
			// t_rval
			// 
			this.t_rval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_rval.Location = new System.Drawing.Point(352, 200);
			this.t_rval.Name = "t_rval";
			this.t_rval.Size = new System.Drawing.Size(40, 23);
			this.t_rval.TabIndex = 5;
			this.t_rval.Text = "0";
			// 
			// t_gval
			// 
			this.t_gval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_gval.Location = new System.Drawing.Point(352, 224);
			this.t_gval.Name = "t_gval";
			this.t_gval.Size = new System.Drawing.Size(40, 23);
			this.t_gval.TabIndex = 5;
			this.t_gval.Text = "0";
			// 
			// t_bval
			// 
			this.t_bval.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.t_bval.Location = new System.Drawing.Point(352, 248);
			this.t_bval.Name = "t_bval";
			this.t_bval.Size = new System.Drawing.Size(40, 23);
			this.t_bval.TabIndex = 5;
			this.t_bval.Text = "0";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.Location = new System.Drawing.Point(328, 112);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(24, 16);
			this.label1.TabIndex = 6;
			this.label1.Text = "H";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label2.Location = new System.Drawing.Point(328, 136);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(24, 16);
			this.label2.TabIndex = 6;
			this.label2.Text = "S";
			this.label2.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label3.Location = new System.Drawing.Point(328, 160);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(24, 16);
			this.label3.TabIndex = 6;
			this.label3.Text = "L";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label4.Location = new System.Drawing.Point(328, 200);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(24, 16);
			this.label4.TabIndex = 6;
			this.label4.Text = "R";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// label5
			// 
			this.label5.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label5.Location = new System.Drawing.Point(328, 224);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(24, 16);
			this.label5.TabIndex = 6;
			this.label5.Text = "G";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// label6
			// 
			this.label6.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label6.Location = new System.Drawing.Point(328, 248);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(24, 16);
			this.label6.TabIndex = 6;
			this.label6.Text = "B";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// label7
			// 
			this.label7.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label7.Location = new System.Drawing.Point(400, 112);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(24, 16);
			this.label7.TabIndex = 6;
			this.label7.Text = "°";
			// 
			// label8
			// 
			this.label8.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label8.Location = new System.Drawing.Point(400, 144);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(24, 16);
			this.label8.TabIndex = 6;
			this.label8.Text = "%";
			// 
			// label9
			// 
			this.label9.Font = new System.Drawing.Font("Tahoma", 9.75F);
			this.label9.Location = new System.Drawing.Point(400, 168);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(24, 16);
			this.label9.TabIndex = 6;
			this.label9.Text = "%";
			this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// ColorDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(506, 295);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.t_hval);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.cdSlider);
			this.Controls.Add(this.cdPicker);
			this.Controls.Add(this.colorDisplay);
			this.Controls.Add(this.t_sval);
			this.Controls.Add(this.t_lval);
			this.Controls.Add(this.t_rval);
			this.Controls.Add(this.t_gval);
			this.Controls.Add(this.t_bval);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label9);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ColorDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Pick color";
			this.ResumeLayout(false);

		}
		#endregion

		private void cdSlider_ColorChanged()
		{
			cdPicker.BaseHue = cdSlider.SelectedHue;
		}

		public void update()
		{
			t_hval.Text = cdSlider.SelectedHue.ToString();
			t_sval.Text = ((int)(cdPicker.Saturation*100)).ToString();
			t_lval.Text = ((int)(cdPicker.Brightness*100)).ToString();

			t_rval.Text = selected_color.R.ToString();
			t_gval.Text = selected_color.G.ToString();
			t_bval.Text = selected_color.B.ToString();

			t_hval.update();
			t_sval.update();
			t_lval.update();
		}

		private void cdPicker_ColorChanged()
		{
			Color c = PaintFunctions.HsbToColor(cdSlider.SelectedHue, cdPicker.Saturation, cdPicker.Brightness);
			selected_color=c;
			colorInfo.GrabFromColor(c);
			update();

		}

		private void colorInfo_ColorChanged()
		{
		}
	}
}
