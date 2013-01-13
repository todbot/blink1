using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Blink1Control
{
    public partial class Form1 : Form
    {
        Blink1Server blink1Server = new Blink1Server();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            notifyIcon1.BalloonTipTitle = "Blink1Control";
            notifyIcon1.BalloonTipText = "Alerts are running. Double-click to show settings.";

            if (FormWindowState.Minimized == WindowState) {
                Hide();
                notifyIcon1.Visible = true;
                notifyIcon1.ShowBalloonTip(500);
            }
            else if (FormWindowState.Normal == this.WindowState) {
                notifyIcon1.Visible = false;
            }
        }

        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
        }

        private void notifyIcon1_Click(object sender, EventArgs e)
        {
            Console.WriteLine("notifyIcon1 SingleClick! "+e);
        }

        private void Awesomium_Windows_Forms_WebControl_Resize(object sender, EventArgs e)
        {
            Console.WriteLine("HELLO THERE");
        }

        /*
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
         */
    }
}
