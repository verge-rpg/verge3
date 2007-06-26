using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for AnimationEditor.
	/// </summary>
	public class AnimationEditor : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel editpanel;
		private System.Windows.Forms.Panel panel3;
		private System.Windows.Forms.ColumnHeader chID;
		private System.Windows.Forms.ColumnHeader chName;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.NumericUpDown n_start;
		private System.Windows.Forms.NumericUpDown n_end;
		private System.Windows.Forms.NumericUpDown n_delay;
		private System.Windows.Forms.TextBox t_name;
		private System.Windows.Forms.ComboBox c_mode;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Button b_Cancel;
		private System.Windows.Forms.Button b_OK;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private Map currMap;
		private Vsp24 currVsp;
		private winmaped2.TileViewer tv_start;
		private winmaped2.TileViewer tv_end;
		private System.Windows.Forms.Button b_newanim;
		private System.Windows.Forms.Button b_delanim;
		private System.Windows.Forms.ListView lv_anims;
		private ArrayList anims = new ArrayList();
		private winmaped2.VSPController vspc;
		private winmaped2.TileViewer tv_preview;
		private System.Windows.Forms.Label fpslabel;
	
		public ArrayList Anims { get { return anims; } }
		private VspAnimation currAnim;
		public AnimationEditor()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			
			c_mode.SelectedIndex=0;

			vspc.VspView.SelectionChanged += new SEventHandler(VspView_SelectionChanged);
			tv_preview.bAnimate=true;

		}

		public void init(Map m)
		{
			currMap = m;
			currVsp = m.vsp;

			vspc.SetActiveVsp(currVsp);
			vspc.SetControllerMode(VSPController.ControllerMode.SelectorDual);

//			n_start.Maximum = m.vsp.tileCount-1;
//			n_end.Maximum = m.vsp.tileCount-1;

			foreach(VspAnimation va in currVsp.Animations)
			{
				anims.Add( va.Clone() );
			}
			foreach(VspAnimation va in anims)
			{
				ListViewItem lvi = new ListViewItem( new string[] { va.ID.ToString(), va.Name } );
				lvi.Tag = va;
				lv_anims.Items.Add(lvi);
			}
			DisableEditor();
		}

		private void DisableEditor()
		{
			editpanel.Enabled=false;
			b_delanim.Enabled=false;
		}
		private void EnableEditor()
		{
			editpanel.Enabled=true;
			//b_delanim.Enabled=true;
		}
		private void GrabData()
		{
            currAnim.Name = t_name.Text;
			currAnim.Start = (int)n_start.Value;
			currAnim.End = (int)n_end.Value;
			currAnim.Delay = (int)n_delay.Value;
			currAnim.Mode = c_mode.SelectedIndex;
		}
		private void PutData()
		{
			t_name.Text = currAnim.Name;
			n_start.Value = currAnim.Start;
			n_end.Value = currAnim.End;
			n_delay.Value = currAnim.Delay;
			c_mode.SelectedIndex = currAnim.Mode;
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AnimationEditor));
			this.panel1 = new System.Windows.Forms.Panel();
			this.b_Cancel = new System.Windows.Forms.Button();
			this.b_OK = new System.Windows.Forms.Button();
			this.editpanel = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.fpslabel = new System.Windows.Forms.Label();
			this.vspc = new winmaped2.VSPController();
			this.tv_start = new winmaped2.TileViewer();
			this.label1 = new System.Windows.Forms.Label();
			this.c_mode = new System.Windows.Forms.ComboBox();
			this.t_name = new System.Windows.Forms.TextBox();
			this.n_delay = new System.Windows.Forms.NumericUpDown();
			this.n_end = new System.Windows.Forms.NumericUpDown();
			this.n_start = new System.Windows.Forms.NumericUpDown();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.tv_end = new winmaped2.TileViewer();
			this.tv_preview = new winmaped2.TileViewer();
			this.panel3 = new System.Windows.Forms.Panel();
			this.lv_anims = new System.Windows.Forms.ListView();
			this.chID = new System.Windows.Forms.ColumnHeader();
			this.chName = new System.Windows.Forms.ColumnHeader();
			this.panel2 = new System.Windows.Forms.Panel();
			this.b_newanim = new System.Windows.Forms.Button();
			this.b_delanim = new System.Windows.Forms.Button();
			this.panel1.SuspendLayout();
			this.editpanel.SuspendLayout();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_delay)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_end)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_start)).BeginInit();
			this.panel3.SuspendLayout();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel1.Controls.Add(this.b_Cancel);
			this.panel1.Controls.Add(this.b_OK);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel1.Location = new System.Drawing.Point(0, 485);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(600, 48);
			this.panel1.TabIndex = 0;
			// 
			// b_Cancel
			// 
			this.b_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_Cancel.Location = new System.Drawing.Point(512, 12);
			this.b_Cancel.Name = "b_Cancel";
			this.b_Cancel.Size = new System.Drawing.Size(75, 23);
			this.b_Cancel.TabIndex = 3;
			this.b_Cancel.Text = "Cancel";
			// 
			// b_OK
			// 
			this.b_OK.Location = new System.Drawing.Point(424, 12);
			this.b_OK.Name = "b_OK";
			this.b_OK.Size = new System.Drawing.Size(75, 23);
			this.b_OK.TabIndex = 2;
			this.b_OK.Text = "OK";
			this.b_OK.Click += new System.EventHandler(this.b_OK_Click);
			// 
			// editpanel
			// 
			this.editpanel.Controls.Add(this.groupBox1);
			this.editpanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.editpanel.Location = new System.Drawing.Point(0, 0);
			this.editpanel.Name = "editpanel";
			this.editpanel.Size = new System.Drawing.Size(368, 485);
			this.editpanel.TabIndex = 1;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.fpslabel);
			this.groupBox1.Controls.Add(this.vspc);
			this.groupBox1.Controls.Add(this.tv_start);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.c_mode);
			this.groupBox1.Controls.Add(this.t_name);
			this.groupBox1.Controls.Add(this.n_delay);
			this.groupBox1.Controls.Add(this.n_end);
			this.groupBox1.Controls.Add(this.n_start);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.tv_end);
			this.groupBox1.Controls.Add(this.tv_preview);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(352, 464);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Animation";
			// 
			// fpslabel
			// 
			this.fpslabel.Location = new System.Drawing.Point(248, 352);
			this.fpslabel.Name = "fpslabel";
			this.fpslabel.Size = new System.Drawing.Size(96, 16);
			this.fpslabel.TabIndex = 11;
			this.fpslabel.Text = "0.0 frames/sec";
			// 
			// vspc
			// 
			this.vspc.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.vspc.Location = new System.Drawing.Point(6, 176);
			this.vspc.Name = "vspc";
			this.vspc.Size = new System.Drawing.Size(340, 164);
			this.vspc.TabIndex = 10;
			// 
			// tv_start
			// 
			this.tv_start.ActiveObsTile = null;
			this.tv_start.ActiveTile = null;
			this.tv_start.ActiveTileIndex = 0;
			this.tv_start.Location = new System.Drawing.Point(248, 56);
			this.tv_start.Name = "tv_start";
			this.tv_start.Size = new System.Drawing.Size(48, 48);
			this.tv_start.TabIndex = 7;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(100, 23);
			this.label1.TabIndex = 6;
			this.label1.Text = "Name:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// c_mode
			// 
			this.c_mode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_mode.Items.AddRange(new object[] {
            "Forward",
            "Reverse",
            "Random",
            "Ping Pong"});
			this.c_mode.Location = new System.Drawing.Point(120, 384);
			this.c_mode.Name = "c_mode";
			this.c_mode.Size = new System.Drawing.Size(120, 21);
			this.c_mode.TabIndex = 5;
			// 
			// t_name
			// 
			this.t_name.Location = new System.Drawing.Point(120, 24);
			this.t_name.Name = "t_name";
			this.t_name.Size = new System.Drawing.Size(216, 20);
			this.t_name.TabIndex = 4;
			this.t_name.TextChanged += new System.EventHandler(this.t_name_TextChanged);
			// 
			// n_delay
			// 
			this.n_delay.Location = new System.Drawing.Point(120, 352);
			this.n_delay.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
			this.n_delay.Name = "n_delay";
			this.n_delay.Size = new System.Drawing.Size(120, 20);
			this.n_delay.TabIndex = 3;
			this.n_delay.ValueChanged += new System.EventHandler(this.n_delay_ValueChanged);
			// 
			// n_end
			// 
			this.n_end.Location = new System.Drawing.Point(120, 120);
			this.n_end.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
			this.n_end.Name = "n_end";
			this.n_end.Size = new System.Drawing.Size(120, 20);
			this.n_end.TabIndex = 1;
			this.n_end.ValueChanged += new System.EventHandler(this.n_end_ValueChanged);
			// 
			// n_start
			// 
			this.n_start.Location = new System.Drawing.Point(120, 56);
			this.n_start.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
			this.n_start.Name = "n_start";
			this.n_start.Size = new System.Drawing.Size(120, 20);
			this.n_start.TabIndex = 0;
			this.n_start.ValueChanged += new System.EventHandler(this.n_start_ValueChanged);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(16, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(100, 23);
			this.label2.TabIndex = 6;
			this.label2.Text = "Start Tile:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(16, 120);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(100, 23);
			this.label3.TabIndex = 6;
			this.label3.Text = "End Tile:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(16, 352);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(100, 23);
			this.label4.TabIndex = 6;
			this.label4.Text = "Delay:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(16, 384);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(100, 23);
			this.label5.TabIndex = 6;
			this.label5.Text = "Animation Mode:";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tv_end
			// 
			this.tv_end.ActiveObsTile = null;
			this.tv_end.ActiveTile = null;
			this.tv_end.ActiveTileIndex = 0;
			this.tv_end.Location = new System.Drawing.Point(248, 120);
			this.tv_end.Name = "tv_end";
			this.tv_end.Size = new System.Drawing.Size(48, 48);
			this.tv_end.TabIndex = 7;
			// 
			// tv_preview
			// 
			this.tv_preview.ActiveObsTile = null;
			this.tv_preview.ActiveTile = null;
			this.tv_preview.ActiveTileIndex = 0;
			this.tv_preview.Location = new System.Drawing.Point(248, 384);
			this.tv_preview.Name = "tv_preview";
			this.tv_preview.Size = new System.Drawing.Size(64, 64);
			this.tv_preview.TabIndex = 7;
			// 
			// panel3
			// 
			this.panel3.Controls.Add(this.lv_anims);
			this.panel3.Controls.Add(this.panel2);
			this.panel3.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel3.Location = new System.Drawing.Point(368, 0);
			this.panel3.Name = "panel3";
			this.panel3.Size = new System.Drawing.Size(232, 485);
			this.panel3.TabIndex = 2;
			// 
			// lv_anims
			// 
			this.lv_anims.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chID,
            this.chName});
			this.lv_anims.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lv_anims.FullRowSelect = true;
			this.lv_anims.GridLines = true;
			this.lv_anims.HideSelection = false;
			this.lv_anims.Location = new System.Drawing.Point(0, 0);
			this.lv_anims.MultiSelect = false;
			this.lv_anims.Name = "lv_anims";
			this.lv_anims.Size = new System.Drawing.Size(232, 437);
			this.lv_anims.TabIndex = 0;
			this.lv_anims.UseCompatibleStateImageBehavior = false;
			this.lv_anims.View = System.Windows.Forms.View.Details;
			this.lv_anims.SelectedIndexChanged += new System.EventHandler(this.lv_anims_SelectedIndexChanged);
			// 
			// chID
			// 
			this.chID.Text = "ID";
			this.chID.Width = 40;
			// 
			// chName
			// 
			this.chName.Text = "Name";
			this.chName.Width = 149;
			// 
			// panel2
			// 
			this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel2.Controls.Add(this.b_newanim);
			this.panel2.Controls.Add(this.b_delanim);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel2.Location = new System.Drawing.Point(0, 437);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(232, 48);
			this.panel2.TabIndex = 1;
			// 
			// b_newanim
			// 
			this.b_newanim.Location = new System.Drawing.Point(8, 8);
			this.b_newanim.Name = "b_newanim";
			this.b_newanim.Size = new System.Drawing.Size(104, 32);
			this.b_newanim.TabIndex = 2;
			this.b_newanim.Text = "New Animation";
			this.b_newanim.Click += new System.EventHandler(this.b_newanim_Click);
			// 
			// b_delanim
			// 
			this.b_delanim.Enabled = false;
			this.b_delanim.Location = new System.Drawing.Point(120, 8);
			this.b_delanim.Name = "b_delanim";
			this.b_delanim.Size = new System.Drawing.Size(104, 32);
			this.b_delanim.TabIndex = 1;
			this.b_delanim.Text = "Delete Selected";
			// 
			// AnimationEditor
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(600, 533);
			this.Controls.Add(this.editpanel);
			this.Controls.Add(this.panel3);
			this.Controls.Add(this.panel1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(608, 1000000);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(608, 520);
			this.Name = "AnimationEditor";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Animations";
			this.panel1.ResumeLayout(false);
			this.editpanel.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_delay)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_end)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_start)).EndInit();
			this.panel3.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void lv_anims_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if(currAnim!=null) GrabData();
			if(lv_anims.SelectedItems.Count==0)
			{
				DisableEditor();
				return;
			}
			ListViewItem lvi = lv_anims.SelectedItems[0];
			currAnim = (VspAnimation)lvi.Tag;
			PutData();
			EnableEditor();
		}

		private void b_newanim_Click(object sender, System.EventArgs e)
		{
			VspAnimation va = new VspAnimation();
			va.ID = anims.Count;
			anims.Add(va);

			ListViewItem lvi = new ListViewItem( new string[] { va.ID.ToString(), va.Name } );
			lvi.Tag = va;
			lv_anims.Items.Add(lvi);

			lvi.Selected=true;
			lvi.EnsureVisible();
		}

		private void b_OK_Click(object sender, System.EventArgs e)
		{
			if(currAnim!=null) GrabData();
			DialogResult = DialogResult.OK;
			Close();
		}
		private void updatefps()
		{
			if(currAnim!=null)
			{
				if(n_delay.Value!=0)
				{
					double d = 100.0 / (double)n_delay.Value;
					fpslabel.Text = d.ToString(".###") + " frames/sec";
				}
				else fpslabel.Text = "unknown frames/sec";
			}
		}

		private void n_start_ValueChanged(object sender, System.EventArgs e)
		{
			if(n_start.Value != vspc.VspView.SelectedTileF)
				vspc.VspView.SelectedTileF = (int)n_start.Value;
			int t = (int)n_start.Value;
			if(t>=currVsp.tileCount)
				return;
			Vsp24Tile vt = (Vsp24Tile)currVsp.Tiles[t];
			tv_start.ActiveTile = vt;
			tv_preview.ActiveTileIndex =(int)n_start.Value;
		}

		private void n_end_ValueChanged(object sender, System.EventArgs e)
		{
			if(n_end.Value != vspc.VspView.SelectedTileB)
				vspc.VspView.SelectedTileB = (int)n_end.Value;
			int t = (int)n_end.Value;
			if(t>=currVsp.tileCount)
				return;
			Vsp24Tile vt = (Vsp24Tile)currVsp.Tiles[t];
			tv_end.ActiveTile = vt;
		}

		private void VspView_SelectionChanged()
		{
			n_end.Value = vspc.VspView.SelectedTileB;
			n_start.Value = vspc.VspView.SelectedTileF;
		}

		private void t_name_TextChanged(object sender, System.EventArgs e)
		{
			if(currAnim==null) return;
			foreach(ListViewItem lvi in lv_anims.Items)
			{
				if(lvi.Tag == currAnim)
					lvi.SubItems[1].Text = t_name.Text;
			}
		}

		private void n_delay_ValueChanged(object sender, System.EventArgs e)
		{
			updatefps();
		}
	}
}
