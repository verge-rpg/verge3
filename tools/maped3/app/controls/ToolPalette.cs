using System;
using System.Windows.Forms;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;



namespace winmaped2
{
	public class ToolPalette : Panel
	{
		public class ToolButton : RadioButton
		{
			public Guid guid;
		}

		private Plugins.IMapPlugin _currMapPlugin = null;
		public Plugins.IMapPlugin currMapPlugin
		{
			get
			{
				return _currMapPlugin;
			}
		}

        Hashtable tools = new Hashtable();
		public ToolPalette() : base()
		{
		}
		public delegate void ToolEventHandler(ToolEventArgs e);
		public event ToolEventHandler SelectedPluginChanged;

		public void registerButton(ToolButton tb, Plugins.IMapPlugin mapPlugin)
		{
			tb.guid = mapPlugin.guid;
			tools[mapPlugin.guid] = mapPlugin;
			tb.CheckedChanged += new EventHandler(toolchanged);
		}

		public void registerPlugin(Plugins.IMapPlugin mapPlugin, Guid guid)
		{
			tools[guid] = mapPlugin;
		}

		private void toolchanged(object sender, EventArgs e)
		{
			ToolButton tb = (ToolButton)sender;
			if(!tb.Checked) return;
			_currMapPlugin = (Plugins.IMapPlugin)tools[tb.guid];
			if(SelectedPluginChanged != null)
				SelectedPluginChanged(new ToolEventArgs(_currMapPlugin));
		}
	}
	public class ToolEventArgs
	{
		public Plugins.IMapPlugin selectedPlugin;
		public ToolEventArgs(Plugins.IMapPlugin imp)
		{
			selectedPlugin=imp;
		}
	}
}
