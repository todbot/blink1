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
        private Boolean showedBaloon = false;
        bool mAllowVisible;     // ContextMenu's Show command used
        bool mAllowClose;       // ContextMenu's Exit command used
        bool mLoadFired;        // Form was shown once

        public Form1()
        {
            InitializeComponent();

            BrowserSettings bs = new BrowserSettings();
            bs.WebGlDisabled = true;
            bs.PluginsDisabled = true;
            web_view = new WebView("http://127.0.0.1:8934/blink_1/", bs);
            web_view.Dock = DockStyle.Fill;
            web_view.RequestHandler = this;
            this.Controls.Add(web_view);

            mAllowVisible = !Blink1Server.startMinimized;
            stripMenuStartMinimized.Checked = !mAllowVisible;

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            /*
            Blink1Server.Log("Load****");
            // no smaller than design time size
            //this.MinimumSize = new System.Drawing.Size(this.Width, this.Height);
            // no larger than screen size
            //this.MaximumSize = new System.Drawing.Size((int)System.Windows.SystemParameters.PrimaryScreenWidth, (int)System.Windows.SystemParameters.PrimaryScreenHeight);
            this.MinimumSize = new Size(800, 600);
            this.MaximumSize = new Size(800, 600);
            this.AutoSize = true;
            this.AutoSizeMode = AutoSizeMode.GrowAndShrink;
            */
            // rest of your code here...
        }
        //"Form Shown" event handler
        private void Form_Shown(object sender, EventArgs e)
        {
            //to minimize window
            this.WindowState = FormWindowState.Minimized;

            //to hide from taskbar
            this.Hide();
        }

        // to allow to start minimized
        // see: http://stackoverflow.com/questions/1730731/how-to-start-winform-app-minimized-to-tray

        protected override void SetVisibleCore(bool value)
        {
            if (!mAllowVisible) value = false;
            base.SetVisibleCore(value);
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (!mAllowClose) {
                this.Hide();
                e.Cancel = true;
            }
            base.OnFormClosing(e);
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            notifyIcon1.BalloonTipTitle = "Blink1Control";
            notifyIcon1.BalloonTipText = "Alerts are running. Double-click to show settings.";

            if (FormWindowState.Minimized == WindowState) {
                Hide();
                notifyIcon1.Visible = true;
                if (!showedBaloon) {
                    notifyIcon1.ShowBalloonTip(500);
                    showedBaloon = true;
                }
            }
            else if (FormWindowState.Normal == this.WindowState) {
                //notifyIcon1.Visible = false;
            }
        }

        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            mAllowVisible = true;
            mLoadFired = true;
            Show();
            WindowState = FormWindowState.Normal;
        }

        private void notifyIcon1_Click(object sender, EventArgs e)
        {
            Blink1Server.Log("notifyIcon1 SingleClick! " + e);
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

        private void stripMenuStartMinimized_Click(object sender, EventArgs e)
        {
            Blink1Server.startMinimized = stripMenuStartMinimized.Checked;
            mAllowVisible = !Blink1Server.startMinimized;
            blink1Server.saveSettings();
        }

        private void stripMenuResetAlerts_Click(object sender, EventArgs e)
        {
            blink1Server.resetAlerts();
        }

        private void stripMenuExit_Click(object sender, EventArgs e)
        {
            mAllowClose = mAllowVisible = true;
            if (!mLoadFired) Show();

            Close();
            doExit();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Blink1Server.Log("FormClosed!");
            doExit();
        }

        private void doExit()
        {
            blink1Server.shutdown();
            notifyIcon1.Visible = false;
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
