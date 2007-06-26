using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for ImportDialog.
	/// </summary>
	public class ImportDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button b_ok;
		private System.Windows.Forms.Button b_cancel;
		private System.Windows.Forms.ComboBox c_dest;
		private System.Windows.Forms.ComboBox c_method;
		private System.Windows.Forms.ComboBox c_source;
		private System.Windows.Forms.CheckBox c_padding;
		private System.Windows.Forms.OpenFileDialog openVspDialog;
		private System.Windows.Forms.OpenFileDialog openImageDialog;
		private CheckBox cbAddLayer;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ImportDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			c_dest.SelectedIndex = 0;
			c_method.SelectedIndex = 0;
			c_source.SelectedIndex = 0;
		}
		public void init()
		{
			InsertAt = Global.VspViewer.SelectedTileF+1;
			c_method.Items[2] = "Insert After Selected Tile (Index: "+(InsertAt-1)+")";
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
			this.c_padding = new System.Windows.Forms.CheckBox();
			this.c_dest = new System.Windows.Forms.ComboBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.c_method = new System.Windows.Forms.ComboBox();
			this.c_source = new System.Windows.Forms.ComboBox();
			this.b_ok = new System.Windows.Forms.Button();
			this.b_cancel = new System.Windows.Forms.Button();
			this.openVspDialog = new System.Windows.Forms.OpenFileDialog();
			this.openImageDialog = new System.Windows.Forms.OpenFileDialog();
			this.cbAddLayer = new System.Windows.Forms.CheckBox();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.cbAddLayer);
			this.groupBox1.Controls.Add(this.c_padding);
			this.groupBox1.Controls.Add(this.c_dest);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.c_method);
			this.groupBox1.Controls.Add(this.c_source);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(408, 179);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Import Options";
			// 
			// c_padding
			// 
			this.c_padding.Location = new System.Drawing.Point(160, 112);
			this.c_padding.Name = "c_padding";
			this.c_padding.Size = new System.Drawing.Size(152, 24);
			this.c_padding.TabIndex = 4;
			this.c_padding.Text = "Source Padding";
			this.c_padding.CheckedChanged += new System.EventHandler(this.c_padding_CheckedChanged);
			// 
			// c_dest
			// 
			this.c_dest.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_dest.Items.AddRange(new object[] {
            "Tiles",
            "Obstructions"});
			this.c_dest.Location = new System.Drawing.Point(160, 24);
			this.c_dest.Name = "c_dest";
			this.c_dest.Size = new System.Drawing.Size(232, 21);
			this.c_dest.TabIndex = 3;
			this.c_dest.SelectedIndexChanged += new System.EventHandler(this.c_dest_SelectedIndexChanged);
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(8, 88);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(128, 16);
			this.label3.TabIndex = 2;
			this.label3.Text = "Data Source:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(16, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(120, 16);
			this.label2.TabIndex = 1;
			this.label2.Text = "Insertion Method:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(120, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Destination Tile Bank:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// c_method
			// 
			this.c_method.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_method.Items.AddRange(new object[] {
            "Append",
            "Replace",
            "Insert After Selected Tile"});
			this.c_method.Location = new System.Drawing.Point(160, 56);
			this.c_method.Name = "c_method";
			this.c_method.Size = new System.Drawing.Size(232, 21);
			this.c_method.TabIndex = 3;
			this.c_method.SelectedIndexChanged += new System.EventHandler(this.c_method_SelectedIndexChanged);
			// 
			// c_source
			// 
			this.c_source.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_source.Items.AddRange(new object[] {
            "VSP File",
            "Image File",
            "Clipboard"});
			this.c_source.Location = new System.Drawing.Point(160, 88);
			this.c_source.Name = "c_source";
			this.c_source.Size = new System.Drawing.Size(232, 21);
			this.c_source.TabIndex = 3;
			this.c_source.SelectedIndexChanged += new System.EventHandler(this.c_source_SelectedIndexChanged);
			// 
			// b_ok
			// 
			this.b_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.b_ok.Location = new System.Drawing.Point(253, 193);
			this.b_ok.Name = "b_ok";
			this.b_ok.Size = new System.Drawing.Size(75, 23);
			this.b_ok.TabIndex = 1;
			this.b_ok.Text = "OK";
			// 
			// b_cancel
			// 
			this.b_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_cancel.Location = new System.Drawing.Point(341, 193);
			this.b_cancel.Name = "b_cancel";
			this.b_cancel.Size = new System.Drawing.Size(75, 23);
			this.b_cancel.TabIndex = 2;
			this.b_cancel.Text = "Cancel";
			// 
			// openVspDialog
			// 
			this.openVspDialog.Filter = "VSP Files(*.vsp)|*.vsp";
			this.openVspDialog.RestoreDirectory = true;
			// 
			// openImageDialog
			// 
			this.openImageDialog.Filter = "Image Files (*.png,*.jpg,*.jpeg,*.pcx,*.bmp,*.tga,*.gif)|*.png;*.jpg;*.jpeg;*.pcx" +
				";*.bmp;*.tga;*.gif";
			this.openImageDialog.RestoreDirectory = true;
			// 
			// cbAddLayer
			// 
			this.cbAddLayer.AutoSize = true;
			this.cbAddLayer.Location = new System.Drawing.Point(160, 142);
			this.cbAddLayer.Name = "cbAddLayer";
			this.cbAddLayer.Size = new System.Drawing.Size(144, 17);
			this.cbAddLayer.TabIndex = 5;
			this.cbAddLayer.Text = "Create Layer From Image";
			this.cbAddLayer.UseVisualStyleBackColor = true;
			// 
			// ImportDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(426, 228);
			this.Controls.Add(this.b_cancel);
			this.Controls.Add(this.b_ok);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ImportDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "ImportDialog";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

		}
		#endregion



		public ImportDest IDest;
		public ImportSource ISource;
		public ImportMethod IMethod;
		public bool		bPadding;
		public int		InsertAt;

		public bool bAddLayer { get { return cbAddLayer.Checked && cbAddLayer.Enabled; } }

		private void update()
		{
			cbAddLayer.Enabled = (IDest == ImportDest.Tiles && (ISource == ImportSource.Clipboard || ISource == ImportSource.Image));
		}

		private void c_dest_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			IDest = (ImportDest)c_dest.SelectedIndex;
			update();
		}

		private void c_method_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			IMethod = (ImportMethod)c_method.SelectedIndex;
			update();
		}

		private void c_source_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			ISource = (ImportSource)c_source.SelectedIndex;
			update();
		}
		private void c_padding_CheckedChanged(object sender, System.EventArgs e)
		{
			bPadding = c_padding.Checked;
			update();
		}
	}
	public enum ImportDest { Tiles, Obs }
	public enum ImportMethod { Append, Replace, Insert }
	public enum ImportSource { VSP, Image, Clipboard }
}
