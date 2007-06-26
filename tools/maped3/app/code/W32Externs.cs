using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace Win32Util
{
	/// <summary>
	/// Encapsulates window functions that aren't in the framework.
	/// NOTE: This class is not thread-safe. 
	/// </summary>
	public class Win32Window
	{
		IntPtr window;
		ArrayList windowList = null;
		static ArrayList topLevelWindows = null;

		/// <summary>
		/// Create a Win32Window
		/// </summary>
		/// <param name="window">The window handle</param>
		public Win32Window(IntPtr window)
		{
			this.window = window;
		}

		/// <summary>
		/// Extract the window handle 
		/// </summary>
		public IntPtr Window
		{
			get
			{
				return window;
			}
		}

		/// <summary>
		/// Return true if this window is null
		/// </summary>
		public bool IsNull
		{
			get
			{
				return window == IntPtr.Zero;
			}
		}

		/// <summary>
		/// The children of this window, as an ArrayList
		/// </summary>
		public ArrayList Children
		{
			get
			{
				windowList = new ArrayList();
				EnumChildWindows(window, new EnumWindowsProc(EnumerateChildProc), 0);
				ArrayList children = windowList;
				windowList = null;
				return children;
			}
		}

		bool EnumerateChildProc(IntPtr window, int i)
		{
			windowList.Add(new Win32Window(window));
			return(true);
		}

		/// <summary>
		/// All top level windows 
		/// </summary>
		public static ArrayList TopLevelWindows
		{
			get
			{
				topLevelWindows = new ArrayList();
				EnumWindows(new EnumWindowsProc(EnumerateTopLevelProc), 0);
				ArrayList top = topLevelWindows;
				topLevelWindows = null;
				return top;
			}
		}

		static bool EnumerateTopLevelProc(IntPtr window, int i)
		{
			topLevelWindows.Add(new Win32Window(window));
			return(true);
		}

		/// <summary>
		/// Return all windows of a given thread
		/// </summary>
		/// <param name="threadId">The thread id</param>
		/// <returns></returns>
		public static ArrayList GetThreadWindows(int threadId)
		{
			topLevelWindows = new ArrayList();
			EnumThreadWindows(threadId, new EnumWindowsProc(EnumerateThreadProc), 0);
			ArrayList windows = topLevelWindows;
			topLevelWindows = null;
			return windows;
		}

		/// <summary>
		/// The deskop window
		/// </summary>
		public static Win32Window DesktopWindow
		{
			get
			{
				return new Win32Window(GetDesktopWindow());
			}
		}

		/// <summary>
		/// The current foreground window
		/// </summary>
		public static Win32Window ForegroundWindow
		{
			get
			{
				return new Win32Window(GetForegroundWindow());
			}
		}

		static bool EnumerateThreadProc(IntPtr window, int i)
		{
			topLevelWindows.Add(new Win32Window(window));
			return(true);
		}

		/// <summary>
		/// Bring a window to the top
		/// </summary>
		public void BringWindowToTop()
		{
			BringWindowToTop(window);
		}

		/// <summary>
		/// Find a child of this window
		/// </summary>
		/// <param name="className">Name of the class, or null</param>
		/// <param name="windowName">Name of the window, or null</param>
		/// <returns></returns>
		public Win32Window FindChild(string className, string windowName)
		{
			return new Win32Window(
				FindWindowEx(window, IntPtr.Zero, className, windowName));
		}

		/// <summary>
		/// Find a window by name or class
		/// </summary>
		/// <param name="className">Name of the class, or null</param>
		/// <param name="windowName">Name of the window, or null</param>
		/// <returns></returns>
		public static Win32Window FindWindow(string className, string windowName)
		{
			return new Win32Window(FindWindowWin32(className, windowName));
		}

		/// <summary>
		/// Tests whether one window is a child of another
		/// </summary>
		/// <param name="parent">Parent window</param>
		/// <param name="window">Window to test</param>
		/// <returns></returns>
		public static bool IsChild(Win32Window parent, Win32Window window)
		{
			return IsChild(parent.window, window.window);
		}

		/// <summary>
		/// Send a windows message to this window
		/// </summary>
		/// <param name="message"></param>
		/// <param name="wparam"></param>
		/// <param name="lparam"></param>
		/// <returns></returns>
		public int SendMessage(int message, int wparam, int lparam)
		{
			return SendMessage(window, message, wparam, lparam);
		}

		/// <summary>
		/// Post a windows message to this window
		/// </summary>
		/// <param name="message"></param>
		/// <param name="wparam"></param>
		/// <param name="lparam"></param>
		/// <returns></returns>
		public int PostMessage(int message, int wparam, int lparam)
		{
			return PostMessage(window, message, wparam, lparam);
		}

		/// <summary>
		/// Get the parent of this window. Null if this is a top-level window
		/// </summary>
		public Win32Window Parent
		{
			get
			{
				return new Win32Window(GetParent(window));
			}
		}

		/// <summary>
		/// Get the last (topmost) active popup
		/// </summary>
		public Win32Window LastActivePopup
		{
			get
			{
				IntPtr popup = GetLastActivePopup(window);
				if (popup == window)
					return new Win32Window(IntPtr.Zero);
				else
					return new Win32Window(popup);
			}
		}

		/// <summary>
		/// The text in this window
		/// </summary>
		public string Text
		{
			get
			{
				int length = GetWindowTextLength(window);
				StringBuilder sb = new StringBuilder(length + 1);
				GetWindowText(window, sb, sb.Capacity);
				return sb.ToString();				
			}
			set
			{
				SetWindowText(window, value);
			}
		}

		/// <summary>
		/// Get a long value for this window. See GetWindowLong()
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		public int GetWindowLong(int index)
		{
			return GetWindowLong(window, index);
		}

		/// <summary>
		/// Set a long value for this window. See SetWindowLong()
		/// </summary>
		/// <param name="index"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		public int SetWindowLong(int index, int value)
		{
			return SetWindowLong(window, index, value);
		}

		/// <summary>
		/// The id of the thread that owns this window
		/// </summary>
		public int ThreadId
		{
			get
			{
				return GetWindowThreadProcessId(window, IntPtr.Zero );
			}
		}

		/// <summary>
		/// The id of the process that owns this window
		/// </summary>
		public int ProcessId
		{
			get
			{
				int processId = 0;
				GetWindowThreadProcessId(window, ref processId);
				return processId;
			}
		}

		/// <summary>
		/// The placement of this window
		/// </summary>
		public WindowPlacement WindowPlacement
		{
			get
			{
				WindowPlacement placement = new WindowPlacement();
				GetWindowPlacement(window, ref placement);
				return placement;
			}
		}

		/// <summary>
		/// Whether the window is minimized
		/// </summary>
		public bool Minimized
		{
			get
			{
				return IsIconic(window);
			}
		}

		/// <summary>
		/// Whether the window is maximized
		/// </summary>
		public bool Maximized
		{
			get
			{
				return IsZoomed(window);
			}
		}

		/// <summary>
		/// Turn this window into a tool window, so it doesn't show up in the Alt-tab list...
		/// </summary>
		/// 
		const int GWL_EXSTYLE = -20;
		const int WS_EX_TOOLWINDOW = 0x00000080;
		const int WS_EX_APPWINDOW = 0x00040000;

		public void MakeToolWindow()
		{
			int windowStyle = GetWindowLong(GWL_EXSTYLE);
			SetWindowLong(GWL_EXSTYLE, windowStyle | WS_EX_TOOLWINDOW);
		}
		public static IntPtr GetActiveWin()
		{
			return GetActiveWindow();
		}

		[DllImport("user32.dll")]
		static extern bool BringWindowToTop(IntPtr window);
		
		[DllImport("user32.dll")]
		static extern IntPtr FindWindowEx(IntPtr parent, IntPtr childAfter, string className, string windowName);

		[DllImport("user32.dll", EntryPoint="FindWindow")]
		static extern IntPtr FindWindowWin32(string className, string windowName);

		[DllImport("user32.dll")]
		static extern int SendMessage(IntPtr window, int message, int wparam, int lparam);

		[DllImport("user32.dll")]
		static extern int PostMessage(IntPtr window, int message, int wparam, int lparam);

		[DllImport("user32.dll")]
		static extern IntPtr GetParent(IntPtr window);

		[DllImport("user32.dll")]
		static extern IntPtr GetDesktopWindow();

		[DllImport("user32.dll")]
		static extern IntPtr GetForegroundWindow();

		[DllImport("user32.dll")]
		static extern IntPtr GetLastActivePopup(IntPtr window);

		[DllImport("user32.dll")]
		static extern int GetWindowText(
			IntPtr window,
			[In][Out] StringBuilder text,
			int copyCount);

		[DllImport("user32.dll")]
		static extern bool SetWindowText(
			IntPtr window,
			[MarshalAs(UnmanagedType.LPTStr)]
			string text);

		[DllImport("user32.dll")]
		static extern int GetWindowTextLength(IntPtr window);

		[DllImport("user32.dll")]
		static extern int SetWindowLong(
			IntPtr window,
			int index,
			int value);

		[DllImport("user32.dll")]
		static extern int GetWindowLong(
			IntPtr window,
			int index);



		// BOOL CALLBACK EnumWindowsProc(
		//				  HWND hwnd,      // handle to parent window
		//				  LPARAM lParam   // application-defined value
		//		
		delegate bool EnumWindowsProc(
			IntPtr window, int i);


		// BOOL EnumWindows(
		//	WNDENUMPROC lpEnumFunc,  // callback function
		//	LPARAM lParam            // application-defined value
		//	);


		[DllImport("user32.dll")]
		static extern IntPtr GetActiveWindow();

		[DllImport("user32.dll")]
		static extern bool EnumChildWindows(
			IntPtr window, EnumWindowsProc callback, int i);

		[DllImport("user32.dll")]
		static extern bool EnumThreadWindows(
			int threadId, EnumWindowsProc callback, int i);

		[DllImport("user32.dll")]
		static extern bool EnumWindows(EnumWindowsProc callback, int i);

		[DllImport("user32.dll")]
		static extern int GetWindowThreadProcessId(IntPtr window, ref int processId);

		[DllImport("user32.dll")]
		static extern int GetWindowThreadProcessId(IntPtr window, IntPtr ptr);

		[DllImport("user32.dll")]
		static extern bool GetWindowPlacement(IntPtr window, ref WindowPlacement position);

		[DllImport("user32.dll")]
		static extern bool IsChild(IntPtr parent, IntPtr window);

		[DllImport("user32.dll")]
		static extern bool IsIconic(IntPtr window);

		[DllImport("user32.dll")]
		static extern bool IsZoomed(IntPtr window);

		[DllImport("user32.dll")]
		private static extern IntPtr GetWindowDC(IntPtr hwnd);
		
		[DllImport("user32.dll")]
		private static extern bool GetWindowRect(IntPtr hwnd, ref Rectangle rectangle);
		
		[DllImport("gdi32.dll")]
		private static extern UInt64 BitBlt
			(IntPtr hDestDC, int x, int y, int nWidth, int nHeight,
			IntPtr hSrcDC, int xSrc, int ySrc, System.Int32 dwRop);
		
		public static Image DesktopAsBitmap
		{
			get
			{
				Image myImage = new Bitmap(Screen.PrimaryScreen.Bounds.Width,
					Screen.PrimaryScreen.Bounds.Height);
				Graphics gr1 = Graphics.FromImage(myImage);
				IntPtr dc1 = gr1.GetHdc();
				IntPtr dc2 = GetWindowDC(GetDesktopWindow());
				BitBlt(dc1, 0, 0, Screen.PrimaryScreen.Bounds.Width,
					Screen.PrimaryScreen.Bounds.Height, dc2, 0, 0, SRCCOPY); 
				gr1.ReleaseHdc(dc1);
				return myImage;
			}
		}
		static int SRCCOPY = 0x00CC0020;  // dest = source 

		public Image WindowAsBitmap
		{
			get
			{
				if (IsNull)
					return null;

				this.BringWindowToTop();
				System.Threading.Thread.Sleep(500);

				Rectangle rect = new Rectangle();
				if (!GetWindowRect(window, ref rect))
					return null;
				
				Image myImage = new Bitmap(rect.Width, rect.Height);
				Graphics gr1 = Graphics.FromImage(myImage);
				IntPtr dc1 = gr1.GetHdc();
				IntPtr dc2 = GetWindowDC(window);
				BitBlt(dc1, 0, 0, rect.Width, rect.Height, dc2, 0, 0, SRCCOPY); 
				gr1.ReleaseHdc(dc1);
				return myImage;

			}
		}

	}
}

public struct WindowPlacement
{
	public int length;
	public int flags;
	public int showCmd;
	public Point minPosition;
	public Point maxPosition;
	public Rectangle normalPosition;
}
