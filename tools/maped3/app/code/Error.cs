using System;

namespace winmaped2
{
	/// <summary>
	/// Summary description for Error.
	/// </summary>
	public class Errors
	{
		public static void Error(string caption, string error)
		{
			System.Windows.Forms.MessageBox.Show(error,caption);
		}
		public static void Error(string error)
		{
			System.Windows.Forms.MessageBox.Show(error);
		}
	}
}
