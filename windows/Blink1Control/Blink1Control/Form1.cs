using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using CefSharp;
using CefSharp.WinForms;


namespace Blink1Control
{
    public partial class Form1 : Form,IRequestHandler
    {
        Blink1Server blink1Server = new Blink1Server();

        private readonly WebView web_view;

        public Form1()
        {
            InitializeComponent();

            BrowserSettings bs = new BrowserSettings();
            Console.WriteLine("BrowserSettings: " + bs);
            bs.WebGlDisabled = true;
            bs.PluginsDisabled = true;
            //web_view = new WebView("http://stackoverflow.com", bs);
            web_view = new WebView("http://127.0.0.1:8934/blink_1/", bs);
            web_view.Dock = DockStyle.Fill;
            web_view.RequestHandler = this;
            this.Controls.Add(web_view);
            //containerControl1.Controls.Add(web_view);
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

        #region IRequestHandler Members

        bool IRequestHandler.OnBeforeBrowse(IWebBrowser browser, IRequest request,
                                   NavigationType naigationvType, bool isRedirect)
        {
            //System.Diagnostics.Debug.WriteLine("OnBeforeBrowse");
            return false;
        }

        bool IRequestHandler.OnBeforeResourceLoad(IWebBrowser browser,
                                    IRequestResponse requestResponse)
        {
            //System.Diagnostics.Debug.WriteLine("OnBeforeResourceLoad");
            //IRequest request = requestResponse.Request;
            return false;
        }

        void IRequestHandler.OnResourceResponse(IWebBrowser browser, string url,
                                       int status, string statusText,
                                       string mimeType, WebHeaderCollection headers)
        {
            //System.Diagnostics.Debug.WriteLine("OnResourceResponse");
        }

        #endregion
    }
}
