using System;
using System.Windows;
using System.Windows.Forms;
using System.ComponentModel;
using System.Web;
using System.Net;

namespace blink1
{
    public partial class Window1 : Window
    {
        private static System.Threading.AutoResetEvent listenForNextRequest = new System.Threading.AutoResetEvent(false);

        public HttpListener _httpListener = new HttpListener();
        public bool disposeTime = false;
        public Window1()
        {
            try
            {
                InitializeComponent();
                //_httpListener.Prefixes.Add("http://*:1234/");
                //_httpListener.Realm = "/html/blink_1/index.html";
                //_httpListener.Start();
               // System.Threading.ThreadPool.QueueUserWorkItem(Listen);
                //System.Threading.Thread.Sleep(1);
                //System.Windows.Forms.Application.DoEvents();
                //webView.LoadURL("/html/index.html");
                // Enable "minimize to tray" behavior for this Window
                MinimizeToTray.Enable(this);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
        protected override void OnClosing(CancelEventArgs e)
        {
            if (disposeTime)
            {
                //close it
            }
            else{
            base.OnClosing(e);
            e.Cancel = true;
            this.WindowState = WindowState.Minimized;
            }
        }
        private void ListenerCallback(IAsyncResult result)
        {
            HttpListener listener = result.AsyncState as HttpListener;
            HttpListenerContext context = null;

            if (listener == null)
                // Nevermind 
                return;

            try
            {
                context = listener.EndGetContext(result);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
                return;
            }
            finally
            {
                listenForNextRequest.Set();
            }
            if (context == null)
                return;
            ProcessRequest(context);
        }

        // Loop here to begin processing of new requests. 
        private void Listen(object state)
        {
            while (_httpListener.IsListening)
            {
                _httpListener.BeginGetContext(new AsyncCallback(ListenerCallback), _httpListener);
                listenForNextRequest.WaitOne();
            }
        }


        protected void ProcessRequest(HttpListenerContext context)
        {
            //content hooks
        }
    }
}
