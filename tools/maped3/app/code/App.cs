using System;
using System.Windows.Forms;

namespace winmaped2 {
    /// <summary>
    /// Summary description for Main.
    /// </summary>
    public class App {
        [STAThread]
        public static int Main(string[] args) {

            MainWindow mw = new MainWindow();

            if (args.Length > 0) {
                mw.LoadMap(args[0]);
                mw.ui_update();
            }
            Application.Run(mw);
            return 0;
        }
    }
}
