using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using RegisterFileType;
using System.Threading;
using System.Diagnostics;

namespace winmaped2
{
	/// <summary>
	/// Summary description for Main.
	/// </summary>
	public class App
	{
        [STAThread]		
		public static int Main(string[] args)
		{

			MainWindow mw = new MainWindow();

			if(args.Length>0)
			{
				mw.LoadMap(args[0]);
				mw.ui_update();
			}
			Application.Run(mw);
			return 0;
		}
	}
}
