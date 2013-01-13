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

        public bool disposeTime = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            notifyIcon1.BalloonTipTitle = "Minimize to Tray App";
            notifyIcon1.BalloonTipText = "You have successfully minimized your form.";

            if (FormWindowState.Minimized == this.WindowState) {
                notifyIcon1.Visible = true;
                notifyIcon1.ShowBalloonTip(500);
                this.Hide();
            }
            else if (FormWindowState.Normal == this.WindowState) {
                notifyIcon1.Visible = false;
            }
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            this.Show();
            this.WindowState = FormWindowState.Normal;
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
