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
            stripMenuBlink1Status.Text = "blink1 status: hello";
            if (blink1Server.blink1.getCachedCount() > 0) {
                stripMenuBlink1Status.Text = "blink(1) found";
                stripMenuBlink1Id.Text = "serial: " + blink1Server.blink1.getCachedSerial(0);
                stripMenuKey.Text = "key: "+ blink1Server.blink1.blink1Id;
            }
            else {
                stripMenuBlink1Status.Text = "blink(1) not found";
                stripMenuBlink1Id.Text = "serial: -none-";
                stripMenuKey.Text = "key: " + blink1Server.blink1.blink1Id;
            }
        }

        private void stripMenuResetAlerts_Click(object sender, EventArgs e)
        {

        }
        private void stripMenuExit_Click(object sender, EventArgs e)
        {
            Close();
            doExit();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Console.WriteLine("FormClosed!");
            doExit();
        }

        private void Awesomium_Windows_Forms_WebControl_Resize(object sender, EventArgs e)
        {
            Console.WriteLine("HELLO THERE");
        }

        private void doExit()
        {
            blink1Server.shutdown();
            System.Windows.Forms.Application.Exit();
            System.Diagnostics.Process.GetCurrentProcess().Kill();
        }

    }
}
