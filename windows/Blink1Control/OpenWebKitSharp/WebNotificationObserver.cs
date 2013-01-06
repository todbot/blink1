using System;
using System.Collections.Generic;
using System.Text;
using WebKit;
using WebKit.Interop;

namespace WebKit
{
    internal delegate void OnNotifyEvent(IWebNotification notification);

    internal class WebNotificationObserver : IWebNotificationObserver
    {
        public event OnNotifyEvent OnNotify = delegate { };

        #region webNotificationObserver Members

        public void onNotify(IWebNotification notification)
        {
            OnNotify(notification);
        }

        #endregion
    }
}
