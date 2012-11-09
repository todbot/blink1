
#if DEBUG || RELEASE
using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using WebKit.JSCore;
using System.Threading;
using WebKit.Interop;

namespace WebKit
{
    public class ConsoleEventArgs : EventArgs
    {
        public string Message { get; internal set; }
        public string Url { get; internal set; }
        public int LineNumber { get; internal set; }
        public bool IsError { get; internal set; }
        public ConsoleEventArgs(string message, string url, int error, int linenumber)
        {
            Message = message;
            Url = url;
            IsError = Convert.ToBoolean(error);
            LineNumber = linenumber;
        }
    }
    public delegate void ConsoleEventHandler(object sender, ConsoleEventArgs e);
    public class JSManagement
    {
        public event ConsoleEventHandler ConsoleMessageAdded = delegate { };
        WebKitBrowser _owner;
        public JSManagement(WebKitBrowser browser)
        {
            _owner = browser;
            IWebFrame fe = (IWebFrame)browser.webView.mainFrame();

            ScriptObject = new JSCore.JSContext(fe);
            browser.uiDelegate.AddMessageToConsole += new AddMessage(uiDelegate_AddMessageToConsole);
        }

        void uiDelegate_AddMessageToConsole(WebView sender, string mes, string u, int line, int error)
        {
            ConsoleMessageAdded(this, new ConsoleEventArgs(mes, u, error, line));
        }

        /// <summary>
        /// Returns the Global Context for JavaScript use.
        /// </summary>
        public JSCore.JSContext GlobalContext
        {
            get
            {
                return new JSCore.JSContext((IWebFrame)_owner.WebView.mainFrame());
            }
        }

        /// <summary>
        /// Calls a function with the specified arguments in the script environment.
        /// </summary>
        public JSValue CallFunction(string Name, object[] arguments)
        {
            JSCore.JSObject global = GlobalContext.GetGlobalObject();
            JSCore.JSValue window = global.GetProperty("window");
            if (window == null || !window.IsObject) 
                return null;
            JSCore.JSObject windowObj = window.ToObject();
            return windowObj.CallFunction(Name, arguments);
        }

        /// <summary>
        /// Executes a Script in the Global Context and returns its value.
        /// </summary>
        public JSCore.JSValue EvaluateScript(string Name)
        {
            JSCore.JSValue val = ((JSCore.JSContext)GlobalContext).EvaluateScript(Name);
            return val != null ? val : null;
        }

        internal void CreateWindowScriptObject(JSCore.JSContext context)
        {
            if (ScriptObject != null && context != null)
            {
                JSCore.JSObject global = context.GetGlobalObject();
                JSCore.JSValue window = global.GetProperty("window");
                if (window == null || !window.IsObject) return;
                JSCore.JSObject windowObj = window.ToObject();
                if (windowObj == null) return;
                windowObj.SetProperty("external", (object)ScriptObject);
            }
        }

        /// <summary>
        /// Gets or sets the Script Object that can be accessed for JavaScript use.
        /// </summary>
        /// <value>The object that can be used for scripting.</value>
        public object ScriptObject
        {
            get { return so; }
            set
            {
                so = value;
                CreateWindowScriptObject(GlobalContext);
            }
        }

        internal object so { get; set; }
    }
}
#endif