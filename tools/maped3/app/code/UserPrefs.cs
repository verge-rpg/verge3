using System;
using System.Drawing;
using System.Collections;
using System.IO;
using System.Xml;

namespace winmaped2
{
	public class UserPrefs
	{
		public static int		DefaultZoomLevel	= 2;
		public static bool		AutoSave			= true;
		public static int		AutoSaveDelay		= 5; // 5 minutes
		public static int		AutoSaveLimit		= 20; // 20 backup maps
		public static int		ObsColor			= Color.White.ToArgb();
		public static int		ZonesColor			= Color.Blue.ToArgb();
		public static int		EntsColor			= Color.Yellow.ToArgb();



		private static string PrefFile = "MapedPreferences.xml";
		public static void Load()
		{

			XmlTextReader xtr = null;
			try
			{
				xtr = new XmlTextReader(PrefFile);

				xtr.ReadStartElement();

				xtr.ReadStartElement("DefaultZoomLevel");
				DefaultZoomLevel = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadStartElement("AutoSave");
				AutoSave = bool.Parse(xtr.ReadString());
				xtr.ReadEndElement();
			
				xtr.ReadStartElement("AutoSaveDelay");
				AutoSaveDelay = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadStartElement("AutoSaveLimit");
				AutoSaveLimit = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadStartElement("ObsColor");
				ObsColor = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadStartElement("ZonesColor");
				ZonesColor = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadStartElement("EntsColor");
				EntsColor = int.Parse(xtr.ReadString());
				xtr.ReadEndElement();

				xtr.ReadEndElement();
		
				xtr.Close();
			}
			catch
			{
				Errors.Error("Load", "There was an error loading your maped preferences. Using default settings.");
				xtr.Close();
			}


		}
		public static void Save()
		{
            XmlTextWriter xtw = new XmlTextWriter(PrefFile,null);

			xtw.WriteStartDocument();
			xtw.WriteStartElement("MapedPreferences");

			xtw.WriteStartElement("DefaultZoomLevel");
			xtw.WriteString(DefaultZoomLevel.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("AutoSave");
			xtw.WriteString(AutoSave.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("AutoSaveDelay");
			xtw.WriteString(AutoSaveDelay.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("AutoSaveLimit");
			xtw.WriteString(AutoSaveLimit.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("ObsColor");
			xtw.WriteString(ObsColor.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("ZonesColor");
			xtw.WriteString(ZonesColor.ToString());
			xtw.WriteEndElement();

			xtw.WriteStartElement("EntsColor");
			xtw.WriteString(EntsColor.ToString());
			xtw.WriteEndElement();

			
			xtw.WriteEndElement();
			xtw.WriteEndDocument();

			xtw.Close();

		}
	}
}
