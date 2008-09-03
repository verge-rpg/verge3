using System;

using System.IO;

using NUnit.Framework;
using winmaped2;
using winmaped2.pr2;

namespace winmaped2.Tests
{
    [TestFixture]
    public class FormatTests
    {
        public FormatTests()
        {
            //setup.
            if( Environment.CurrentDirectory.Contains(@"\bin\Debug") ) {
                Environment.CurrentDirectory = Environment.CurrentDirectory + @"\..\..\app\";
            }
        }

        [Test]
        public void LoadMap()
        {
            string path = "city01.map";

            Assert.IsTrue( File.Exists(path) );
         
            InputOutput.ReadMap( path );
        }

        [Test]
        public void SaveMap()
        {
            String loadPath = "city01.map";
            String savePath = "city01-saved.map";

            Assert.IsTrue(File.Exists(loadPath), "'" + loadPath + "' should exist, but doesn't.");
            Assert.IsFalse(File.Exists(savePath), "'" + savePath + "' should exist, but does.");

            Map myMap = InputOutput.ReadMap(loadPath);

            FileInfo saveFileInfo = new FileInfo(savePath);

            InputOutput.WriteMap(saveFileInfo, myMap);

            Assert.IsTrue(File.Exists(savePath));

            File.Delete(savePath);
        }

        [Test]
        public void LoadThenSaveThenLoadMap()
        {
            String loadPath = "city01.map";
            String savePath = "city01-saved.map";

            Assert.IsTrue(File.Exists(loadPath), "'" + loadPath + "' should exist, but doesn't.");
            Assert.IsFalse(File.Exists(savePath), "'" + savePath + "' should exist, but does.");

            Map myMap = InputOutput.ReadMap(loadPath);

            FileInfo saveFileInfo = new FileInfo(savePath);

            InputOutput.WriteMap(saveFileInfo, myMap);

            Assert.IsTrue(File.Exists(savePath));

            myMap = InputOutput.ReadMap(savePath);

            File.Delete(savePath);
        }
    }
}
