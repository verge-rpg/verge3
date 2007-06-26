using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace winmaped2
{

	/// <summary>
	/// Summary description for EditVsp.
	/// </summary>
	public class EditVsp : System.Windows.Forms.Form
	{
		private TilesetViewer tilesetViewer;
		public System.Windows.Forms.VScrollBar vScrollBar1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.Button btnDeleteRow;
		private System.Windows.Forms.Button btnInsertRow;
		private System.Windows.Forms.Button btnInsertTile;
		private System.Windows.Forms.Button btnDeleteTile;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Button btnCopy;
		private System.Windows.Forms.Button btnPaste;
		private System.Windows.Forms.Button btnCut;
		private System.Windows.Forms.Button button1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Vsp24 vsp { get { return Global.ActiveVsp; } }

		public EditVsp()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			this.KeyPreview = true;
			tilesetViewer.init(this);
			updateScrollbar();
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			if(e.Control && e.KeyCode == Keys.Z)
			{
				Global.opManager.undo();
				tilesetViewer.Invalidate();
			}
			if(e.Control && e.KeyCode == Keys.Y)
			{
				Global.opManager.redo();
				tilesetViewer.Invalidate();
			}
			if(e.Control && e.KeyCode == Keys.C)
				tilesetViewer.copy();
			if(e.Control && e.KeyCode == Keys.X)
				tilesetViewer.cut();
			if(e.Control && e.KeyCode == Keys.V)
				tilesetViewer.paste();
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

		public void updateScrollbar()
		{
			vScrollBar1.Minimum = 0;
			vScrollBar1.Maximum = vsp.Tiles.Count/tilesetViewer.TilesWide;
			vScrollBar1.LargeChange = tilesetViewer.TilesHigh;
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.tilesetViewer = new winmaped2.TilesetViewer();
			this.vScrollBar1 = new System.Windows.Forms.VScrollBar();
			this.btnDeleteRow = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnDeleteTile = new System.Windows.Forms.Button();
			this.btnInsertTile = new System.Windows.Forms.Button();
			this.btnInsertRow = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.btnCut = new System.Windows.Forms.Button();
			this.btnPaste = new System.Windows.Forms.Button();
			this.btnCopy = new System.Windows.Forms.Button();
			this.button1 = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// tilesetViewer
			// 
			this.tilesetViewer.Dock = System.Windows.Forms.DockStyle.Left;
			this.tilesetViewer.Location = new System.Drawing.Point(0, 0);
			this.tilesetViewer.Name = "tilesetViewer";
			this.tilesetViewer.Size = new System.Drawing.Size(320, 379);
			this.tilesetViewer.TabIndex = 0;
			// 
			// vScrollBar1
			// 
			this.vScrollBar1.Dock = System.Windows.Forms.DockStyle.Left;
			this.vScrollBar1.Location = new System.Drawing.Point(320, 0);
			this.vScrollBar1.Name = "vScrollBar1";
			this.vScrollBar1.Size = new System.Drawing.Size(16, 379);
			this.vScrollBar1.TabIndex = 0;
			this.vScrollBar1.ValueChanged += new System.EventHandler(this.vScrollBar1_ValueChanged);
			this.vScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.vScrollBar1_Scroll);
			// 
			// btnDeleteRow
			// 
			this.btnDeleteRow.Location = new System.Drawing.Point(16, 56);
			this.btnDeleteRow.Name = "btnDeleteRow";
			this.btnDeleteRow.Size = new System.Drawing.Size(80, 24);
			this.btnDeleteRow.TabIndex = 0;
			this.btnDeleteRow.Text = "Delete Row";
			this.btnDeleteRow.Click += new System.EventHandler(this.btnDeleteRow_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnDeleteTile);
			this.groupBox1.Controls.Add(this.btnInsertTile);
			this.groupBox1.Controls.Add(this.btnInsertRow);
			this.groupBox1.Controls.Add(this.btnDeleteRow);
			this.groupBox1.Location = new System.Drawing.Point(344, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(112, 176);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "From Bottom...";
			// 
			// btnDeleteTile
			// 
			this.btnDeleteTile.Location = new System.Drawing.Point(16, 136);
			this.btnDeleteTile.Name = "btnDeleteTile";
			this.btnDeleteTile.Size = new System.Drawing.Size(80, 24);
			this.btnDeleteTile.TabIndex = 4;
			this.btnDeleteTile.Text = "Delete Tile";
			this.btnDeleteTile.Click += new System.EventHandler(this.btnDeleteTile_Click);
			// 
			// btnInsertTile
			// 
			this.btnInsertTile.Location = new System.Drawing.Point(16, 104);
			this.btnInsertTile.Name = "btnInsertTile";
			this.btnInsertTile.Size = new System.Drawing.Size(80, 24);
			this.btnInsertTile.TabIndex = 3;
			this.btnInsertTile.Text = "Insert Tile";
			this.btnInsertTile.Click += new System.EventHandler(this.btnInsertTile_Click);
			// 
			// btnInsertRow
			// 
			this.btnInsertRow.Location = new System.Drawing.Point(16, 24);
			this.btnInsertRow.Name = "btnInsertRow";
			this.btnInsertRow.Size = new System.Drawing.Size(80, 24);
			this.btnInsertRow.TabIndex = 1;
			this.btnInsertRow.Text = "Insert Row";
			this.btnInsertRow.Click += new System.EventHandler(this.btnInsertRow_Click);
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(0, 0);
			this.button2.Name = "button2";
			this.button2.TabIndex = 0;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.btnCut);
			this.groupBox2.Controls.Add(this.btnPaste);
			this.groupBox2.Controls.Add(this.btnCopy);
			this.groupBox2.Location = new System.Drawing.Point(344, 192);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(112, 144);
			this.groupBox2.TabIndex = 5;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Clipboard";
			// 
			// btnCut
			// 
			this.btnCut.Location = new System.Drawing.Point(16, 60);
			this.btnCut.Name = "btnCut";
			this.btnCut.Size = new System.Drawing.Size(80, 24);
			this.btnCut.TabIndex = 6;
			this.btnCut.Text = "Cut";
			this.btnCut.Click += new System.EventHandler(this.btnCut_Click);
			// 
			// btnPaste
			// 
			this.btnPaste.Enabled = false;
			this.btnPaste.Location = new System.Drawing.Point(16, 96);
			this.btnPaste.Name = "btnPaste";
			this.btnPaste.Size = new System.Drawing.Size(80, 24);
			this.btnPaste.TabIndex = 5;
			this.btnPaste.Text = "Paste";
			this.btnPaste.Click += new System.EventHandler(this.btnPaste_Click);
			// 
			// btnCopy
			// 
			this.btnCopy.Location = new System.Drawing.Point(16, 24);
			this.btnCopy.Name = "btnCopy";
			this.btnCopy.Size = new System.Drawing.Size(80, 24);
			this.btnCopy.TabIndex = 4;
			this.btnCopy.Text = "Copy";
			this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(344, 344);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(112, 23);
			this.button1.TabIndex = 6;
			this.button1.Text = "Done";
			// 
			// EditVsp
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(466, 379);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.vScrollBar1);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.tilesetViewer);
			this.Controls.Add(this.groupBox2);
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(474, 32000);
			this.Menu = this.mainMenu1;
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(474, 408);
			this.Name = "EditVsp";
			this.Text = "Arrange Tiles";
			this.Resize += new System.EventHandler(this.EditVsp_Resize);
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void vScrollBar1_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
		{
		
		}

		private void vScrollBar1_ValueChanged(object sender, System.EventArgs e)
		{
			tilesetViewer.Invalidate();
		}

		private void EditVsp_Resize(object sender, System.EventArgs e)
		{
			updateScrollbar();
			tilesetViewer.Invalidate();
		}

		private void btnDeleteRow_Click(object sender, System.EventArgs e)
		{
			int toDelete = vsp.tileCount % tilesetViewer.TilesWide;
			if(toDelete == 0)
				toDelete = tilesetViewer.TilesWide;
			if(vsp.tileCount == 0)
				toDelete = 0;
			
			if(toDelete == 0)
				return;

			int startDelete = (vsp.tileCount/tilesetViewer.TilesWide)*tilesetViewer.TilesWide;
			if(toDelete == tilesetViewer.TilesWide)
				startDelete -= tilesetViewer.TilesWide;
			Ops.AddRemoveTilesGroup artg = new Ops.AddRemoveTilesGroup(0);

			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Delete Row");

			for(int i=0;i<toDelete;i++)
				artg.addRecord(startDelete,1);
			
			om.addExec(artg);
			om.endGroup();

			updateScrollbar();
			tilesetViewer.Invalidate();
		}

		private void btnInsertRow_Click(object sender, System.EventArgs e)
		{
			int todo = tilesetViewer.TilesWide - (vsp.tileCount % tilesetViewer.TilesWide);
			if(todo == 0)
				todo = tilesetViewer.TilesWide;

			int startInsert = vsp.tileCount;
			Ops.AddRemoveTilesGroup artg = new Ops.AddRemoveTilesGroup(0);

			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Insert Row");

			for(int i=0;i<todo;i++)
				artg.addRecord(startInsert,0);
			
			om.addExec(artg);
			om.endGroup();

			updateScrollbar();
			tilesetViewer.Invalidate();
		}

		private void btnDeleteTile_Click(object sender, System.EventArgs e)
		{
			if(Global.ActiveVsp.tileCount == 0)
				return;

			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Delete Tile");
			Ops.AddRemoveTilesGroup artg = new Ops.AddRemoveTilesGroup(0);
			artg.addRecord(Global.ActiveVsp.tileCount-1,1);
			om.addExec(artg);
			om.endGroup();		
			updateScrollbar();
			tilesetViewer.Invalidate();
		}

		private void btnInsertTile_Click(object sender, System.EventArgs e)
		{
			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Insert Tile");
			Ops.AddRemoveTilesGroup artg = new Ops.AddRemoveTilesGroup(0);
			artg.addRecord(Global.ActiveVsp.tileCount,0);
			om.addExec(artg);
			om.endGroup();
			updateScrollbar();
			tilesetViewer.Invalidate();
		}

		private void btnCopy_Click(object sender, System.EventArgs e)
		{
			tilesetViewer.copy();	
		}

		private void btnPaste_Click(object sender, System.EventArgs e)
		{
			tilesetViewer.paste();
		}

		private void btnCut_Click(object sender, System.EventArgs e)
		{
			tilesetViewer.cut();
		}

	}




	unsafe class TilesetViewer : Control, Selection.IMaskReader
	{
		public TilesetViewer() {}
		public void init(EditVsp parent)
		{
			this.parent = parent;
			SetStyle(ControlStyles.AllPaintingInWmPaint,true);
			SetStyle(ControlStyles.UserPaint,true);
			SetStyle(ControlStyles.DoubleBuffer,true);
			SetStyle(ControlStyles.Opaque,true);
			SetStyle(ControlStyles.UserMouse,true);
			
			slideTimer = new System.Timers.Timer(100);
			slideTimer.Elapsed += new System.Timers.ElapsedEventHandler(slideTimer_Elapsed);

		}

		public int TilesWide
		{
			get { return Width/16; }
		}
		public int TilesHigh
		{
			get { return Height/16; }
		}

		public int scrollOffset { get { return parent.vScrollBar1.Value*TilesWide; } }

		public int logicalRow { get { return scrollOffset/TilesWide; } }

		Selection selection = new Selection();
		Selection originalSelection;
		int[,] selectedTiles;
		Selection srcSelection = null;
		int mx,my,mtx,mty;
		int mx1,my1,mtx1,mty1;
		int bmtx0,bmtx1,bmty0,bmty1;
		bool bSelectingRect = false;
		bool bDraggingTiles = false;
		int xDrag, yDrag;
		public EditVsp parent = null;
		public bool bSelection = false;
		System.Timers.Timer slideTimer;
		
		//0 = dragged tiles
		//1 = tiledata
		//enum DragLayerType { Tiles, Tiledata };
		//selectionType;

		EditLayer editLayer = null;

		abstract class EditLayer
		{
			public abstract void render();
			public int x,y;
			public int w,h;
		}

		class TileEditLayer : EditLayer, Selection.IMaskReader
		{
			public TileEditLayer(Selection sel)
			{

			}

			int Selection.IMaskReader.readInteger(int x, int y)
			{
				//int i = y*this.TilesWide+x;
				//if(i>=parent.vsp.tileCount) return -1;
				return 0;
			}

			int Selection.IMaskReader.invalidInteger { get { return -1; } }

			public override void render()
			{
			}
		}		
		

		void executeMove()
		{
			Map map = Global.ActiveMap;
			Vsp24 vsp = Global.ActiveVsp;
			Operations.OperationManager om = Global.opManager;

			om.beginGroup("VSP Manager: Smart Tile Move");

			Ops.SetTileGroup stg = new Ops.SetTileGroup(0);
			Ops.SetTiledataGroup stdg = new Ops.SetTiledataGroup(0);

			//move tiles
			for(int y=0;y<originalSelection.height;y++)
				for(int x=0;x<originalSelection.width;x++)
				{
					int xs = x + originalSelection.x;
					int ys = y + originalSelection.y;
					int xd = x + selection.x;
					int yd = y + selection.y;
					int ts = ys*TilesWide+xs;
					int td = yd*TilesWide+xd;

					if(ts>=vsp.Tiles.Count||ts<0) continue;

					stdg.addRecord(td,((Vsp24Tile)vsp.Tiles[ts]).Pixels);

					for(int l=0;l<map.Layers.Count;l++)
					{
						MapLayer ml = (MapLayer)map.Layers[l];
						if(ml.type != LayerType.Tile)
							continue;
						for(int yi=0;yi<ml.Height;yi++)
							for(int xi=0;xi<ml.Width;xi++)
							{
								int t = ml.getTile(xi,yi);
								if(t == ts)
									stg.addRecord(l,xi,yi,td);
							}
					}
				}

			om.add(stg);
			om.add(stdg);
			om.endGroupExec();
		}

		public void cut()
		{
			if(!bSelection)
				return;

			copy();

			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Cut Tiledata");
			Ops.SetTiledataGroup stdg = new Ops.SetTiledataGroup(0);

			Selection s  = originalSelection;
			if (s.width>0&&s.height>0)
			{
				int y0 = s.y;
				int x0 = s.x;
				int magenta = pr2.Render.makeColor(255,0,255);
				for(int y=0;y<s.height;y++)
					for(int x=0;x<s.width;x++)
					{
						int t = (s.y+y)*TilesWide + s.x+x;
						int[] newData = new int[16*16];
						for(int i=0;i<256;i++)
							newData[i] = magenta;
						stdg.addRecord(t,newData);
					}

				om.add(stdg);
			}
			om.endGroupExec();
	
			Invalidate();	
		}

		public void copy()
		{
			if(!bSelection)
				return;

			Selection s  = originalSelection;

			

			if(s.height>0&&s.width>0)
			{
				pr2.Render.Image img = pr2.Render.Image.create(s.width*16,s.height*16);
				int y0 = s.y;
				int x0 = s.x;
				for(int y=0;y<s.height;y++)
					for(int x=0;x<s.width;x++)
					{
						if(!s.getPoint(x0+x,y0+y))
							continue;
						int t = (y0+y)*TilesWide+x+x0;
						int[] p = Global.ActiveVsp.getPixels(t);
						fixed(int *data = p)
							pr2.Render.renderTile32(img,x*16,y*16,data,true);
					}

				WindowsClipboard.setImage(img);
				img.Dispose();
			}
		}

		public void paste()
		{
			if(!WindowsClipboard.IsImage)
				return;

			if(!bSelection)
				return;

			pr2.Render.Image img = WindowsClipboard.getImage();

			int tx = img.width/16;
			int ty = img.height/16;

			Selection s = originalSelection;
			this.selection = originalSelection.copy();

			if(tx != s.width)
				return;
			if(ty != s.height)
				return;

			Operations.OperationManager om = Global.opManager;
			om.beginGroup("VSP Manager: Paste Tiledata");

			Ops.SetTiledataGroup stdg = new Ops.SetTiledataGroup(0);
			
			int y0 = s.y;
			int x0 = s.x;
			int[] arrImg = img.getArray();
			for(int y=0;y<s.height;y++)
				for(int x=0;x<s.width;x++)
				{
					int t = (s.y+y)*TilesWide + s.x+x;
					stdg.addRecord(t,Global.Misc.sliceIntArrayImage(arrImg,img.width,x*16,y*16,16,16));
				}

			om.add(stdg);
			om.endGroupExec();

	
			img.Dispose();
			Invalidate();			
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if(e.Button == MouseButtons.Right)
			{
				bSelection = false;
				Invalidate();
				return;
			}
			else if(e.Button == MouseButtons.Left)
			{
				if(bSelection)
				{
					if(e.Clicks == 2)
					{
						executeMove();
						Invalidate();
					}
					else
					{
						if(selection.getPoint(e.X/16,e.Y/16+logicalRow))
						{
							bDraggingTiles = true;
							srcSelection = selection.copy();
						}
						else
							bSelection = false;
					}
				}
				
				if(!bDraggingTiles)
					bSelectingRect = true;

				mx1 = mx = e.X;
				my1 = my = e.Y;
				clip(ref mx1, ref my1);
				clip(ref mx, ref my);
				mtx = (mx+8)/16;
				mty = (my+8)/16 + logicalRow;
				bmtx0 = mx/16;
				bmty0 = my/16 + logicalRow;
				
				xDrag = 0;
				yDrag = 0;

				slideTimer.Enabled = true;
				Capture = true;
				Invalidate();
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			if(e.Button == MouseButtons.Left)
			{
				if(bSelectingRect)
				{
					bSelectingRect = false;
					slideTimer.Enabled = false;
					Capture = false;
					bSelection = true;

					Rectangle r = Rectangle.FromLTRB(
						Math.Min(mtx,mtx1),
						Math.Min(mty,mty1),
						Math.Max(mtx,mtx1),
						Math.Max(mty,mty1));
					selection.clear();
					selection.setRectangle(r,true);
					originalSelection = selection.copy();
					editLayer = new TileEditLayer(selection);
					Invalidate();
				}
				if(bDraggingTiles)
				{
					slideTimer.Enabled = false;
					Capture = false;
					bSelection = true;
					bDraggingTiles = false;
					Invalidate();
				}
			}
		}

		void handleMouseMove()
		{
			Point p = new Point(0,0);
			p = this.PointToScreen(p);
			p.X = Control.MousePosition.X - p.X;
			p.Y = Control.MousePosition.Y - p.Y;
			handleMouseMove(p.X,p.Y);
		}

		void clip(ref int x, ref int y)
		{
			if(x<0) x = 0;
			if(y<0) y = 0;
			if(x >= Width-1) x = Width-1;
			if(y >= Height-1) y = Height-1;
		}

		void handleMouseMove(int x, int y)
		{
			mx1 = x;
			my1 = y;
			if(bSelectingRect)
			{
				clip(ref mx1, ref my1);
			}
			mtx1 = (mx1+8) / 16;
			mty1 = (my1+8) / 16 + logicalRow;
			bmtx1 = mx1/16;
			bmty1 = my1/16 + logicalRow;
			if(bSelectingRect)
			{
				Invalidate();
			}
			if(bDraggingTiles)
			{
				selection.offset(bmtx1 - bmtx0 - xDrag,bmty1 - bmty0 - yDrag);
				xDrag = bmtx1 - bmtx0;
				yDrag = bmty1 - bmty0;
				Invalidate();
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			handleMouseMove(e.X,e.Y);
		}


		protected override void OnPaint(PaintEventArgs e)
		{
			if(parent == null)
			{
				e.Graphics.FillRectangle(System.Drawing.Brushes.Black,e.ClipRectangle.Left,e.ClipRectangle.Right,e.ClipRectangle.Width,e.ClipRectangle.Height);
				return;
			}

			e.Graphics.PixelOffsetMode = PixelOffsetMode.Half;
			e.Graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;

			Bitmap bmp = new Bitmap(TilesWide*16,(TilesHigh+1)*16,System.Drawing.Imaging.PixelFormat.Format32bppArgb);
			pr2.Render.Image qimg = pr2.Render.Image.lockBitmap(bmp);
			
			int row=0,col=0;
			unchecked
			{
				
				for (int i = scrollOffset; i < parent.vsp.Tiles.Count; i++ )
				{

					fixed(int *tiledata = ((Vsp24Tile)parent.vsp.Tiles[i]).Pixels)
						pr2.Render.renderTile32(qimg,col*16,row*16,tiledata,true);
					
					if(bSelection)
					{
						int xx = col;
						int yy = row+logicalRow;
						if(selection.getPoint(xx,yy))
						{
							int tile = originalSelection.getPointIntegerValue(xx - selection.x + originalSelection.x, yy-selection.y + originalSelection.y,this);
							if(tile != -1)
								fixed(int *tiledata = ((Vsp24Tile)parent.vsp.Tiles[tile]).Pixels)
									pr2.Render.renderTile32(qimg,col*16,row*16,tiledata,true);
						}
					}

					col++;
					if ( col == TilesWide )
					{
						col = 0;
						row++;
						if ( row == TilesHigh )
							break;
					}
				}

			}

			//render the empty area
			while(row != TilesHigh+1)
			{
				while(col != TilesWide)
				{
					pr2.Render.renderColoredStippleTile(qimg,col*16,row*16,pr2.Render.makeColor(0,0,0),pr2.Render.makeColor(192,192,192));
					col++;
				}
				col = 0;
				row++;
			}


			qimg.Dispose();
			e.Graphics.DrawImage(bmp, 0, 0, TilesWide*16, (TilesHigh+1)*16);
			bmp.Dispose();

	
			e.Graphics.PixelOffsetMode = PixelOffsetMode.Default;

			if(bSelectingRect)
			{
				Point p0 = (new Point(Math.Min(mtx,mtx1),Math.Min(mty,mty1)));
				Point p1 = (new Point(Math.Max(mtx,mtx1),Math.Max(mty,mty1)));
				Pen pen = new Pen(Color.White);
				pen.DashStyle = DashStyle.Dash;
				pen.Width = 1;
				e.Graphics.DrawRectangle(pen,p0.X*16,(p0.Y-logicalRow)*16,(p1.X - p0.X)*16,(p1.Y-p0.Y)*16);
				pen.Dispose();
			}
			if(bSelection)
			{
				GraphicsPath gp = new GraphicsPath();
				
				selection.updateGraphicsPath(gp,16,0,-logicalRow*16);
				Pen pen = new Pen(Color.FromArgb(128,0,0,0));
				pen.Width = 5;
				pen.DashStyle = DashStyle.Solid;
				e.Graphics.TranslateTransform(1.0f,1.0f);
				e.Graphics.DrawPath(pen,gp);
				e.Graphics.TranslateTransform(-1.0f,-1.0f);

				pen.Color = Color.White;
				pen.Width = 1;
				pen.DashStyle = DashStyle.Dash;
				e.Graphics.DrawPath(pen,gp);
				pen.Dispose();
				gp.Dispose();
			}
		}

		private void slideTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
		{
			if(bSelectingRect || bDraggingTiles)
			{
				if(my1<0)
				{
					if(parent.vScrollBar1.Value>0)
						parent.vScrollBar1.Value--;
					handleMouseMove();
				}
				else if(my1>this.Height)
				{
					if(parent.vScrollBar1.Value<parent.vScrollBar1.Maximum-parent.vScrollBar1.LargeChange+1)
						parent.vScrollBar1.Value++;
					handleMouseMove();
				}
			}
		}

		int Selection.IMaskReader.readInteger(int x, int y)
		{
			int i = y*this.TilesWide+x;
			if(i>=parent.vsp.tileCount) return -1;
			return i;
		}

		int Selection.IMaskReader.invalidInteger { get { return -1; } }


	}


	class Selection
	{
		int[,] mask;
		Rectangle r = new Rectangle();

		public int x { get { return r.Left; } }
		public int y { get { return r.Top; } }
		public int width
		{
			get
			{
				int minx = minX(10000);
				int maxx = maxX(10000);
				if(minx==10000 || maxx==10000)
					return 0;
				else return maxx-minx+1;
			}
		}
		public int height
		{
			get
			{
				int miny = minY(10000);
				int maxy = maxY(10000);
				if(miny==10000 || maxy==10000)
					return 0;
				else return maxy-miny+1;
			}
		}

		public interface IMaskReader
		{
			int readInteger(int x, int y);
			int invalidInteger { get; }
		}

		public int getPointIntegerValue(int x, int y, IMaskReader imr)
		{
			if(getPoint(x,y))
				return imr.readInteger(x,y);
			else
				return imr.invalidInteger;
		}

		public int[,] createIntegerMask(IMaskReader imr)
		{
			int[,] ret = new int[r.Height,r.Width];
			for(int y=0;y<r.Height;y++)
				for(int x=0;x<r.Width;x++)
					ret[y,x] = imr.readInteger(r.Left+x,r.Top+y);
			return ret;
		}

		public Selection copy()
		{
			Selection s = new Selection(r.Width,r.Height);
			s.r = r;
			s.mask = (int[,])mask.Clone();
			return s;
		}

		public Selection()
		{
			mask = new int[0,0];
			r = new Rectangle(0,0,0,0);
		}

		public void offset(int x, int y)
		{
			r = new Rectangle(r.Left + x, r.Top + y, r.Right - r.Left,r.Bottom - r.Top);
		}

		public Selection(int w, int h)
		{
			mask = new int[h,((w+31)&~31)>>5];
			r = new Rectangle(0,0,w,h);
		}


		public int minX(int sentinel)
		{
			for(int x=0;x<r.Width;x++)
				for(int y=0;y<r.Height;y++)
					if(getPoint(this.x+x,this.y+y))
						return x+this.x;
			return sentinel;
		}

		public int maxX(int sentinel)
		{
			for(int x=r.Width-1;x>=0;x--)
				for(int y=0;y<r.Height;y++)
					if(getPoint(this.x+x,this.y+y))
						return x+this.x;
			return sentinel;
		}

		public int minY(int sentinel)
		{
			for(int y=0;y<r.Height;y++)
				for(int x=0;x<r.Width;x++)
					if(getPoint(this.x+x,this.y+y))
						return y+this.y;
			return sentinel;
		}

		public int maxY(int sentinel)
		{
			for(int y=r.Height-1;y>=0;y--)
				for(int x=0;x<r.Width;x++)
					if(getPoint(this.x+x,this.y+y))
						return y+this.y;
			return sentinel;
		}


		private void alterRectangleToInclude(int x, int y)
		{
			Rectangle rNew;
			//Rectangle rOld;
			//special case, when x/y dont make sense because we have no rectangle
			if(r.Width == 0 && r.Height == 0)
			{
				mask = new int[1,1];
				r = new Rectangle(x,y,1,1);
				return;
			}

			//special case: we may already contain that point
			if(r.Contains(x,y))
				return;

			rNew = Rectangle.FromLTRB(
				Math.Min(x,r.Left),
				Math.Min(y,r.Top),
				Math.Max(x+1,r.Right),
				Math.Max(y+1,r.Bottom));

			int[,] newMask = new int[rNew.Height,((rNew.Width+31)&~31)>>5];

			for(int yi=0;yi<rNew.Height;yi++)
			{
				for(int xi=0;xi<rNew.Width;xi++)
				{
					if(r.Contains(rNew.Left+xi,rNew.Top+yi))
						_setPoint(newMask,rNew,rNew.Left+xi,rNew.Top+yi,_getPoint(mask,r,rNew.Left+xi,rNew.Top+yi));
				}
			}

			mask = newMask;
			r = rNew;
		}

		private bool _getPoint(int[,] mask, Rectangle r, int x, int y)
		{
			x -= r.Left;
			y -= r.Top;
			int i = (mask[y,x/32] >> (x&31))&1;
			return i==1;
		}

		private void _setPoint(int[,] mask, Rectangle r, int x, int y, bool value)
		{
			x -= r.Left;
			y -= r.Top;
			mask[y,x/32] &= ~(1<<(x&31));
			if(value)
				mask[y,x/32] |= 1<<(x&31);
		}
			
		public void clear()
		{
			r.Width = r.Height = 0;
			mask = new int[0,0];
		}

		public void setRectangle(Rectangle r, bool value)
		{
			for(int y=0;y<r.Height;y++)
				for(int x=0;x<r.Width;x++)
					setPoint(x+r.Left,y+r.Top,value);
		}

		public void setPoint(int x, int y, bool bValue)
		{
			if(bValue)
			{
				alterRectangleToInclude(x,y);
				_setPoint(mask,r,x,y,true);
			}
			else
			{
				if(r.Contains(x,y))
					_setPoint(mask,r,x,y,false);
			}
		}

		public bool getPoint(int x, int y)
		{
			if(!r.Contains(x,y))
				return false;
			else
				return _getPoint(mask,r,x,y);
		}

		public void updateGraphicsPath(GraphicsPath gp, int scale, int xofs, int yofs)
		{
			//todo: much optimization
			gp.Reset();
			Selection sHorz = new Selection(r.Width,r.Height*2);
			Selection sVert = new Selection(r.Width*2,r.Height);
			int xo = r.Left * scale;
			int yo = r.Top * scale;
			for(int y=0;y<r.Height;y++)
			{
				for(int x=0;x<r.Width;x++)
				{
					if(getPoint(x+r.Left,y+r.Top))
					{
						if(!getPoint(x-1+r.Left,y+r.Top))
						{
							gp.StartFigure();
							gp.AddLine(x*scale+xofs,y*scale+yofs,x*scale+xofs,y*scale+scale+yofs);
							gp.CloseFigure();
						}
						//sVert.setPoint(x*2,y,true);
						if(!getPoint(x+1+r.Left,y+r.Top))
						{
							gp.StartFigure();
							gp.AddLine(x*scale+scale+xofs,y*scale+yofs,x*scale+scale+xofs,y*scale+scale+yofs);
							gp.CloseFigure();
						}
						//sVert.setPoint(x*2+1,y,true);
						if(!getPoint(x+r.Left,y-1+r.Top))
						{
							gp.StartFigure();
							gp.AddLine(x*scale+xofs,y*scale+yofs,x*scale+scale+xofs,y*scale+yofs);
							gp.CloseFigure();
						}
							
						//	sHorz.setPoint(x,y*2,true);
						if(!getPoint(x+r.Left,y+1+r.Top))
						{
							gp.StartFigure();
							gp.AddLine(x*scale+xofs,y*scale+scale+yofs,x*scale+scale+xofs,y*scale+scale+yofs);
							gp.CloseFigure();
						}
							
						//	sHorz.setPoint(x,y*2+1,true);
					}
				}
			}
			Matrix m = new Matrix();
			m.Translate((float)xo,(float)yo);
			gp.Transform(m);
		}
	}

}
