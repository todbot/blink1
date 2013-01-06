using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;

namespace Blink1Control
{
    // this is from justin's work
    // should also see: http://www.codeproject.com/Articles/27599/Minimize-window-to-system-tray
    // and: http://stackoverflow.com/questions/2708371/how-can-i-have-my-windows-form-application-minimize-to-system-taskbar

    /// <summary>
    /// Class implementing support for "minimize to tray" functionality.
    /// </summary>
    public static class MinimizeToTray
    {
        /// <summary>
        /// Enables "minimize to tray" behavior for the specified Window.
        /// </summary>
        /// <param name="window">Window to enable the behavior for.</param>
        public static void Enable(MainWindow window)
        {
            // No need to track this instance; its event handlers will keep it alive
            new MinimizeToTrayInstance(window);
        }

        /// <summary>
        /// Class implementing "minimize to tray" functionality for a Window instance.
        /// </summary>
        private class MinimizeToTrayInstance
        {
            private MainWindow _window;
            private NotifyIcon _notifyIcon;
            private System.Windows.Forms.MenuItem menuItem1;

            private System.Windows.Forms.MenuItem menuItem2;
            /// <summary>
            /// Initializes a new instance of the MinimizeToTrayInstance class.
            /// </summary>
            /// <param name="window">Window instance to attach to.</param>
            public MinimizeToTrayInstance(MainWindow window)
            {
                Debug.Assert(window != null, "window parameter is null.");
                _window = window;
                _window.StateChanged += new EventHandler(HandleStateChanged);
                // Initialize menuItem1 this.menuItem1.Index = 0;
                this.menuItem1 = new MenuItem();
                this.menuItem2 = new MenuItem();
                this.menuItem1.Text = "E&xit";
                this.menuItem1.Click += new System.EventHandler(this.menuItem1_Click);
                this.menuItem2.Text = "Settings";
                this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
            }

            /// <summary>
            /// Handles the Window's StateChanged event.
            /// </summary>
            /// <param name="sender">Event source.</param>
            /// <param name="e">Event arguments.</param>
            private void HandleStateChanged(object sender, EventArgs e)
            {
                if (_notifyIcon == null) {
                    // Initialize NotifyIcon instance "on demand"
                    _notifyIcon = new NotifyIcon();
                    _notifyIcon.Icon = Icon.ExtractAssociatedIcon(Assembly.GetEntryAssembly().Location);
                    _notifyIcon.MouseDoubleClick += new MouseEventHandler(HandleNotifyIconOrBalloonClicked);
                    _notifyIcon.BalloonTipClicked += new EventHandler(HandleNotifyIconOrBalloonClicked);
                    _notifyIcon.ContextMenu = new ContextMenu();
                    _notifyIcon.ContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] { menuItem1, menuItem2 });
                    _notifyIcon.Visible = true;
                }
                // Update copy of Window Title in case it has changed
                _notifyIcon.Text = _window.Title;

                // Show/hide Window and NotifyIcon
                var minimized = (_window.WindowState == WindowState.Minimized);
                //_window.ShowInTaskbar = true;
                _window.ShowInTaskbar = !minimized;
                //_notifyIcon.Visible = minimized;
                if (minimized) {
                    _notifyIcon.ShowBalloonTip(1000, null, _window.Title, ToolTipIcon.None);
                }
            }


            /// <summary>
            /// Handles a click on the notify icon or its balloon.
            /// </summary>
            /// <param name="sender">Event source.</param>
            /// <param name="e">Event arguments.</param>
            private void HandleNotifyIconOrBalloonClicked(object sender, EventArgs e)
            {
                // Restore the Window
                _window.WindowState = WindowState.Normal;
            }
            private void menuItem1_Click(object Sender, EventArgs e)
            {
                // Close the form, which closes the application. 
                _window.disposeTime = true;
                _window.Close();
            }
            private void menuItem2_Click(object Sender, EventArgs e)
            {
                // Restore the Window and reload
                _window.WindowState = WindowState.Normal;
                //
            }
        }
    }
}
