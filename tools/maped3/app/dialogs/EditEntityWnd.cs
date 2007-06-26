using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{
	/// <summary>
	/// Summary description for EntityEditWnd.
	/// </summary>
	public class EntityEditWnd : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Panel panel3;
		private System.Windows.Forms.Panel panel4;
		private System.Windows.Forms.ListView lv_ents;
		private System.Windows.Forms.ColumnHeader ch_entID;
		private System.Windows.Forms.ColumnHeader ch_entName;
		private System.Windows.Forms.Button b_OK;
		private System.Windows.Forms.Button b_Cancel;
		private System.Windows.Forms.Button button1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.TextBox t_desc;
		private System.Windows.Forms.TextBox t_movescript;
		private System.Windows.Forms.ComboBox c_movetype;
		private System.Windows.Forms.NumericUpDown n_movespeed;
		private System.Windows.Forms.CheckBox ck_isobs;
		private System.Windows.Forms.CheckBox ck_obeyobs;
		private System.Windows.Forms.TextBox t_chrfile;
		private System.Windows.Forms.ComboBox c_facing;
		private System.Windows.Forms.NumericUpDown n_y;
		private System.Windows.Forms.NumericUpDown n_x;
		private System.Windows.Forms.GroupBox g_movescript;
		private System.Windows.Forms.GroupBox g_wanderrect;
		private System.Windows.Forms.NumericUpDown n_wx0;
		private System.Windows.Forms.NumericUpDown n_wy0;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.NumericUpDown n_wx1;
		private System.Windows.Forms.NumericUpDown n_wy1;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.GroupBox g_wanderzone;
		ArrayList entCopy = new ArrayList();
		private System.Windows.Forms.Label l_wzone;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.GroupBox g_edit;
		private System.Windows.Forms.LinkLabel linkLabel1;
		private System.Windows.Forms.ContextMenu cm_ent;
		private System.Windows.Forms.MenuItem m_delent;
		private System.Windows.Forms.MenuItem m_newent;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.Button b_delent;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.NumericUpDown n_wanderdelay;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.CheckBox chk_autoface;
		private System.Windows.Forms.Panel wanderpanel;
		private System.Windows.Forms.TextBox t_onActivate;
		private System.Windows.Forms.Label label19;
		Map map;

		public ArrayList EditedEntities { get { return entCopy; } }


		public EntityEditWnd()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			c_movetype.SelectedIndex=0;
			c_facing.SelectedIndex=1;
			DisableEditor();
			if(Global.ActiveMap.FileOnDisk==null) button2.Enabled=false;
		}
		public void init(Map map)
		{
			ArrayList ents = map.Entities;
			this.map = map;
			foreach(MapEntity me in ents)
				entCopy.Add(me.Clone());
			foreach(MapEntity me in entCopy)
			{
				ListViewItem lvi = new ListViewItem( new string[] { me.ID.ToString(), me.Description + "; "+me.ChrName } );
				lvi.Tag = me;
				lv_ents.Items.Add(lvi);
			}
		}
		public void seek(MapEntity me)
		{
			foreach(ListViewItem lvi in lv_ents.Items)
			{
				MapEntity me_ = (MapEntity)lvi.Tag;
				if(me_.ID==me.ID)
				{
					lvi.Selected=true;
					lvi.EnsureVisible();
				}
			}
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
			this.panel1 = new System.Windows.Forms.Panel();
			this.lv_ents = new System.Windows.Forms.ListView();
			this.ch_entID = new System.Windows.Forms.ColumnHeader();
			this.ch_entName = new System.Windows.Forms.ColumnHeader();
			this.cm_ent = new System.Windows.Forms.ContextMenu();
			this.m_newent = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.m_delent = new System.Windows.Forms.MenuItem();
			this.panel4 = new System.Windows.Forms.Panel();
			this.button1 = new System.Windows.Forms.Button();
			this.b_delent = new System.Windows.Forms.Button();
			this.panel2 = new System.Windows.Forms.Panel();
			this.b_Cancel = new System.Windows.Forms.Button();
			this.b_OK = new System.Windows.Forms.Button();
			this.panel3 = new System.Windows.Forms.Panel();
			this.g_edit = new System.Windows.Forms.GroupBox();
			this.label19 = new System.Windows.Forms.Label();
			this.t_onActivate = new System.Windows.Forms.TextBox();
			this.chk_autoface = new System.Windows.Forms.CheckBox();
			this.linkLabel1 = new System.Windows.Forms.LinkLabel();
			this.label13 = new System.Windows.Forms.Label();
			this.g_wanderzone = new System.Windows.Forms.GroupBox();
			this.l_wzone = new System.Windows.Forms.Label();
			this.t_desc = new System.Windows.Forms.TextBox();
			this.label10 = new System.Windows.Forms.Label();
			this.c_movetype = new System.Windows.Forms.ComboBox();
			this.label5 = new System.Windows.Forms.Label();
			this.n_movespeed = new System.Windows.Forms.NumericUpDown();
			this.label4 = new System.Windows.Forms.Label();
			this.ck_isobs = new System.Windows.Forms.CheckBox();
			this.ck_obeyobs = new System.Windows.Forms.CheckBox();
			this.button2 = new System.Windows.Forms.Button();
			this.t_chrfile = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.c_facing = new System.Windows.Forms.ComboBox();
			this.label2 = new System.Windows.Forms.Label();
			this.n_y = new System.Windows.Forms.NumericUpDown();
			this.n_x = new System.Windows.Forms.NumericUpDown();
			this.label1 = new System.Windows.Forms.Label();
			this.g_wanderrect = new System.Windows.Forms.GroupBox();
			this.wanderpanel = new System.Windows.Forms.Panel();
			this.label15 = new System.Windows.Forms.Label();
			this.n_wanderdelay = new System.Windows.Forms.NumericUpDown();
			this.label14 = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label9 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.n_wy1 = new System.Windows.Forms.NumericUpDown();
			this.n_wx1 = new System.Windows.Forms.NumericUpDown();
			this.n_wy0 = new System.Windows.Forms.NumericUpDown();
			this.n_wx0 = new System.Windows.Forms.NumericUpDown();
			this.label8 = new System.Windows.Forms.Label();
			this.g_movescript = new System.Windows.Forms.GroupBox();
			this.t_movescript = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.panel1.SuspendLayout();
			this.panel4.SuspendLayout();
			this.panel2.SuspendLayout();
			this.panel3.SuspendLayout();
			this.g_edit.SuspendLayout();
			this.g_wanderzone.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_movespeed)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_y)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_x)).BeginInit();
			this.g_wanderrect.SuspendLayout();
			this.wanderpanel.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_wanderdelay)).BeginInit();
			this.groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.n_wy1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wx1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wy0)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wx0)).BeginInit();
			this.g_movescript.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.lv_ents);
			this.panel1.Controls.Add(this.panel4);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
			this.panel1.Location = new System.Drawing.Point(392, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(232, 477);
			this.panel1.TabIndex = 0;
			// 
			// lv_ents
			// 
			this.lv_ents.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																					  this.ch_entID,
																					  this.ch_entName});
			this.lv_ents.ContextMenu = this.cm_ent;
			this.lv_ents.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lv_ents.FullRowSelect = true;
			this.lv_ents.GridLines = true;
			this.lv_ents.HideSelection = false;
			this.lv_ents.Location = new System.Drawing.Point(0, 0);
			this.lv_ents.MultiSelect = false;
			this.lv_ents.Name = "lv_ents";
			this.lv_ents.Size = new System.Drawing.Size(232, 429);
			this.lv_ents.TabIndex = 1;
			this.lv_ents.View = System.Windows.Forms.View.Details;
			this.lv_ents.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lv_ents_MouseDown);
			this.lv_ents.SelectedIndexChanged += new System.EventHandler(this.lv_ents_SelectedIndexChanged);
			// 
			// ch_entID
			// 
			this.ch_entID.Text = "ID";
			this.ch_entID.Width = 41;
			// 
			// ch_entName
			// 
			this.ch_entName.Text = "Name";
			this.ch_entName.Width = 163;
			// 
			// cm_ent
			// 
			this.cm_ent.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				   this.m_newent,
																				   this.menuItem3,
																				   this.m_delent});
			this.cm_ent.Popup += new System.EventHandler(this.cm_ent_Popup);
			// 
			// m_newent
			// 
			this.m_newent.Index = 0;
			this.m_newent.Text = "Create New Entity";
			this.m_newent.Click += new System.EventHandler(this.m_newent_Click);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 1;
			this.menuItem3.Text = "-";
			// 
			// m_delent
			// 
			this.m_delent.Index = 2;
			this.m_delent.Text = "&Delete Entity";
			this.m_delent.Click += new System.EventHandler(this.m_delent_Click);
			// 
			// panel4
			// 
			this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel4.Controls.Add(this.button1);
			this.panel4.Controls.Add(this.b_delent);
			this.panel4.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel4.Location = new System.Drawing.Point(0, 429);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(232, 48);
			this.panel4.TabIndex = 0;
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(8, 8);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(104, 32);
			this.button1.TabIndex = 0;
			this.button1.Text = "Add New Entity";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// b_delent
			// 
			this.b_delent.Enabled = false;
			this.b_delent.Location = new System.Drawing.Point(120, 8);
			this.b_delent.Name = "b_delent";
			this.b_delent.Size = new System.Drawing.Size(104, 32);
			this.b_delent.TabIndex = 0;
			this.b_delent.Text = "Delete Selected";
			this.b_delent.Click += new System.EventHandler(this.m_delent_Click);
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.b_Cancel);
			this.panel2.Controls.Add(this.b_OK);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel2.Location = new System.Drawing.Point(0, 477);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(624, 48);
			this.panel2.TabIndex = 1;
			this.panel2.Paint += new System.Windows.Forms.PaintEventHandler(this.panel2_Paint);
			// 
			// b_Cancel
			// 
			this.b_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.b_Cancel.Location = new System.Drawing.Point(536, 16);
			this.b_Cancel.Name = "b_Cancel";
			this.b_Cancel.TabIndex = 1;
			this.b_Cancel.Text = "Cancel";
			// 
			// b_OK
			// 
			this.b_OK.Location = new System.Drawing.Point(448, 16);
			this.b_OK.Name = "b_OK";
			this.b_OK.TabIndex = 0;
			this.b_OK.Text = "OK";
			this.b_OK.Click += new System.EventHandler(this.b_OK_Click);
			// 
			// panel3
			// 
			this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel3.Controls.Add(this.g_edit);
			this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel3.Location = new System.Drawing.Point(0, 0);
			this.panel3.Name = "panel3";
			this.panel3.Size = new System.Drawing.Size(392, 477);
			this.panel3.TabIndex = 2;
			// 
			// g_edit
			// 
			this.g_edit.Controls.Add(this.label19);
			this.g_edit.Controls.Add(this.t_onActivate);
			this.g_edit.Controls.Add(this.chk_autoface);
			this.g_edit.Controls.Add(this.linkLabel1);
			this.g_edit.Controls.Add(this.label13);
			this.g_edit.Controls.Add(this.g_wanderzone);
			this.g_edit.Controls.Add(this.t_desc);
			this.g_edit.Controls.Add(this.label10);
			this.g_edit.Controls.Add(this.c_movetype);
			this.g_edit.Controls.Add(this.label5);
			this.g_edit.Controls.Add(this.n_movespeed);
			this.g_edit.Controls.Add(this.label4);
			this.g_edit.Controls.Add(this.ck_isobs);
			this.g_edit.Controls.Add(this.ck_obeyobs);
			this.g_edit.Controls.Add(this.button2);
			this.g_edit.Controls.Add(this.t_chrfile);
			this.g_edit.Controls.Add(this.label3);
			this.g_edit.Controls.Add(this.c_facing);
			this.g_edit.Controls.Add(this.label2);
			this.g_edit.Controls.Add(this.n_y);
			this.g_edit.Controls.Add(this.n_x);
			this.g_edit.Controls.Add(this.label1);
			this.g_edit.Controls.Add(this.g_wanderrect);
			this.g_edit.Controls.Add(this.g_movescript);
			this.g_edit.Location = new System.Drawing.Point(8, 8);
			this.g_edit.Name = "g_edit";
			this.g_edit.Size = new System.Drawing.Size(376, 464);
			this.g_edit.TabIndex = 0;
			this.g_edit.TabStop = false;
			this.g_edit.Text = "Entity";
			// 
			// label19
			// 
			this.label19.Location = new System.Drawing.Point(24, 152);
			this.label19.Name = "label19";
			this.label19.TabIndex = 36;
			this.label19.Text = "onActivate";
			this.label19.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// t_onActivate
			// 
			this.t_onActivate.Location = new System.Drawing.Point(144, 152);
			this.t_onActivate.Name = "t_onActivate";
			this.t_onActivate.Size = new System.Drawing.Size(184, 20);
			this.t_onActivate.TabIndex = 35;
			this.t_onActivate.Text = "textBox1";
			// 
			// chk_autoface
			// 
			this.chk_autoface.Location = new System.Drawing.Point(136, 224);
			this.chk_autoface.Name = "chk_autoface";
			this.chk_autoface.TabIndex = 28;
			this.chk_autoface.Text = "Auto-Face";
			this.chk_autoface.CheckedChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// linkLabel1
			// 
			this.linkLabel1.ActiveLinkColor = System.Drawing.Color.Blue;
			this.linkLabel1.Location = new System.Drawing.Point(336, 432);
			this.linkLabel1.Name = "linkLabel1";
			this.linkLabel1.Size = new System.Drawing.Size(32, 16);
			this.linkLabel1.TabIndex = 27;
			this.linkLabel1.TabStop = true;
			this.linkLabel1.Text = "Help";
			this.linkLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.linkLabel1.VisitedLinkColor = System.Drawing.Color.Blue;
			this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
			// 
			// label13
			// 
			this.label13.Location = new System.Drawing.Point(192, 264);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(100, 16);
			this.label13.TabIndex = 26;
			this.label13.Text = "pixels/sec";
			// 
			// g_wanderzone
			// 
			this.g_wanderzone.Controls.Add(this.l_wzone);
			this.g_wanderzone.Location = new System.Drawing.Point(16, 320);
			this.g_wanderzone.Name = "g_wanderzone";
			this.g_wanderzone.Size = new System.Drawing.Size(320, 80);
			this.g_wanderzone.TabIndex = 25;
			this.g_wanderzone.TabStop = false;
			// 
			// l_wzone
			// 
			this.l_wzone.Location = new System.Drawing.Point(16, 16);
			this.l_wzone.Name = "l_wzone";
			this.l_wzone.Size = new System.Drawing.Size(280, 24);
			this.l_wzone.TabIndex = 0;
			this.l_wzone.Text = "Entity will restrict movement to zone 0";
			this.l_wzone.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// t_desc
			// 
			this.t_desc.Location = new System.Drawing.Point(144, 24);
			this.t_desc.Name = "t_desc";
			this.t_desc.Size = new System.Drawing.Size(208, 20);
			this.t_desc.TabIndex = 22;
			this.t_desc.Text = "";
			this.t_desc.TextChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label10
			// 
			this.label10.Location = new System.Drawing.Point(24, 24);
			this.label10.Name = "label10";
			this.label10.TabIndex = 21;
			this.label10.Text = "Description:";
			this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// c_movetype
			// 
			this.c_movetype.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_movetype.Items.AddRange(new object[] {
															"Off (Stationary)",
															"Wander Zone",
															"Wander Rectangle",
															"Movescript"});
			this.c_movetype.Location = new System.Drawing.Point(136, 288);
			this.c_movetype.Name = "c_movetype";
			this.c_movetype.Size = new System.Drawing.Size(121, 21);
			this.c_movetype.TabIndex = 13;
			this.c_movetype.SelectedIndexChanged += new System.EventHandler(this.c_movetype_SelectedIndexChanged);
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(16, 288);
			this.label5.Name = "label5";
			this.label5.TabIndex = 12;
			this.label5.Text = "Movement Type:";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_movespeed
			// 
			this.n_movespeed.Location = new System.Drawing.Point(136, 256);
			this.n_movespeed.Maximum = new System.Decimal(new int[] {
																		65535,
																		0,
																		0,
																		0});
			this.n_movespeed.Name = "n_movespeed";
			this.n_movespeed.Size = new System.Drawing.Size(56, 20);
			this.n_movespeed.TabIndex = 11;
			this.n_movespeed.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_movespeed.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(16, 256);
			this.label4.Name = "label4";
			this.label4.TabIndex = 10;
			this.label4.Text = "Movement Speed:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// ck_isobs
			// 
			this.ck_isobs.Location = new System.Drawing.Point(136, 200);
			this.ck_isobs.Name = "ck_isobs";
			this.ck_isobs.TabIndex = 9;
			this.ck_isobs.Text = "Obstruct Others";
			this.ck_isobs.CheckedChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// ck_obeyobs
			// 
			this.ck_obeyobs.Location = new System.Drawing.Point(136, 176);
			this.ck_obeyobs.Name = "ck_obeyobs";
			this.ck_obeyobs.Size = new System.Drawing.Size(136, 24);
			this.ck_obeyobs.TabIndex = 8;
			this.ck_obeyobs.Text = "Obey Obstructions";
			this.ck_obeyobs.CheckedChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(288, 120);
			this.button2.Name = "button2";
			this.button2.TabIndex = 7;
			this.button2.Text = "Browse...";
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// t_chrfile
			// 
			this.t_chrfile.Location = new System.Drawing.Point(144, 120);
			this.t_chrfile.Name = "t_chrfile";
			this.t_chrfile.Size = new System.Drawing.Size(136, 20);
			this.t_chrfile.TabIndex = 6;
			this.t_chrfile.Text = "";
			this.t_chrfile.TextChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(24, 120);
			this.label3.Name = "label3";
			this.label3.TabIndex = 5;
			this.label3.Text = "CHR File:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// c_facing
			// 
			this.c_facing.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.c_facing.Items.AddRange(new object[] {
														  "Up",
														  "Down",
														  "Left",
														  "Right"});
			this.c_facing.Location = new System.Drawing.Point(144, 88);
			this.c_facing.Name = "c_facing";
			this.c_facing.Size = new System.Drawing.Size(121, 21);
			this.c_facing.TabIndex = 4;
			this.c_facing.DoubleClick += new System.EventHandler(this.c_facing_DoubleClick);
			this.c_facing.SelectedIndexChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 88);
			this.label2.Name = "label2";
			this.label2.TabIndex = 3;
			this.label2.Text = "Facing:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// n_y
			// 
			this.n_y.Location = new System.Drawing.Point(216, 56);
			this.n_y.Maximum = new System.Decimal(new int[] {
																65535,
																0,
																0,
																0});
			this.n_y.Name = "n_y";
			this.n_y.Size = new System.Drawing.Size(64, 20);
			this.n_y.TabIndex = 2;
			this.n_y.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_y.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// n_x
			// 
			this.n_x.Location = new System.Drawing.Point(144, 56);
			this.n_x.Maximum = new System.Decimal(new int[] {
																65535,
																0,
																0,
																0});
			this.n_x.Name = "n_x";
			this.n_x.Size = new System.Drawing.Size(64, 20);
			this.n_x.TabIndex = 1;
			this.n_x.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_x.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 56);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(100, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Map Position:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// g_wanderrect
			// 
			this.g_wanderrect.Controls.Add(this.wanderpanel);
			this.g_wanderrect.Controls.Add(this.label12);
			this.g_wanderrect.Controls.Add(this.groupBox2);
			this.g_wanderrect.Location = new System.Drawing.Point(16, 320);
			this.g_wanderrect.Name = "g_wanderrect";
			this.g_wanderrect.Size = new System.Drawing.Size(320, 136);
			this.g_wanderrect.TabIndex = 24;
			this.g_wanderrect.TabStop = false;
			this.g_wanderrect.Visible = false;
			// 
			// wanderpanel
			// 
			this.wanderpanel.Controls.Add(this.label15);
			this.wanderpanel.Controls.Add(this.n_wanderdelay);
			this.wanderpanel.Controls.Add(this.label14);
			this.wanderpanel.Location = new System.Drawing.Point(16, 104);
			this.wanderpanel.Name = "wanderpanel";
			this.wanderpanel.Size = new System.Drawing.Size(288, 24);
			this.wanderpanel.TabIndex = 8;
			// 
			// label15
			// 
			this.label15.Location = new System.Drawing.Point(160, 0);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(100, 16);
			this.label15.TabIndex = 7;
			this.label15.Text = "ticks";
			// 
			// n_wanderdelay
			// 
			this.n_wanderdelay.Location = new System.Drawing.Point(104, 0);
			this.n_wanderdelay.Maximum = new System.Decimal(new int[] {
																		  65000,
																		  0,
																		  0,
																		  0});
			this.n_wanderdelay.Name = "n_wanderdelay";
			this.n_wanderdelay.Size = new System.Drawing.Size(56, 20);
			this.n_wanderdelay.TabIndex = 6;
			this.n_wanderdelay.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_wanderdelay.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label14
			// 
			this.label14.Location = new System.Drawing.Point(0, 0);
			this.label14.Name = "label14";
			this.label14.TabIndex = 5;
			this.label14.Text = "Wander Delay:";
			this.label14.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label12
			// 
			this.label12.Location = new System.Drawing.Point(16, 24);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(104, 32);
			this.label12.TabIndex = 4;
			this.label12.Text = "Wander Rectangle:";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.label9);
			this.groupBox2.Controls.Add(this.label11);
			this.groupBox2.Controls.Add(this.label7);
			this.groupBox2.Controls.Add(this.n_wy1);
			this.groupBox2.Controls.Add(this.n_wx1);
			this.groupBox2.Controls.Add(this.n_wy0);
			this.groupBox2.Controls.Add(this.n_wx0);
			this.groupBox2.Controls.Add(this.label8);
			this.groupBox2.Location = new System.Drawing.Point(120, 16);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(144, 80);
			this.groupBox2.TabIndex = 3;
			this.groupBox2.TabStop = false;
			// 
			// label9
			// 
			this.label9.Location = new System.Drawing.Point(16, 56);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(16, 16);
			this.label9.TabIndex = 2;
			this.label9.Text = "x2";
			// 
			// label11
			// 
			this.label11.Location = new System.Drawing.Point(72, 56);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(16, 16);
			this.label11.TabIndex = 2;
			this.label11.Text = "y2";
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(16, 24);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(16, 16);
			this.label7.TabIndex = 2;
			this.label7.Text = "x1";
			// 
			// n_wy1
			// 
			this.n_wy1.Location = new System.Drawing.Point(88, 48);
			this.n_wy1.Maximum = new System.Decimal(new int[] {
																  65000,
																  0,
																  0,
																  0});
			this.n_wy1.Name = "n_wy1";
			this.n_wy1.Size = new System.Drawing.Size(40, 20);
			this.n_wy1.TabIndex = 1;
			this.n_wy1.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_wy1.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// n_wx1
			// 
			this.n_wx1.Location = new System.Drawing.Point(32, 48);
			this.n_wx1.Maximum = new System.Decimal(new int[] {
																  65000,
																  0,
																  0,
																  0});
			this.n_wx1.Name = "n_wx1";
			this.n_wx1.Size = new System.Drawing.Size(40, 20);
			this.n_wx1.TabIndex = 1;
			this.n_wx1.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_wx1.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// n_wy0
			// 
			this.n_wy0.Location = new System.Drawing.Point(88, 16);
			this.n_wy0.Maximum = new System.Decimal(new int[] {
																  65000,
																  0,
																  0,
																  0});
			this.n_wy0.Name = "n_wy0";
			this.n_wy0.Size = new System.Drawing.Size(40, 20);
			this.n_wy0.TabIndex = 1;
			this.n_wy0.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_wy0.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// n_wx0
			// 
			this.n_wx0.Location = new System.Drawing.Point(32, 16);
			this.n_wx0.Maximum = new System.Decimal(new int[] {
																  65000,
																  0,
																  0,
																  0});
			this.n_wx0.Name = "n_wx0";
			this.n_wx0.Size = new System.Drawing.Size(40, 20);
			this.n_wx0.TabIndex = 1;
			this.n_wx0.KeyUp += new System.Windows.Forms.KeyEventHandler(this.n_x_KeyUp);
			this.n_wx0.ValueChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label8
			// 
			this.label8.Location = new System.Drawing.Point(72, 24);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(16, 16);
			this.label8.TabIndex = 2;
			this.label8.Text = "y1";
			// 
			// g_movescript
			// 
			this.g_movescript.Controls.Add(this.t_movescript);
			this.g_movescript.Controls.Add(this.label6);
			this.g_movescript.Location = new System.Drawing.Point(16, 320);
			this.g_movescript.Name = "g_movescript";
			this.g_movescript.Size = new System.Drawing.Size(320, 96);
			this.g_movescript.TabIndex = 23;
			this.g_movescript.TabStop = false;
			this.g_movescript.Visible = false;
			// 
			// t_movescript
			// 
			this.t_movescript.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
			this.t_movescript.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.t_movescript.Location = new System.Drawing.Point(120, 16);
			this.t_movescript.Name = "t_movescript";
			this.t_movescript.Size = new System.Drawing.Size(192, 21);
			this.t_movescript.TabIndex = 15;
			this.t_movescript.Text = "";
			this.t_movescript.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.t_movescript_KeyPress);
			this.t_movescript.TextChanged += new System.EventHandler(this.eFieldChanged);
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(8, 16);
			this.label6.Name = "label6";
			this.label6.TabIndex = 14;
			this.label6.Text = "Movement Script:";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// openFileDialog
			// 
			this.openFileDialog.Filter = "V3 CHR Files (*.chr)|*.chr";
			// 
			// EntityEditWnd
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(624, 525);
			this.Controls.Add(this.panel3);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.panel2);
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(632, 100000);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(632, 520);
			this.Name = "EntityEditWnd";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Entities";
			this.panel1.ResumeLayout(false);
			this.panel4.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.panel3.ResumeLayout(false);
			this.g_edit.ResumeLayout(false);
			this.g_wanderzone.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_movespeed)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_y)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_x)).EndInit();
			this.g_wanderrect.ResumeLayout(false);
			this.wanderpanel.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_wanderdelay)).EndInit();
			this.groupBox2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.n_wy1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wx1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wy0)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.n_wx0)).EndInit();
			this.g_movescript.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void button1_Click(object sender, System.EventArgs e)
		{
			MapEntity me = new MapEntity();
			me.ID = entCopy.Count;
			entCopy.Add(me);
			ListViewItem lvi = new ListViewItem( new string[] { me.ID.ToString(), me.Description + "; "+me.ChrName } );
			lvi.Tag=me;
			lv_ents.Items.Add(lvi);
			lvi.Selected=true;
			lvi.EnsureVisible();
		}

		private void label7_Click(object sender, System.EventArgs e)
		{
		
		}

		private void panel2_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
		
		}
		MapZone get_ent_zone(int entity)
		{
			MapEntity me = (MapEntity)entCopy[entity];
			if(map.Zones.Count==0) return null;
			MapZone mz = (MapZone)map.Zones[ map.ZoneLayer.getTile(me.TileX,me.TileY) ];
			return mz;
		}

		private void c_movetype_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			int c = c_movetype.SelectedIndex;
			g_wanderrect.Visible=false;
			g_movescript.Visible=false;
			g_wanderzone.Visible=false;
			switch(c)
			{
				case 3:
					g_wanderrect.Visible=false;
					g_wanderzone.Visible=false;
					g_movescript.Visible=true;
					break;
				case 2:
					g_wanderrect.Visible=true;
					if(g_wanderzone.Controls.Contains(wanderpanel))
						g_wanderzone.Controls.Remove(wanderpanel);
					g_wanderrect.Controls.Add(wanderpanel);
					wanderpanel.Location = new Point(wanderpanel.Location.X, 104);
					g_wanderzone.Visible=false;
					g_movescript.Visible=false;
					break;
				case 1:
					g_wanderrect.Visible=false;
					g_wanderzone.Visible=true;
					if(g_wanderrect.Controls.Contains(wanderpanel))
						g_wanderrect.Controls.Remove(wanderpanel);
					g_wanderzone.Controls.Add(wanderpanel);
					wanderpanel.Location = new Point(wanderpanel.Location.X, 48);
					g_movescript.Visible=false;
					MapZone mz = get_ent_zone(0);
                    l_wzone.Text = "Entity will restrict movement to zone "+((mz==null)?0:mz.ID);
					break;
				default:break;
			}			
		}

		private void button2_Click(object sender, System.EventArgs e)
		{
			openFileDialog.InitialDirectory = map.FileOnDisk.Directory.FullName;
			if(openFileDialog.ShowDialog()==DialogResult.Cancel)
				return;
			string f = Helper.GetRelativePath(map.FileOnDisk.Directory.FullName,openFileDialog.FileName);
			t_chrfile.Text = f;
		}
		bool bInvokeItems=true;
		void DisableEditor()
		{
			g_edit.Enabled = false;
		}
		void EnableEditor()
		{
			g_edit.Enabled = true;
		}
		void EmptyFields()
		{

		}
		void UpdateListView()
		{
			if(lv_ents.SelectedItems.Count==0)return;
			ListViewItem lvi = (ListViewItem)lv_ents.SelectedItems[0];
			MapEntity me = (MapEntity)lvi.Tag;
			lvi.SubItems[1].Text = me.Description + "; "+me.ChrName;
		}
		void ResetListView()
		{
			lv_ents.Items.Clear();
			foreach(MapEntity me in entCopy)
			{
				ListViewItem lvi = new ListViewItem( new string[] { me.ID.ToString(), me.Description + "; "+me.ChrName } );
				lvi.Tag = me;
				lv_ents.Items.Add(lvi);
			}
		}
		MapEntity curr;
		void FieldsToItem()
		{
			if(!bInvokeItems) return;
			if(lv_ents.SelectedItems.Count==0)return;
			if(curr==null) return;
			
			MapEntity me = curr;
			me.Description = t_desc.Text;
			me.TileX = (int)n_x.Value;
			me.TileY = (int)n_y.Value;
			me.Facing = (int)c_facing.SelectedIndex + 1;
			me.ChrName = t_chrfile.Text;
			me.ObeyObstruction = ck_obeyobs.Checked?1:0;
			me.IsObstruction = ck_isobs.Checked?1:0;
			me.Speed = (int)n_movespeed.Value;
			me.MoveType = (int)c_movetype.SelectedIndex;
			me.WanderRectangle.x0 = (int)n_wx0.Value;
			me.WanderRectangle.y0 = (int)n_wy0.Value;
			me.WanderRectangle.x1 = (int)n_wx1.Value;
			me.WanderRectangle.y1 = (int)n_wy1.Value;
			me.MoveScript = t_movescript.Text;
			me.WanderDelay = (int)n_wanderdelay.Value;
			me.AutoFace = chk_autoface.Checked?1:0;
			me.onActivate = t_onActivate.Text;
		}
		void ItemsToField(MapEntity me)
		{
			bInvokeItems=false;
			t_desc.Text = me.Description;
			n_x.Value = me.TileX;
			n_y.Value = me.TileY;
			c_facing.SelectedIndex = me.Facing -1;
			t_chrfile.Text = me.ChrName;
			ck_obeyobs.Checked = (me.ObeyObstruction==1);
			ck_isobs.Checked = (me.IsObstruction==1);
			n_movespeed.Value = me.Speed;
			c_movetype.SelectedIndex = me.MoveType;
			n_wx0.Value = me.WanderRectangle.x0;
			n_wy0.Value = me.WanderRectangle.y0;
			n_wx1.Value = me.WanderRectangle.x1;
			n_wy1.Value = me.WanderRectangle.y1;
			t_movescript.Text = me.MoveScript;
			n_wanderdelay.Value = me.WanderDelay;
			chk_autoface.Checked = (me.AutoFace==1);
			t_onActivate.Text = me.onActivate;
			bInvokeItems=true;
		}

		private void lv_ents_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			FieldsToItem();
			if(lv_ents.SelectedIndices.Count==0)
			{
				EmptyFields();
				DisableEditor();
				b_delent.Enabled=false;
				return;
			}
			ListViewItem lvi = (ListViewItem)lv_ents.SelectedItems[0];
			MapEntity me = (MapEntity)lvi.Tag;
			curr = me;
			ItemsToField(me);
			EnableEditor();
			b_delent.Enabled=true;
		}
		private void eFieldChanged(object sender, System.EventArgs e)
		{
			FieldsToItem();
			UpdateListView();			
		}

		private void c_facing_DoubleClick(object sender, System.EventArgs e)
		{
		
		}

		private void n_x_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			FieldsToItem();		
			UpdateListView();			
		}

		private void t_movescript_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			// compare to valid chars
			char c = e.KeyChar.ToString().ToUpper()[0];
