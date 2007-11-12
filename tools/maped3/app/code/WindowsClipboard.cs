using System.Drawing;
using System.Windows.Forms;

namespace winmaped2
{

	public class WindowsClipboard
	{
		public static bool IsImage 
		{
			get 
			{ 
				IDataObject ido = System.Windows.Forms.Clipboard.GetDataObject();
				if(ido==null)
					return false;

				if(!ido.GetDataPresent("System.Drawing.Bitmap"))return false;
				return true;
			}
		}
		public static Bitmap getBitmap()
		{
			IDataObject ido = System.Windows.Forms.Clipboard.GetDataObject();
			if(ido==null)
				return null;

			if(!ido.GetDataPresent("System.Drawing.Bitmap"))
				return null;
		
			System.Drawing.Bitmap bmp = (System.Drawing.Bitmap)ido.GetData("System.Drawing.Bitmap");
			bmp = bmp.Clone(new Rectangle(0,0,bmp.Width,bmp.Height),System.Drawing.Imaging.PixelFormat.Format32bppArgb);

			return bmp;
		}

		public static pr2.RenderImage getImage()
		{
			Bitmap bmp = getBitmap();
			if(bmp == null)
				return null;
			else
				return pr2.RenderImage.create(bmp);
			
		}

		public static void setBitmap(Bitmap bmp)
		{
			DataObject dobj = new DataObject();
			dobj.SetData("System.Drawing.Bitmap",bmp);
			System.Windows.Forms.Clipboard.SetDataObject(dobj,true);
		}

		public static void setImage(pr2.RenderImage img)
		{
			Bitmap bmp = img.getBitmap();
			setBitmap(bmp);
			bmp.Dispose();
		}


	}
	
}