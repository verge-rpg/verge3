using System;
using System.Drawing;
using System.Collections;
using System.IO;
using System.Xml.Serialization;
using System.Xml;

namespace winmaped2
{
    public class Preferences
    {
        public int DefaultZoomLevel;
        public bool AutoSave;

        public int AutoSaveDelay;
        public int AutoSaveLimit;

        public int ObsColor;
        public int ZonesColor;
        public int EntsColor;

        public string WorkingDirectory;

        /*
        public bool ToolPanelCollapsed;
        public bool ToolPanelHidden;        
         */

        private static Preferences current;
        /// <summary>
        /// Gets or sets the current preference set
        /// </summary>
        public static Preferences Current
        {
            get
            {
                if (current == null)
                    current = new Preferences();
                return current;
            }
            set
            {
                current = value;
            }
        }

        /// <summary>
        /// Creates a Preferences object with the default values
        /// </summary>
        public Preferences()
        {

            this.DefaultZoomLevel = 2;
            this.AutoSave = true;
            this.AutoSaveDelay = 5; // 5 minutes
            this.AutoSaveLimit = 20; // 20 backup maps
            this.ObsColor = Color.White.ToArgb();
            this.ZonesColor = Color.Blue.ToArgb();
            this.EntsColor = Color.Yellow.ToArgb();
            this.WorkingDirectory = Environment.CurrentDirectory;

        }

        private static string PrefFile
        {
            get
            {
                return "MapedPreferences.xml";
            }
        }

        public static void LoadAsCurrent()
        {
            current = Load();
        }
        public static Preferences Load()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Preferences));
            Preferences loadedPref;

            try
            {

                using (StreamReader prefReader = new StreamReader(PrefFile))
                {
                    loadedPref = (Preferences)serializer.Deserialize(prefReader);



                }
            }
            catch (IOException)
            {
                Errors.Error("Couldn't load preferences file, using defaults");
                loadedPref = new Preferences();
            }
            catch (InvalidOperationException)
            {
                Errors.Error("Invalid preferences file, loading defaults");
                loadedPref = new Preferences();
            }

            return loadedPref;
        }
        public void Save()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Preferences));
            StreamWriter prefWriter = new StreamWriter(PrefFile);

            serializer.Serialize(prefWriter, current);

            prefWriter.Close();
        }
    }
}
