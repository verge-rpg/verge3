using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace winmaped2
{
	public class TileEditor : TileViewer
	{
		ColorPicker sourceLeft;
		ColorPicker sourceRight;

		public delegate void TileDataChangedEventHandler(int TileIndex);
		
		public event TileDataChangedEventHandler TileDataChanged;

		public ColorPicker SourceLeft
		{
			get { return sourceLeft; }
			set { sourceLeft = value; }
		}

		public ColorPicker SourceRight
		{
			get { return SourceRight; }
			set { sourceRight = value; }
		}

		public TileEditor()
			: base()
		{
			//this.BorderStyle = BorderStyle.Fixed3D;
		}

		protected Point TranslateToTileCoords(Point controlPoint)
		{
			int x = 0, y = 0;

			x = (int)((controlPoint.X / (float)Width) * 16);
			y = (int)((controlPoint.Y / (float)Height) * 16);

			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x > 15) x = 15;
			if (y > 15) y = 15;

			return new Point(x, y);
		}

		bool bLMousePressed = false,
			bRMousePressed = false;
		protected override void OnMouseDown(MouseEventArgs e)
		{
			Console.WriteLine("Mouse down...");
			//base.OnMouseDown(e);
			if (e.Button == MouseButtons.Left)
				bLMousePressed = true;
			else if (e.Button == MouseButtons.Right)
				bRMousePressed = true;
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			Console.WriteLine("Mouse up...");
			//base.OnMouseUp(e);
			if (e.Button == MouseButtons.Left)
				bLMousePressed = false;
			else if (e.Button == MouseButtons.Right)
				bRMousePressed = false;
		}

		protected unsafe override void OnMouseMove(MouseEventArgs e)
		{
			//base.OnMouseMove(e);
			if (bLMousePressed)
			{
				if (sourceLeft != null)
				{

					//sourceLeft.colorInfo.
					Point destCoord = TranslateToTileCoords(e.Location);
					Console.WriteLine("Draggin left.. ({0},{1})", destCoord.X, destCoord.Y);
                                    active_tile.Image.SetPixel(destCoord.X, destCoord.Y, pr2.Render.makeColor(SourceLeft.colorInfo.R, SourceLeft.colorInfo.G, SourceLeft.colorInfo.B));
					Invalidate(true);

					if (TileDataChanged != null)
						TileDataChanged(ActiveTileIndex);

					
				}
			}
			else if (bRMousePressed)
			{

			}
		}

		/*
		bool mdown=false;
		protected override void OnMouseDown(MouseEventArgs e)
		{
			// m-down
			mdown=true;
			
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			// m-up
			mdown=false;
		}
		protected override void OnMouseMove(MouseEventArgs e)
		{
			// m-move

		}*/
	}
}
