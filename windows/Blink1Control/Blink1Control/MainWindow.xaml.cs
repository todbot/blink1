using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Web;
using System.Net;

namespace Blink1Control
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Blink1Server blink1Server = new Blink1Server();

        public bool disposeTime = false;

        public MainWindow()
        {
            InitializeComponent();

            this.webControl1.Source = new System.Uri("http://127.0.0.1:8934/blink_1/", System.UriKind.Absolute);

            MinimizeToTray.Enable(this);

        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (disposeTime) {
                //close it
                blink1Server.shutdown();
                // FIXME: is this the right thing to do?
                System.Windows.Forms.Application.Exit();
                System.Diagnostics.Process.GetCurrentProcess().Kill();
            }
            else {
                base.OnClosing(e);
                e.Cancel = true;
                this.WindowState = WindowState.Minimized;
            }
        }
    }
}