//			MessageBox.Show(""+(int)e.KeyChar);
			char[] valid = new char[] {'P','T','L','R','U','D','S','W','F','B','X','Y','Z',(char)8};
			if(char.IsDigit(c))return;
			bool ok = false;
			foreach(char ch in valid)
				if(ch==c)
					ok=true;
			e.Handled = !ok;
		}

		private void cm_ent_Popup(object sender, System.EventArgs e)
		{
			if(lv_ents.SelectedItems.Count==0) m_delent.Enabled=false;
			else m_delent.Enabled=true;
		}

		private void lv_ents_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
		}

		private void m_newent_Click(object sender, System.EventArgs e)
		{
			MapEntity me = new MapEntity();
			me.ID = entCopy.Count;
			entCopy.Add(me);
			ListViewItem lvi = new ListViewItem( new string[] { me.ID.ToString(), me.Description + "; "+me.ChrName } );
			lvi.Tag=me;
			lv_ents.Items.Add(lvi);
			lvi.Selected=true;
			lvi.EnsureVisible();	
		}
		private void update_IDs()
		{
			for(int i=0;i<entCopy.Count;i++)
			{
				MapEntity me = (MapEntity)entCopy[i];
				me.ID = i;
			}
			ResetListView();
		}

		private void m_delent_Click(object sender, System.EventArgs e)
		{
			if(lv_ents.SelectedItems.Count==0) return;
			MapEntity me = (MapEntity) lv_ents.SelectedItems[0].Tag;
			if( MessageBox.Show("Are you sure you want to delete Entity #" + me.ID + "? This action can not be undone. \r\nWARNING: If there are entities in the list with a higher ID than this entity, they will be shifted down and will henceforth have a different ID.", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning)
				== DialogResult.Cancel ) return;
			entCopy.Remove(me);
			update_IDs();
		}

		private void b_OK_Click(object sender, System.EventArgs e)
		{
			DialogResult = DialogResult.OK;
			FieldsToItem();
			Close();
		}

		private void linkLabel1_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
		{
			Global.OpenHelp("index.html");
		}


	}
}
