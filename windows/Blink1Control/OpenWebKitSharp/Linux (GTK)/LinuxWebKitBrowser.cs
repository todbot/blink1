#if LINUX
using System;
using WebKit.Linux;
using Gtk;

namespace WebKit.Linux
{
	public class LinuxWebKitBrowser : WebKitBrowser
	{
		internal GtkReparentingWrapperNoThread linuxwrapper;
		public LinuxWebKitBrowser()
		{
			linuxwrapper = new GtkReparentingWrapperNoThread(new Window(WindowType.Popup), this);
            webView.setHostWindow(linuxwrapper.BrowserWindow.Handle.ToInt32());
		}
        public Window GetWindow()
        {
            return linuxwrapper.BrowserWindow;
        }
		protected override void OnHandleCreated (EventArgs e)
		{
			linuxwrapper.Init ();
			base.OnHandleCreated (e);
		}
		protected override void OnResize (EventArgs e)
		{
			linuxwrapper.BrowserWindow.Resize (base.Width, base.Height );
			base.OnResize (e);
		}
        protected override void OnHandleDestroyed(EventArgs e)
        {
            linuxwrapper.BrowserWindow.Destroy();
            linuxwrapper.Dispose();
            base.OnHandleDestroyed(e);
        }
		protected override void OnGotFocus (EventArgs e)
		{
			linuxwrapper.BrowserWindow.GrabFocus();
			base.OnGotFocus (e);
		}
	}
}

#endif