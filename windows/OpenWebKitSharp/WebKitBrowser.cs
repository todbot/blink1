/*
 * Copyright (c) 2009, Peter Nelson (charn.opcode@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 *   
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
*/

// Modification by GT Web Software
// Credits for the initial idea go to Peter Nelson


// credits to VBCoder for updated favicon code

// TODO: dispose / finalize stuff
//       design time support for properties etc..

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using WebKit;
using WebKit.DOM;
using WebKit.CSS;
using WebKit.Interop;
using WebKit.Appearance;
#if DOTNET4
using WebKit.JSCore;
#endif
using System.Reflection;
using System.IO;
using System.Drawing.Printing;
using System.Net;

namespace WebKit
{
    /// <summary>
    /// WebKit Browser Extended Control.
    /// </summary>
    public partial class WebKitBrowser : UserControl
    {
        // static variables
        internal static ActivationContext activationContext;
        private static int actCtxRefCount;

        // private member variables...
        internal IWebView webView;
        internal IntPtr webViewHWND;

        private bool disposed = false;
		
        // initialisation and property stuff
        private string initialText = string.Empty;
        private Uri initialUrl = null;
        private bool loaded = false;    // loaded == true -> webView != null
        private bool initialAllowNavigation = true;
        private bool initialAllowDownloads = true;
        private bool initialAllowNewWindows = true;
        private bool initialAllowCookies = true;
        private bool initialUsePipelining = true;
        private bool initialUseDefaultContextMenu = true;
        private Encoding initialEncoding = null;
#if DEBUG || RELEASE
        private JSManagement m = null;
#endif
        public WebKitCookieStorageAcceptPolicy CookiesPolicy = WebKitCookieStorageAcceptPolicy.WebKitCookieStorageAcceptPolicyAlways;
        private bool initialJavaScriptEnabled = true;
        private bool _contextMenuEnabled = true;
        private readonly Version version = Assembly.GetExecutingAssembly().GetName().Version;

        WebNotificationObserver progs;
        WebNotificationObserver progf;
        // delegates for WebKit events
        internal IWebFrameLoadDelegate frameLoadDelegate;
        internal WebDownloadDelegate downloadDelegate;
        internal WebPolicyDelegate policyDelegate;
        internal WebUIDelegate uiDelegate;
        internal WebFormDelegate formDelegate;
        internal WebNotificationObserver observer;
        internal WebNotificationCenter center;
        internal WebKitDOMCSSManager cssmanager;
        internal AppearanceSettings appearance;
        //internal WebEditingDelegate editingDelegate;
        internal ContextMenuManager contextmenumanager;
        internal WebResourceLoadDelegate resourcesLoadDelegate;
        internal ResourcesIntercepter resourceIntercepter;
        internal CustomUndoSystem undoManager;
        internal GlobalPreferences preferences;

        #region Extended properties

        public ContextMenuManager CustomContextMenuManager
        {
            get
            {
                return contextmenumanager;
            }
        }

        #region zoom
        public void SetPageZoom(float value)
        {
            WebView.setPageSizeMultiplier(value);
        }
        public void IncreaseZoom()
        {
            if (WebView.canZoomPageIn(this.GetWebViewAsObject()) == 1)
            {
                WebView.zoomPageIn(this);
            }
        }
        public void DecreaseZoom()
        {
            if (WebView.canZoomPageOut(this.GetWebViewAsObject()) == 1)
            {
                WebView.zoomPageOut(this);
            }
        }
        #endregion
        #region text zoom
        
        public void SetTextZoom(float value)
        {
            WebView.setTextSizeMultiplier(value);
        }
        public void SetPageZoom(int value)
        {
            WebView.setTextSizeMultiplier(Convert.ToSingle(value));
        }
        public void SetTextZoom(int value)
        {
            WebView.setTextSizeMultiplier(Convert.ToSingle(value));
        }
        public void IncreaseTextZoom()
        {
            if (WebView.canMakeTextLarger(this.GetWebViewAsObject()) == 1)
            {
                WebView.makeTextLarger(this);
            }
        }
        public void DecreaseTextZoom()
        {
            if (WebView.canMakeTextSmaller(this.GetWebViewAsObject()) == 1)
            {
                WebView.makeTextSmaller(this);
            }
        }
        public void ResetTextZoom()
        {
            WebView.resetPageZoom(this);
        }
        #endregion
        internal Element _el;
        public Image PageScreenshot
        {
            get
            {
                Bitmap b = new Bitmap(this.Width, this.Height);
                this.DrawToBitmap(b, new Rectangle(this.Location, this.Size));
                return (Image)b;
            }
        }


        public Element GetCurrentElement()
        {
            return _el;
        }

        public WebKitDOMCSSManager CSSManager
        {
            get { return cssmanager; }
        }

        public bool CanCopyLinkContents
        {
            get
            {
                tagPOINT p = new tagPOINT();
                Point labelOrigin = new Point(0, 0); // this is referencing the control
                Point screenOrigin = PointToScreen(labelOrigin); // this references
                
                p.x = Cursor.Position.X - screenOrigin.X;
                p.y = Cursor.Position.Y - screenOrigin.Y;
                object el;

                WebView.elementAtPoint(ref p).RemoteRead("WebElementLinkURLKey", out el, null, 0, null);
                return !String.IsNullOrEmpty((string)el);
            }
        }
        public bool CanCopyText
        {
            get
            {
                return !String.IsNullOrEmpty(WebView.selectedText());
            }
        }
        public void CopyLink()
        {
            tagPOINT p = new tagPOINT();
            Point labelOrigin = new Point(0, 0); // this is referencing the control
            Point screenOrigin = PointToScreen(labelOrigin); // this references
            p.x = Cursor.Position.X - screenOrigin.X;
            p.y = Cursor.Position.Y - screenOrigin.Y;
            object el;
            WebView.elementAtPoint(ref p).RemoteRead("WebElementLinkURLKey", out el, null, 0, null);
            if (!String.IsNullOrEmpty((string)el))
                Clipboard.SetText((string)el);
        }
        public void CopySelectedText()
        {
            if (CanCopyText)
                Clipboard.SetText(WebView.selectedText());
        }
        /// <summary>
        /// Returns the WebView object of the current browser.
        /// </summary>
        public WebViewClass WebView
        {
            get 
            {
                return ((WebViewClass)this.GetWebViewAsObject()); 
            }
        }

        /// <summary>
        /// This method shows the SaveAs Dialog to the user and saves th
        /// current page to the returned path if the dialog result is OK
        /// </summary>
        public void ShowSaveAsDialog()
        {
            using (SaveFileDialog sv = new SaveFileDialog())
            {
                sv.Filter = LanguageLoader.HTMLDoc + "|*.html;*.htm|" + LanguageLoader.AllFiles + "|*.*";

                if (sv.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    System.IO.File.WriteAllText(sv.FileName, DocumentText);
                }
            }
        }

        /// <summary>
        /// Navigates to a local file.
        /// </summary>
        public void OpenDocument(string path)
        {
            string tonav = "file:///" + path.Replace(" ", "%20").Replace(@"\", "/");
            Navigate(tonav);
            
        }


        private bool _pr = false;

        /// <summary>
        /// Private browsing will not save cookies and other data but you have to handle 
        /// history in your own way if you use history in your application.
        /// </summary>
        public bool PrivateBrowsing
        {
            get
            { return _pr; }
            set 
            {
                _pr = value; 
                try
                {
                    if (webView != null)
                    {
                        if (value == true)
                        {
                            WebView.preferences().setPrivateBrowsingEnabled(1);
                            WebView.preferences().setCookieStorageAcceptPolicy(WebKitCookieStorageAcceptPolicy.WebKitCookieStorageAcceptPolicyNever);
                        }
                        else
                        {
                            WebView.preferences().setPrivateBrowsingEnabled(0);
                            WebView.preferences().setCookieStorageAcceptPolicy(CookiesPolicy);
                        }
                    }
                }
                catch { }     // there are some problems with the cairo builds and that's why we use try catch
            }
        }
        #endregion

        #region Overridden methods

        /// <summary>
        /// Processes a command key.  Overridden in WebKitBrowser to forward key events to the WebKit window.
        /// </summary>
        /// <param name="msg">The window message to process.</param>
        /// <param name="keyData">The key to process.</param>
        /// <returns>Success value.</returns>
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Keys key = (Keys)msg.WParam.ToInt32();
            if (key == Keys.Left || key == Keys.Right || key == Keys.Up || 
                key == Keys.Down || key == Keys.Tab)
            {
                NativeMethods.SendMessage(webViewHWND, (uint)msg.Msg, msg.WParam, msg.LParam);
                return true;
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        #endregion

        #region WebKitBrowser events

        // public events, roughly the same as in WebBrowser class
        // using the null object pattern to avoid null tests
        /// <summary>
        /// Occurs when the DocumentTitle property value changes.
        /// </summary>
        public event EventHandler DocumentTitleChanged = delegate { };

        /// <summary>
        /// Occurs when a CloseWindowRequest is sent by the page.
        /// </summary>
        public event EventHandler CloseWindowRequest = delegate { };

        /// <summary>
        /// Occurs when headers for the URL that the WebKitBrowser attempts to navigate to are available
        /// </summary>
        public event HeadersAvailableEventHandler HeadersAvailable = delegate { };

        /// <summary>
        /// Occurs when it is detected that the current site is dangerous.
        /// </summary>
        public event EventHandler DangerousSiteDetected = delegate { };

        /// <summary>
        /// Occurs when a favicon is available for the current site.
        /// </summary>
        public event FaviconAvailable FaviconAvailable = delegate { };

        /// <summary>
        /// Occurs when the Status Text for this WebView is changed.
        /// </summary>
        public event StatusTextChanged StatusTextChanged = delegate { };

        public event GeolocationRequest GeolocationPositionRequest = delegate { };
        /// <summary>
        /// Occurs when the WebKitBrowser control has updated information on the download progress of a document it is navigating to.
        /// </summary>
        public event ProgressChangedEventHandler ProgressChanged = delegate { };
        /// <summary>
        /// Occurs when the WebKitBrowser control finishes loading a document.
        /// </summary>
        public event WebBrowserDocumentCompletedEventHandler DocumentCompleted = delegate { };

        /// <summary>
        /// Occurs when the WebKitBrowser control has navigated to a new document and has begun loading it.
        /// </summary>
        public event WebBrowserNavigatedEventHandler Navigated = delegate { };

        /// <summary>
        /// Occurs before the WebKitBrowser control navigates to a new document.
        /// </summary>
        public event WebKitBrowserNavigatingEventHandler Navigating = delegate { };

        /// <summary>
        /// Occurs when an error occurs on the current document, or when navigating to a new document.
        /// </summary>
        public event WebKitBrowserErrorEventHandler Error = delegate { };

        /// <summary>
        /// Occurs when the WebKitBrowser control requests that a download starts.
        /// </summary>
        public event FileDownloadBeginEventHandler DownloadBegin = delegate { };

        /// <summary>
        /// Occurs when a form is submitted.
        /// </summary>
        public event WillSubmitForm FormSubmit = delegate { };

        /// <summary>
        /// Occurs when text is changed in an HTML area.
        /// </summary>
        public event TextDidChangeInArea TextChangeInArea = delegate { };

        /// <summary>
        /// Occurs when text is changed in an HTML field.
        /// </summary>
        public event TextDidChangeInField TextChangeInField = delegate { };

        /// <summary>
        /// Occurs when starts writing in an HTML field.
        /// </summary>
        public event TextFieldDidBeginEditing TextFieldBeginEditing = delegate { };

        /// <summary>
        /// Occurs when finishes writing in an HTML field.
        /// </summary>
        public event TextFieldDidEndEditing TextFieldEndEditing = delegate { };

        /// <summary>
        /// Occurs when CanGoBack changes.
        /// </summary>
        public event CanGoBackChanged CanGoBackChanged = delegate { };

        /// <summary>
        /// Occurs when CanGoForward changes.
        /// </summary>
        public event CanGoForwardChanged CanGoForwardChanged = delegate { };

        /// <summary>
        /// Occurs when the user moves the cursor over an element in the current document.
        /// </summary>
        public event MouseDidMoveOverElementEventHandler MouseDidMoveOverElement = delegate { };

        /// <summary>
        /// Occurs when a plugin crashed in this WebView.
        /// </summary>
        public event PluginFailedEventHandler PluginFailed = delegate { };

        /// <summary>
        /// Occurs when the WebKitBrowser control attempts to open a link in a new window.
        /// </summary>
        public event NewWindowRequestEventHandler NewWindowRequest = delegate { };

        /// <summary>
        /// Occurs when the WebKitBrowser control creates a new window.
        /// </summary>
        public event NewWindowCreatedEventHandler NewWindowCreated = delegate { };

        /// <summary>
        /// Occurs when the WebKitBrowser control creates a new window.
        /// </summary>
        public event NewWindowCreatedEventHandler PopupCreated = delegate { };

        
        /// <summary>
        /// Occurs when JavaScript requests an alert panel to be displayed via the alert() function.
        /// </summary>
        public event ShowJavaScriptAlertPanelEventHandler ShowJavaScriptAlertPanel = delegate { };

        /// <summary>
        /// Occurs when JavaScript requests a confirm panel to be displayed via the confirm() function.
        /// </summary>
        public event ShowJavaScriptConfirmPanelEventHandler ShowJavaScriptConfirmPanel = delegate { };

        /// <summary>
        /// Occurs when JavaScript requests a prompt panel to be displayed via the prompt() function.
        /// </summary>
        public event ShowJavaScriptPromptPanelEventHandler ShowJavaScriptPromptPanel = delegate { };

        /// <summary>
        /// Occurs when JavaScript requests a prompt panel to be displayed via the prompt() function when closing a frame.
        /// </summary>
        public event ShowJavaScriptPromptBeforeUnloadEventHandler ShowJavaScriptPromptBeforeUnload = delegate { };

        #endregion

        #region Public properties

        /// <summary>
        /// The current page zoom.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public float PageZoom { get { return WebView.pageSizeMultiplier(); } set { WebView.setPageSizeMultiplier(value); } }

        /// <summary>
        /// Estimated progress of page loading
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public double EstimatedProgress { get { return WebView.estimatedProgress(); } }

        /// <summary>
        /// The current print page settings.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PageSettings PageSettings { get; set; }

        /// <summary>
        /// Gets the title of the current document.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public string DocumentTitle { get; private set; }

        /// <summary>
        /// Gets or sets the current Url.
        /// </summary>
        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("Specifies the Url to navigate to.")]
        public Uri Url
        {
            get
            {
                if (loaded)
                {
                    Uri result;
                    return Uri.TryCreate(webView.mainFrame().dataSource().request().url(), 
                        UriKind.Absolute, out result) ? result : null;
                }
                else
                {
                    return initialUrl;
                }
            }
            set
            {
                if (loaded)
                {
                    if (value != null)
                        Navigate(value.AbsoluteUri);
                }
                else
                {
                    if (value != null && value.ToString() != string.Empty)
                    {
                        Stop();
                        Navigate(value.AbsoluteUri);
                    }

                    initialUrl = value;
                }
            }
        }

        /// <summary>
        /// Gets a value indicating whether a web page is currently being loaded.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool IsBusy
        {
            get
            {
                if (loaded)
                    return (webView.isLoading() > 0); 
                else
                    return false;
            }
        }

        /// <summary>
        /// Gets or sets the HTML content of the current document.
        /// </summary>
        [Browsable(true), DefaultValue(""), Category("Appearance")]
        [Description("The HTML content to be displayed if no Url is specified.")]
        public string DocumentText
        {
            get
            {
                if (loaded)
                {
                    try
                    {
                        return webView.mainFrame().dataSource().representation().documentSource();
                    }
                    catch (COMException)
                    {
                        return string.Empty;
                    }
                }
                else
                {
                    return initialText;
                }
            }
            set
            {
                if (loaded)
                    webView.mainFrame().loadHTMLString(value, null);
                else
                    initialText = value;
            }
        }

        /// <summary>
        /// Gets the Style of the Document. Null if WebView is null (before the creation of the WebKitBrowser object
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), Description("Gets the Style of the Document")]
        public WebKitDOMCSSDeclarationStyle DocumentStyle
        {
            get
            {
                if (loaded)
                    return new WebKitDOMCSSDeclarationStyle(WebView.computedStyleForElement(WebView.mainFrameDocument().documentElement(), string.Empty));
                else
                    return null;
            }
        }

        /// <summary>
        /// Gets the currently selected text.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public string SelectedText
        {
            get
            {
                if (loaded)
                    return webView.selectedText();
                else
                    return string.Empty;
            }
        }

        /// <summary>
        /// Gets or sets the application name for the user agent.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public string ApplicationName
        {
            get
            {
                if (webView != null)
                    return GlobalPreferences.ApplicationName;
                else
                    return string.Empty;
            }
            set
            {
                if (webView != null)
                    webView.setApplicationNameForUserAgent(value);
                GlobalPreferences.ApplicationName = value;
            }
        }

        /// <summary>
        /// Gets or sets the user agent string.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public string UserAgent
        {
            get
            {
                if (webView != null)
                    return webView.userAgentForURL(string.Empty);
                else
                    return string.Empty;
            }
            set
            {
                if (webView != null)
                    webView.setCustomUserAgent(value);
            }
        }

        /// <summary>
        /// Gets or sets the text size multiplier (1.0 is normal size).
        /// </summary>
        [Browsable(true), DefaultValue(1.0f), Category("Appearance")]
        [Description("Specifies the text size multiplier.")]
        public float TextSize
        {
            get
            {
                if (webView != null)
                    return webView.textSizeMultiplier();
                else
                    return 1.0f;
            }
            set
            {
                if (webView != null)
                    webView.setTextSizeMultiplier(value);
            }
        }

        /// <summary>
        /// Gets or sets whether the control can navigate to another page 
        /// once it's initial page has loaded.
        /// </summary>
        [Browsable(true), DefaultValue(true), Category("Behavior")]
        [Description("Specifies whether the control can navigate" +
            " to another page once it's initial page has loaded.")]
        public bool AllowNavigation 
        {
            get
            {
                if (loaded)
                    return policyDelegate.AllowNavigation;
                else
                    return initialAllowNavigation;
            }
            set
            {
                if (loaded)
                    policyDelegate.AllowInitialNavigation = policyDelegate.AllowNavigation = value;
                else
                    initialAllowNavigation = value;
            }
        }

        /// <summary>
        /// Gets or sets whether to allow file downloads.
        /// </summary>
        [Browsable(true), DefaultValue(true), Category("Behavior")]
        [Description("Specifies whether to allow file downloads.")]
        public bool AllowDownloads
        {
            get
            {
                if (loaded)
                    return policyDelegate.AllowDownloads;
                else
                    return initialAllowDownloads;
            }
            set
            {
                if (loaded)
                    policyDelegate.AllowDownloads = value;
                else
                    initialAllowDownloads = value;
            }
        }

        /// <summary>
        /// Gets or sets whether to allow links to be opened in a new window.
        /// </summary>
        [Browsable(true), DefaultValue(true), Category("Behavior")]
        [Description("Specifies whether to allow links to be" +
            " opened in a new window.")]
        public bool AllowNewWindows
        {
            get
            {
                if (loaded)
                    return policyDelegate.AllowNewWindows;
                else
                    return initialAllowNewWindows;
            }
            set
            {
                if (loaded)
                    policyDelegate.AllowNewWindows = value;
                else
                    initialAllowNewWindows = value;
            }
        }

        /// <summary>
        /// Gets a value indicating whether a previous page in the navigation history is available.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool CanGoBack
        {
            get
            {
                return loaded ? webView.backForwardList().backListCount() > 0 : false;
            }
        }

        /// <summary>
        /// Gets a value indicating whether a subsequent page in the navigation history is available.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool CanGoForward
        {
            get
            {
                return loaded ? webView.backForwardList().forwardListCount() > 0 : false;
            }
        }

        /// <summary>
        /// Gets the FrameElement of the current Document.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public DOM.HTMLElement FrameElement
        {
            get
            {
                return DOM.HTMLElement.Create(this.WebView.focusedFrame().frameElement());
            }
        }

        /// <summary>
        /// Gets a Document representing the currently displayed page.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public DOM.Document Document
        {
            get
            {
                return DOM.Document.Create(webView.mainFrameDocument());
            }
        }

        /// <summary>
        /// Gets the current version.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Version Version
        {
            get
            {
                return version;
            }
        }

        /// <summary>
        /// Gets or sets the scroll offset of the current page, in pixels from the origin.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Point ScrollOffset
        {
            get
            {
                if (webView != null)
                {
                    IWebViewPrivate v = (IWebViewPrivate)webView;
                    return new Point(v.scrollOffset().x, v.scrollOffset().y);
                }
                else
                {
                    return Point.Empty;
                }
            }
            set
            {
                if (webView != null)
                {
                    IWebViewPrivate v = (IWebViewPrivate)webView;
                    tagPOINT p = new tagPOINT();
                    p.x = value.X - ScrollOffset.X;
                    p.y = value.Y - ScrollOffset.Y;
                    v.scrollBy(ref p);
                }
            }
        }

        /// <summary>
        /// Gets the visible content rectangle of the current view, in pixels.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Rectangle VisibleContent
        {
            get
            {
                if (webView != null)
                {
                    IWebViewPrivate v = (IWebViewPrivate)webView;
                    tagRECT r = v.visibleContentRect();
                    return new Rectangle(r.left, r.top, (r.right - r.left), (r.bottom - r.top));
                }
                else
                {
                    return Rectangle.Empty;
                }
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether the context menu of the WebKitBrowser is enabled.
        /// </summary>
        [Browsable(true), DefaultValue(true), Category("Behavior")]
        [Description("Specifies whether the default browser context menu is enabled or a custom one will be implemented via CustomContextMenuManager")]
        public bool UseDefaultContextMenu
        {
            get { return _contextMenuEnabled; }
            set 
            {
                _contextMenuEnabled = value;
            }
        }

        

        #endregion

        #region Constructors / initialization functions

        /// <summary>
        /// Initializes a new instance of the WebKitBrowser control.
        /// </summary>
        public WebKitBrowser()
        {
            InitializeComponent();

            CheckForIllegalCrossThreadCalls = false; // this will prevent exceptions that occur when you use the favicon event 
            
            PageSettings = new PageSettings();
            if (LicenseManager.UsageMode != LicenseUsageMode.Designtime)
            {
                LanguageLoader.Initialize();
                // Control Events
                this.Load += new EventHandler(WebKitBrowser_Load);
                this.Resize += new EventHandler(WebKitBrowser_Resize);
                // If this is the first time the library has been loaded,
                // initialize the activation context required to load the
                // WebKit COM component registration free
                if ((actCtxRefCount++) == 0)
                {
                    activationContext = new ActivationContext(GlobalPreferences.WebKitPath + "\\OpenWebKitSharp.manifest"); 
                    activationContext.Initialize();
                    // TODO: more error handling here
                    if (activationContext.Initialized != true)
                    {
                        return;
                    }
                    // Enable OLE for drag and drop functionality - WebKit
                    // will throw an OutOfMemory exception if we don't...
                    Application.OleRequired();
                }

                // If this control is brought to focus, focus our webkit child window
                this.GotFocus += (s, e) =>
                {
                    NativeMethods.SetFocus(webViewHWND);
                };
                activationContext.Activate();

                webView = new WebViewClass();

                contextmenumanager = new ContextMenuManager(this);
                Marshal.AddRef(Marshal.GetIUnknownForObject(contextmenumanager));
                
                activationContext.Deactivate();
            }
        }

        internal string LastSelectedLink { get; set; }

        public void ShowInspector()
        {
            WebView.inspector().show();
            WebView.inspector().attach();
        }

        public string SuggestedApplicationNameForTerminal
        {
            get { return @"""" + System.IO.Path.GetFileName(Application.ExecutablePath) + @""""; }
        }
        public void RunTerminal(string command)
        {
            Microsoft.VisualBasic.Interaction.Shell(GlobalPreferences.WebKitPath + @"\defaults.exe " + command);
        }

        private void InitializeWebKit()
        {
            activationContext.Activate();

            resourceIntercepter = new ResourcesIntercepter(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(resourceIntercepter));

            frameLoadDelegate = new WebFrameLoadDelegate();
            Marshal.AddRef(Marshal.GetIUnknownForObject(frameLoadDelegate));
            
            downloadDelegate = new WebDownloadDelegate(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(downloadDelegate));
            
            uiDelegate = new WebUIDelegate(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(uiDelegate));

            resourcesLoadDelegate = new WebResourceLoadDelegate(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(resourcesLoadDelegate));
            
            //editingDelegate = new WebEditingDelegate(this);
            //Marshal.AddRef(Marshal.GetIUnknownForObject(editingDelegate));
            // not used (yet)

            policyDelegate = new WebPolicyDelegate(AllowNavigation, AllowDownloads, AllowNewWindows, this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(policyDelegate));

            formDelegate = new WebFormDelegate(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(formDelegate));

            center = new WebNotificationCenter();
            Marshal.AddRef(Marshal.GetIUnknownForObject(center));

            cssmanager = new WebKitDOMCSSManager(this);
            Marshal.AddRef(Marshal.GetIUnknownForObject(cssmanager));

            undoManager = new CustomUndoSystem(this);

            appearance = new AppearanceSettings(this);

            observer = new WebNotificationObserver();

            webView.setHostWindow(this.Handle.ToInt32());
            ((WebViewClass)webView).setPolicyDelegate(policyDelegate);
            webView.setFrameLoadDelegate(frameLoadDelegate);
            webView.setResourceLoadDelegate(resourcesLoadDelegate);
            webView.setDownloadDelegate(downloadDelegate);
            webView.setUIDelegate(uiDelegate);

            tagRECT rect = new tagRECT();
            rect.top = rect.left = 0;
            rect.bottom = this.Height - 1;
            rect.right = this.Width - 1;
            webView.initWithFrame(rect, null, null);

            IWebViewPrivate webViewPrivate = (IWebViewPrivate)webView;
            webViewHWND = (IntPtr)webViewPrivate.viewWindow();

            webViewPrivate.setFormDelegate(formDelegate);

            // Subscribe to FrameLoadDelegate events
            ((WebFrameLoadDelegate)frameLoadDelegate).DidRecieveTitle += new DidRecieveTitleEvent(frameLoadDelegate_DidRecieveTitle);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidFinishLoadForFrame += new DidFinishLoadForFrameEvent(frameLoadDelegate_DidFinishLoadForFrame);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidStartProvisionalLoadForFrame += new DidStartProvisionalLoadForFrameEvent(frameLoadDelegate_DidStartProvisionalLoadForFrame);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidCommitLoadForFrame += new DidCommitLoadForFrameEvent(frameLoadDelegate_DidCommitLoadForFrame);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidFailLoadWithError += new DidFailLoadWithErrorEvent(frameLoadDelegate_DidFailLoadWithError);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidFailProvisionalLoadWithError += new DidFailProvisionalLoadWithErrorEvent(frameLoadDelegate_DidFailProvisionalLoadWithError);
 #if DEBUG || RELEASE
            ((WebFrameLoadDelegate)frameLoadDelegate).WindowScriptObjectAvailable += new WindowScriptObjectAvailableEvent(WebKitBrowser_WindowScriptObjectAvailable);
            ((WebFrameLoadDelegate)frameLoadDelegate).DidClearWindowObject += new DidClearWindowObjectEvent(WebKitBrowser_DidClearWindowObject);
#endif
            // DownloadDelegate events            
            
            downloadDelegate.DecideDestinationWithSuggestedFilename += new DecideDestinationWithSuggestedFilenameEvent(downloadDelegate_DecideDestinationWithSuggestedFilename);
            downloadDelegate.DidBegin += new DidBeginEvent(downloadDelegate_DidBegin);

            
            // UIDelegate events
            uiDelegate.CreateWebViewWithRequest += new CreateWebViewWithRequestEvent(uiDelegate_CreateWebViewWithRequest);
            uiDelegate.CloseWindowRequest += new CloseWindowRequest(uiDelegate_CloseWindowRequest);
            uiDelegate.StatusTextChanged += new StatusTextChangedEvent(uiDelegate_StatusTextChanged);
            uiDelegate.RunJavaScriptAlertPanelWithMessage += new RunJavaScriptAlertPanelWithMessageEvent(uiDelegate_RunJavaScriptAlertPanelWithMessage);
            uiDelegate.RunJavaScriptConfirmPanelWithMessage += new RunJavaScriptConfirmPanelWithMessageEvent(uiDelegate_RunJavaScriptConfirmPanelWithMessage);
            uiDelegate.RunJavaScriptTextInputPanelWithPrompt += new RunJavaScriptTextInputPanelWithPromptEvent(uiDelegate_RunJavaScriptTextInputPanelWithPrompt);
            uiDelegate.RunJavaScriptPromptBeforeUnload += new RunJavaScriptPromptBeforeUnload(uiDelegate_RunJavaScriptPromptBeforeUnload);
            uiDelegate.MouseDidMoveOverElement += new MouseDidMoveOverElement(uiDelegate_UpdateCurrentElement);
            uiDelegate.GeolocationReq += new AllowGeolocationRequest(uiDelegate_GeolocationReq);
            // FormDelegate Events
            formDelegate.SubmitForm += new SubmitForm(formDelegate_SubmitForm);
            formDelegate.TextChangedInArea += new TextChangedInArea(formDelegate_TextChangedInArea);
            formDelegate.TextChangedInField += new TextChangedInField(formDelegate_TextChangedInField);
            formDelegate.TextFieldBeginEditing += new TextFieldBeginEditing(formDelegate_TextFieldBeginEditing);
            formDelegate.TextFieldEndEditing += new TextFieldEndEditing(formDelegate_TextFieldEndEditing);
            
            // ResourcesLoadDelegate
            resourcesLoadDelegate.PluginFailed += new PluginFailedWithError(resourcesLoadDelegate_PluginFailed);
            resourcesLoadDelegate.ResourceLoaded += new ResourceFinishedLoading(resourcesLoadDelegate_ResourceLoaded);
            resourcesLoadDelegate.ResourceLoading += new ResourceStartedLoading(resourcesLoadDelegate_ResourceLoading);
            resourcesLoadDelegate.ResourceSizeAvailable += new ResourceSizeAvailableEventHandler(resourcesLoadDelegate_ResourceProgress);
            resourcesLoadDelegate.ResourceRequestSent += new ResourceRequest(resourcesLoadDelegate_ResourceRequestSent);
            resourcesLoadDelegate.ResourceFailedLoading += new ResourceFailed(resourcesLoadDelegate_ResourceFailedLoading);
            
            progs = new WebNotificationObserver();
            progf = new WebNotificationObserver();

            policyDelegate.NewWindowRequestUrlAvailable += new WebKit.NewWindowRequest(policyDelegate_NewWindowRequestUrlAvailable);

            ApplicationName = GlobalPreferences.ApplicationName;

            center.defaultCenter().addObserver(observer, "WebProgressEstimateChangedNotification", webView);            
            observer.OnNotify += new OnNotifyEvent(observer_OnNotify);

            center.defaultCenter().addObserver(progs, "WebViewProgressStartedNotification", webView);
            progs.OnNotify += new OnNotifyEvent(progs_OnNotify);
            
            center.defaultCenter().addObserver(progf, "WebViewProgressFinishedNotification", webView);
            progf.OnNotify += new OnNotifyEvent(progf_OnNotify);
            
            //WebView.preferences().setEditingBehavior(WebKitEditingBehavior.WebKitEditingWinBehavior);
            ((IWebPreferencesPrivate)WebView.preferences()).setDeveloperExtrasEnabled(1);
#if DEBUG || RELEASE
            m = new JSManagement(this);
#endif
            activationContext.Deactivate();
        }

        [DllImport("User32")]
        public static extern short GetAsyncKeyState(int vKey);

        bool uiDelegate_GeolocationReq(WebView sender, webFrame frame, IWebSecurityOrigin o)
        {
            GeolocationRequestEventArgs args = new GeolocationRequestEventArgs(Url.ToString(), true, o);
            GeolocationPositionRequest(this, args);
            return args.Allow;
        }

        


#if DEBUG || RELEASE
        void WebKitBrowser_WindowScriptObjectAvailable(WebView WebView, IntPtr context, IntPtr windowScriptObject)
        {
            if (GetScriptManager != null)
                GetScriptManager.CreateWindowScriptObject(new JSCore.JSContext(context));
        }

        void WebKitBrowser_DidClearWindowObject(WebView WebView, IntPtr context, IntPtr windowScriptObject, IWebFrame frame)
        {
            if (GetScriptManager != null)
                GetScriptManager.CreateWindowScriptObject(new JSCore.JSContext(context));
        }
        public JSManagement GetScriptManager
        {
            get
            {
                return m;
            }
        }
#endif
        void progf_OnNotify(IWebNotification notification)
        {
            ProgressChanged(this, new ProgressChangesEventArgs(1.0));
        }

        void progs_OnNotify(IWebNotification notification)
        {
            ProgressChanged(this, new ProgressChangesEventArgs(0.0));
        }

        void listener_KeyPressed(object sender, KeyEventArgs e)
        {
            MessageBox.Show(e.KeyCode.ToString());
        }

        

        void resourcesLoadDelegate_ResourceFailedLoading(IWebURLResponse res, WebError error)
        {
            resourceIntercepter.ResFailed(res, error.localizedDescription());
        }

        string resourcesLoadDelegate_ResourceRequestSent(string url)
        {
            return resourceIntercepter.ResReq(url);
        }


        [Browsable(true), Category("Appearance"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the AppearanceSettings object which allows the user to change the way pages are rendered.")]
        public AppearanceSettings Appearance
        {
            get { return appearance; }
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the object that allows interference with resource loading and monitoring.")]
        public ResourcesIntercepter ResourceIntercepter
        {
            get
            {
                if (loaded)
                    return resourceIntercepter;
                else
                    throw new Exception("You cannot get the ResourceIntercepter before the WebView has been created.");
            }
        }

        void resourcesLoadDelegate_ResourceProgress(IWebURLResponse res, int length)
        {
            resourceIntercepter.ResProg(res, length);
        }

        void resourcesLoadDelegate_ResourceLoading(IWebURLResponse res)
        {
            resourceIntercepter.ResStart(res);
        }

        void resourcesLoadDelegate_ResourceLoaded(IWebURLResponse res)
        {
            resourceIntercepter.ResFinish(res);
        }

        void resourcesLoadDelegate_PluginFailed(WebView sender, WebError error)
        {
            if (webView == sender)
            {
                PluginFailed(this, new PluginFailedErrorEventArgs(error.localizedDescription()));
            }
        }

        void uiDelegate_UpdateCurrentElement(WebView sender, IDOMNode element)
        {
            if (element is IDOMHTMLInputElement)
                undoManager.node = (IDOMHTMLInputElement)element;
            try
            {
                if (element != null && element.GetType().Name != "__ComObject")
                MouseDidMoveOverElement(this, new MouseDidMoveOverElementEventArgs((Element)Element.Create(element)));
            }
            catch { }
        }

        int uiDelegate_RunJavaScriptPromptBeforeUnload(WebView sender, string message)
        {
            ShowJavaScriptPromptBeforeUnloadEventArgs args = new ShowJavaScriptPromptBeforeUnloadEventArgs(message);
            ShowJavaScriptPromptBeforeUnload(this, args);
            return Convert.ToInt32(args.ReturnValue);
        }


        void observer_OnNotify(IWebNotification notification)
        {
            WebKit.ProgressChangesEventArgs args = new ProgressChangesEventArgs((int)(webView.estimatedProgress() * 100));
            ProgressChanged(this, args);
        }

        void policyDelegate_NewWindowRequestUrlAvailable(string url)
        {
            if (!string.IsNullOrEmpty(url))
            {
                newwindowurl = url;
            }
            NewWindowRequest(this, new NewWindowRequestEventArgs(url)); // url and request seem to be empty in UiDelegate.createWebViewWithRequest
        }

        #region Browser Preferences

        
        /// <summary>
        /// Gets or sets the current Document's Encoding.
        /// </summary>
        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets or sets the encoding of the current document.")]
        public Encoding DocumentEncoding
        {
            get
            {
                if (loaded)
                    return Encoding.GetEncoding(WebView.customTextEncodingName());
                else
                    return initialEncoding;
            }
            set
            {
                if (loaded)
                    WebView.setCustomTextEncodingName(value.EncodingName);
                else
                    initialEncoding = value;
            }
        }

        [Description("Gets whether this page is considered as secure.")]
        public bool IsSecure
        {
            get
            {
                return Url.ToString().StartsWith("https://");
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether JavaScript is enabled.
        /// </summary>
        [Browsable(true), DefaultValue(false), Category("Behavior")]
        [Description("Specifies whether a custom context menu will be implemented")]
        public bool UseCustomContextMenu { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether JavaScript is enabled.
        /// </summary>
        [Browsable(true), DefaultValue(true), Category("Behavior")]
        [Description("Specifies whether JavaScript is enabled in the WebKitBrowser")]
        public bool UseJavaScript
        {
            get
            {
                if (loaded)
                    return WebView.preferences().isJavaScriptEnabled() != 0;
                else
                    return initialJavaScriptEnabled;
            }
            set
            {
                if (loaded)
                {
                    var prefs = webView.preferences();
                    prefs.setJavaScriptEnabled(value ? 1 : 0);
                    webView.setPreferences(prefs);
                }
                else
                {
                    initialJavaScriptEnabled = value;
                }
            }
        }
        #endregion

        string uiDelegate_RunJavaScriptTextInputPanelWithPrompt(WebView sender, string message, string defaultText)
        {
            ShowJavaScriptPromptPanelEventArgs args = new ShowJavaScriptPromptPanelEventArgs(message, defaultText);
            ShowJavaScriptPromptPanel(this, args);
            return args.ReturnValue;
        }

        int uiDelegate_RunJavaScriptConfirmPanelWithMessage(WebView sender, string message)
        {
            ShowJavaScriptConfirmPanelEventArgs args = new ShowJavaScriptConfirmPanelEventArgs(message);
            ShowJavaScriptConfirmPanel(this, args);
            return Convert.ToInt32(args.ReturnValue);
        }

        void uiDelegate_RunJavaScriptAlertPanelWithMessage(WebView sender, string message)
        {
            ShowJavaScriptAlertPanel(this, new ShowJavaScriptAlertPanelEventArgs(message));
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the node at the specified point.")]
        public Node ElementAtPoint(System.Drawing.Point point)
        {
            tagPOINT p = new tagPOINT();
            p.x = point.X;
            p.y = point.Y;
            object el;

            WebView.elementAtPoint(ref p).RemoteRead("WebElementDOMNodeKey", out el, null, 0, null);
            return Node.Create(el as IDOMNode);   
          
        }
        private string _status = string.Empty;
        void uiDelegate_StatusTextChanged(string statustext)
        {
            if (IsBusy)
            {
                if (statustext == string.Empty)
                    StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(_status));
                else
                    StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(statustext));
            }
            else
                StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(statustext));
        }

        void uiDelegate_CloseWindowRequest(WebView webView)
        {
            CloseWindowRequest(this, new EventArgs());
        }
        private List<WebKitFormData> FormData = new List<WebKitFormData>();
            
        void formDelegate_TextFieldEndEditing(HTMLElement element)
        {
            TextFieldEndEditing(this, new FormDelegateElementEventArgs(element));
            if (element.InnerText != string.Empty)
            {
                FormData.Add(new WebKitFormData(element, element.GetAttribute("Value")));
            }
        }

        void formDelegate_TextFieldBeginEditing(HTMLElement element)
        {
            TextFieldBeginEditing(this, new FormDelegateElementEventArgs(element));
        }

        void formDelegate_TextChangedInField(HTMLElement element)
        {
            TextChangeInField(this, new FormDelegateElementEventArgs(element));
        }

        void formDelegate_TextChangedInArea(HTMLElement element)
        {
            TextChangeInArea(this, new FormDelegateElementEventArgs(element));
        }


        void formDelegate_SubmitForm(HTMLElement form, IWebFormSubmissionListener listener)
        {
            FormSubmit(this, new FormDelegateFormEventArgs(form, listener, FormData));
            listener.continueSubmit();
            FormData.Clear();
        }


        #endregion

        #region Control event handers

        private void WebKitBrowser_Resize(object sender, EventArgs e)
        {
            // Resize the WebKit control
            NativeMethods.MoveWindow(webViewHWND, 0, 0, this.Width - 1, this.Height - 1, true);
        }

        private void WebKitBrowser_Load(object sender, EventArgs e)
        {
            // Create the WebKit browser component
            InitializeWebKit();

            loaded = webView != null;

            // intialize properties that depend on load
            if (initialUrl != null)
            {
                Navigate(initialUrl.AbsoluteUri);
            }
            else
            {
                DocumentText = initialText;
                policyDelegate.AllowInitialNavigation = false;
            }

            UseJavaScript = initialJavaScriptEnabled;
            AllowCookies = initialAllowCookies;
            if (initialEncoding != null)
               DocumentEncoding = initialEncoding;
            UseDefaultContextMenu = initialUseDefaultContextMenu;
            EnableHTTPPipelining = initialUsePipelining;
        }

        /// <summary>
        /// Gets or sets a value indicating whether HTTP Pipelining is enabled.
        /// </summary>
        [Browsable(false), DefaultValue(true)]
        [Description("Specifies whether Pipelining is enabled in the WebKitBrowser")]
        public bool EnableHTTPPipelining
        {
            get
            {
                if (loaded)
                    return WebView.httpPipeliningEnabled() != 0;
                else
                    return initialUsePipelining;
            }
            set
            {
                if (loaded)
                {
                    WebView.setHTTPPipeliningEnabled(Convert.ToInt32(value));
                }
                else
                    initialUsePipelining = value;
            }
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Specifies whether Cookies are enabled.")]
        public bool AllowCookies
        {
            get
            {
                if (loaded)
                    return Convert.ToBoolean(WebView.cookieEnabled());
                else
                    return initialAllowCookies;
            }
            set
            {
                if (loaded)
                    WebView.setCookieEnabled(Convert.ToInt32(value));
                else
                    initialAllowCookies = value;
            }
        }
        #endregion

        #region IWebFrameLoadDelegate event handlers 

        private void frameLoadDelegate_DidCommitLoadForFrame(WebView WebView, IWebFrame frame)
        {
            if (frame == webView.mainFrame())
            {
                StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(LanguageLoader.RenderingPage));
                Navigated(this, new WebBrowserNavigatedEventArgs(this.Url));
                BackgroundWorker favicongetter = new BackgroundWorker();
                favicongetter.DoWork += new DoWorkEventHandler(favicongetter_DoWork);
                favicongetter.RunWorkerAsync(new _fav(Url, Document));
            }
        }

        void favicongetter_DoWork(object sender, DoWorkEventArgs e)
        {
            if (Preferences.WillGetFavicon)
            {
                try
                {
                    System.Uri theURL = (e.Argument as _fav).u;
                    if (theURL.Host == "www.youtube.com" | theURL.AbsoluteUri == "http://www.youtube.com/")
                    {
                        string iconURL = "http://s.ytimg.com/yt/favicon-refresh-vfldLzJxy.ico";
                        System.Net.WebRequest request = System.Net.WebRequest.Create(iconURL);
                        using (System.Net.WebResponse responseSys = request.GetResponse())
                        {
                            System.IO.Stream stream = responseSys.GetResponseStream();
                            Bitmap favicon = (Bitmap)Image.FromStream(stream);
                            Icon theIco = System.Drawing.Icon.FromHandle(favicon.GetHicon());
                            FaviconAvailable(this, new FaviconAvailableEventArgs(theIco));
                            stream.Flush();
                            stream.Dispose();
                        }
                    }
                    else if (theURL.Host == "html5test.com" | theURL.AbsoluteUri == "http://www.html5test.com/")
                    {
                        string iconURL = "http://" + theURL.Host + "/icon.png";
                        System.Net.WebRequest request = System.Net.WebRequest.Create(iconURL);
                        using (System.Net.WebResponse responseSys = request.GetResponse())
                        {
                            System.IO.Stream stream = responseSys.GetResponseStream();
                            Bitmap favicon = (Bitmap)Image.FromStream(stream);
                            Icon theIco = System.Drawing.Icon.FromHandle(favicon.GetHicon());
                            FaviconAvailable(this, new FaviconAvailableEventArgs(theIco));
                            stream.Flush();
                            stream.Dispose();
                        }
                    }
                    else if (theURL.Host == "sites.google.com" | theURL.AbsoluteUri == "www.google.com/images/icons/product/sites-16.ico")
                    {
                        string iconURL = "https://www.google.com/images/icons/product/sites-16.ico";
                        System.Net.WebRequest request = System.Net.WebRequest.Create(iconURL);
                        
                        using (System.Net.WebResponse responseSys = request.GetResponse())
                        {
                            System.IO.Stream stream = responseSys.GetResponseStream();
                            Bitmap favicon = (Bitmap)Image.FromStream(stream);
                            Icon theIco = System.Drawing.Icon.FromHandle(favicon.GetHicon());
                            
                            FaviconAvailable(this, new FaviconAvailableEventArgs(theIco));

                            stream.Flush();
                            stream.Dispose();
                        }
                    }
                    else
                    {
                        using (System.Net.WebClient webclient = new System.Net.WebClient())
                        {
                            using (System.IO.MemoryStream MemoryStream = new System.IO.MemoryStream(webclient.DownloadData("http://www.google.com/s2/favicons?domain=" + theURL.Host)))
                            {
                                webclient.Dispose();
                                Image favicon = Image.FromStream(MemoryStream);
                                Bitmap bmp = (Bitmap)favicon;
                                if (favicon != null)
                                    FaviconAvailable(this, new FaviconAvailableEventArgs(Icon.FromHandle(bmp.GetHicon())));
                                else
                                    FaviconAvailable(this, new FaviconAvailableEventArgs(Properties.Resources.New_document));
                                MemoryStream.Flush();
                            }
                        }
                    }
                }
                catch
                {
                    FaviconAvailable(this, new FaviconAvailableEventArgs(WebKit.Properties.Resources.New_document));
                }
                
            }
        }
        private bool CheckIsUrlValid(string url)
        {
            try
            {
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
                using (HttpWebResponse rsp = (HttpWebResponse)req.GetResponse())
                {
                    if (rsp.StatusCode == HttpStatusCode.OK)
                    {
                        return true;
                    }
                }
            }
            catch
            { }
            return false;
        }
        internal string tempmimetype = null;
        internal void OnMissingPlugin(IDOMElement el)
        {
            PluginFailed(this, new PluginFailedErrorEventArgs("Plugin missing for element with TagName: " + el.tagName()));
        }

        private void frameLoadDelegate_DidStartProvisionalLoadForFrame(WebView WebView, IWebFrame frame)
        { 
            if (frame == webView.mainFrame())
            {
                string url = frame.provisionalDataSource().request().url();
                if (!string.IsNullOrEmpty(url))
                {
                    BackgroundWorker bw = new BackgroundWorker();
                    bw.DoWork += new DoWorkEventHandler(bw_DoWork);
                    HeadersAvailableEventArgs h = new HeadersAvailableEventArgs(new Uri(frame.provisionalDataSource().request().url()), frame.provisionalDataSource().request());
                    bw.RunWorkerAsync(h);
                    string Url = h.Url.ToString();
                    

                    if (resourceIntercepter != null)
                        resourceIntercepter.Resources.Clear();
                    WebKitBrowserNavigatingEventArgs args = new WebKitBrowserNavigatingEventArgs(new Uri(url), frame.name(), tempmimetype);
                    Navigating(this, args);
                    if (args.Cancel == true)
                    {
                        frame.stopLoading();
                    }
                    else
                    {
                        try
                        {
                            StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(string.Format(LanguageLoader.ConnectingWith, new Uri(frame.provisionalDataSource().request().url()).Host))); 
                            _status = string.Format(LanguageLoader.ConnectingWith, new Uri(frame.provisionalDataSource().request().url()).Host);
                        }
                        catch { }
                    }
                }
            } 
         }
        private WebMutableURLRequestClass currentreq { get; set; }
        void bw_DoWork(object sender, DoWorkEventArgs e)
        {
            HeadersAvailableEventArgs h = e.Argument as HeadersAvailableEventArgs;
            try
            {
                string Url = h.Url.ToString();
                WebRequest WebRequestObject = HttpWebRequest.Create(Url);
                WebResponse ResponseObject = WebRequestObject.GetResponse();

                List<Header> headers = new List<Header>();

                foreach (string HeaderKey in ResponseObject.Headers)
                    headers.Add(new Header(HeaderKey, ResponseObject.Headers[HeaderKey]));

                ResponseObject.Close();

                h.Headers = headers.ToArray();
                HeadersAvailable(this, h);
            }
            catch { }
        }

        private void frameLoadDelegate_DidFinishLoadForFrame(WebView WebView, IWebFrame frame)
        {
            if (frame == webView.mainFrame())
            { 
                policyDelegate.AllowInitialNavigation = policyDelegate.AllowNavigation;
                if (Url != null)
                {
                    try
                    {
                        DocumentCompleted(this, new WebBrowserDocumentCompletedEventArgs(this.Url));
                        if (Url.ToString() == "about:blank") // fix for issue 37: http://code.google.com/p/open-webkit-sharp/issues/detail?id=37
                        {
                            this.DocumentTitle = string.Empty;
                            DocumentTitleChanged(this, new EventArgs());
                        }
                    }
                    catch { }
                }
                ProgressChanged(this, new ProgressChangesEventArgs(1.0));
                CanGoBackChanged(this, new CanGoBackChangedEventArgs(CanGoBack));
                CanGoForwardChanged(this, new CanGoForwardChangedEventArgs(CanGoForward));
                StatusTextChanged(this, new WebKitBrowserStatusChangedEventArgs(LanguageLoader.Completed));
                this._status = "Completed";
                if (Preferences.AutomaticallyCheckForPhishingBehavior)
                {
                    if (WebKit.Security.FishPhish.IsSitePhishing(this))
                    {
                        DangerousSiteDetected(this, new EventArgs());
                    }
                }
            }
         }

        private void frameLoadDelegate_DidRecieveTitle(WebView WebView, string title, IWebFrame frame)
        {
            if (frame == webView.mainFrame())
            {
                DocumentTitle = title;
                DocumentTitleChanged(this, new EventArgs());
            }
        }

        private void frameLoadDelegate_DidFailProvisionalLoadWithError(WebView WebView, IWebError error, IWebFrame frame)
        {
            // ignore an "error" where the page loading is interrupted by a policy change when dowloading a file
            if (!(frame == WebView.mainFrame() && error.Domain() == "WebKitErrorDomain" && error.code() == 102))
            {
                Error(this, new WebKitBrowserErrorEventArgs(error.localizedDescription())); 
            }
        }

        private void frameLoadDelegate_DidFailLoadWithError(WebView WebView, IWebError error, IWebFrame frame)
        {
            if (error.localizedDescription() != "cancelled")
            {
                Error(this, new WebKitBrowserErrorEventArgs(error.localizedDescription()));
            }
        }

        #endregion

        [Description("Activates the context so that interference with WebKit COM Types is allowed.")]
        public static void ActivateContext()
        {
            activationContext.Activate();
        }
        
        [Description("Deactivates the activation context after interference with WebKit COM Types is finished")]
        public static void DeactivateContext()
        {
            activationContext.Deactivate();
        }
        #region WebDownloadDelegate event handlers

        
        private void downloadDelegate_DidBegin(WebDownload download)
        {
            // add anything you want to do before the event is fired
        }
        string candownload = "yes";
        WebDownload canornot;

        private void downloadDelegate_DecideDestinationWithSuggestedFilename(WebDownload download, string fileName)
        {
            download.setDeletesFileUponFailure(1);
            if (string.IsNullOrEmpty(fileName) == false)
            {
                string url = download.request().url();
                if (GlobalPreferences.WillHandleDownloadsManually)
                {
                    FileDownloadBeginEventArgs args = new FileDownloadBeginEventArgs(download.request().url(), fileName, download);
                    DownloadBegin(this, args);
                }
                else
                {
                    if (!(canornot == download))
                    {
                        canornot = download;
                        candownload = "yes";
                    }
                    else { canornot = null; }
                    if (!url.StartsWith("file://"))
                        foreach (Form hello in Application.OpenForms)
                        {
                            if (hello.Name == "MainDownloadForm")
                            {
                                candownload = "no";
                            }
                        }
                    if (candownload == "yes")
                    {
                        MyDownloader.App.UI.MainDownloadForm newd = new MyDownloader.App.UI.MainDownloadForm();
                        newd.Show();
                        newd.downloadList1.NewFileDownload(url, fileName, true, false);
                    }
                    else
                    {
                        ((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).Show();
                        ((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).downloadList1.NewFileDownload(url, fileName, true, false);

                    }
                    download.cancelForResume();
                }
            }
        }
        private delegate void DownloadEventHandler(WebDownload download, string filename);
        #endregion

        #region WebUIDelegate event handlers
        internal string newwindowurl;
        private void uiDelegate_CreateWebViewWithRequest(IWebURLRequest request, out WebView webView, bool popup = false)
        {
            WebKitBrowser b = new WebKitBrowser();
            if (request != null)
            {
                if (!string.IsNullOrEmpty(request.url()))
                    b.Navigate(request.url());
            }
            webView = (WebView) b.webView;
            string url = newwindowurl;
            if (string.IsNullOrEmpty(newwindowurl) && request != null)
                url = request.url();
            
            NewWindowRequestEventArgs args = new NewWindowRequestEventArgs(url);
            if (popup == true)
            {
                PopupCreated(this, new NewWindowCreatedEventArgs(b));
            }
            else
            {
                NewWindowRequest(this, args);
                NewWindowCreated(this, new NewWindowCreatedEventArgs(b));
            }
            //if (this.Focused == false || ElementAtPoint(this.PointToClient(Cursor.Current.HotSpot)).Type == ElementType.Body || (GetCurrentElement().Type != ElementType.LinkOrUknown || GetCurrentElement().TagName == "BODY" || GetCurrentElement().TagName == "IFRAME" || GetCurrentElement() == null && GetCurrentElement().TagName != "OBJECT"))
            //    {
            //        PopupCreated(this, new NewWindowCreatedEventArgs(b));
            //    }
            //    else
            //    {
            //        NewWindowRequest(this, args);
            //        NewWindowCreated(this, new NewWindowCreatedEventArgs(b));
            //    }
        }

        #endregion

        #region Authentication
        public string Username { get; set; }
        public string Password { get; set; }
        #endregion

        #region Editing
        public void Undo()
        {
            undoManager.Undo();
        }
        public void Redo()
        {
            undoManager.Redo();
        }
        public void Paste()
        {
            if (WebView != null)
                WebView.paste(WebView);
        }
        public void Cut()
        {
            if (WebView != null)
                WebView.cut(WebView);
        }
        public void Copy()
        {
            if (WebView != null)
                WebView.copy(WebView);
        }
        #endregion

        #region Public Methods

        
        [Description("Shows the downloader form to the user.")]
        public void ShowDownloader()
        {
            string canshowdownloader;
            canshowdownloader = "yes";
            foreach (Form f in Application.OpenForms)
            {
                if (f.Name == "MainDownloadForm")
                {
                    canshowdownloader = "no";
                }
            }
            if (canshowdownloader == "yes")
            {
                MyDownloader.App.UI.MainDownloadForm newd = new MyDownloader.App.UI.MainDownloadForm();
                newd.Show();
            }
            else
            {
                ((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).Show();
                
            }
        }
        [Description("Opens the default downloader and downloads the specified file.")]
        void DownloadFileUsingDefaultDownloader(string url, string filename)
        {
            string canshowdownloader;
            canshowdownloader = "yes";
            foreach (Form f in Application.OpenForms)
            {
                if (f.Name == "MainDownloadForm")
                {
                    canshowdownloader = "no";
                }
            }
            if (canshowdownloader == "yes")
            {
                MyDownloader.App.UI.MainDownloadForm newd = new MyDownloader.App.UI.MainDownloadForm();
                newd.Show();
                newd.downloadList1.NewFileDownload(url, filename, true);
            }
            else
            {
                ((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).Focus();
                ((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).downloadList1.NewFileDownload(url, filename, true);
				((MyDownloader.App.UI.MainDownloadForm)Application.OpenForms["MainDownloadForm"]).Show ();
            }
        }


        /// <summary>
        /// Unmark all text matches when the find method has been used
        /// </summary>
        public void UnmarkTextMatches()
        {
            WebView.unmarkAllTextMatches();
        }
        /// <summary>
        /// Navigates to the specified Url.
        /// </summary>
        /// <param name="url">Url to navigate to.</param>
        public void Navigate(string url)
        {
            if (loaded)
            {
                if (url.StartsWith("javascript::"))
                {
#if DEBUG || RELEASE
                    try
                    {
                        GetScriptManager.EvaluateScript(url.Split(Convert.ToChar("::"))[1]);
                    }
                    catch (Exception ex) { Error(this, new WebKitBrowserErrorEventArgs("JavaScript execution failed: " + ex.Message)); }
#else
                    try
                    {
                        StringByEvaluatingJavaScriptFromString(url);
                    }
                    catch (Exception ex) { Error(this, new WebKitBrowserErrorEventArgs("JavaScript execution failed: " + ex.Message)); }
#endif
                    return;
                }
                // prepend with "http://" if url not well formed
                if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                    url = "http://" + url;
                else
                    url = "" + url;
                activationContext.Activate();

                WebMutableURLRequest request = new WebMutableURLRequest();

                request.setHTTPMethod("GET");
                
                request.initWithURL(url, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                if (Preferences.IgnoreSSLErrors)
                    request.setAllowsAnyHTTPSCertificate();

                if (!string.IsNullOrEmpty(Username) && !string.IsNullOrEmpty(Password))
                    request.setValue("Basic " + Convert.ToBase64String(System.Text.Encoding.ASCII.GetBytes(string.Format("{0}:{1}", Username, Password))), "Authorization");
                
                webView.mainFrame().loadRequest((WebURLRequest)request);

                activationContext.Deactivate();
            }
            else
            {
                if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                    url = "http://" + url;
                initialUrl = url.Length == 0 ? null : new Uri(url);
            }
        }
		public void Navigate(string url, System.Collections.Generic.List<Header> Headers)
        {
            if (loaded)
            {
                if (url.StartsWith("javascript::"))
                {
#if DEBUG || RELEASE
                    try
                    {
                        GetScriptManager.EvaluateScript(url.Split(Convert.ToChar("::"))[1]);
                    }
                    catch (Exception ex) { Error(this, new WebKitBrowserErrorEventArgs("JavaScript execution failed: " + ex.Message)); }
#else
                    try
                    {
                        StringByEvaluatingJavaScriptFromString(url);
                    }
                    catch (Exception ex) {Error(this, new WebKitBrowserErrorEventArgs("JavaScript execution failed: " + ex.Message)); }
#endif
                    return;
                }
                // prepend with "http://" if url not well formed
                if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                    url = "http://" + url;
                else
                    url = "" + url;
                activationContext.Activate();

                WebMutableURLRequest request = new WebMutableURLRequest();

                request.setHTTPMethod("GET");
                
                request.initWithURL(url, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                
                if (Preferences.IgnoreSSLErrors)
                    request.setAllowsAnyHTTPSCertificate();

                if (!string.IsNullOrEmpty(Username) && !string.IsNullOrEmpty(Password))
                    request.setValue("Basic " + Convert.ToBase64String(System.Text.Encoding.ASCII.GetBytes(string.Format("{0}:{1}", Username, Password))), "Authorization");
                
                foreach(WebKit.Header h in Headers)
				{
					request.setValue (h.Value, h.Field);
				}
                webView.mainFrame().loadRequest((WebURLRequest)request);

                activationContext.Deactivate();
            }
            else
            {
                if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                    url = "http://" + url;
                initialUrl = url.Length == 0 ? null : new Uri(url);
            }
        }

        [Browsable(true), Category("Appearance"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the Favicon of the current page.")]
        public Image GetFavicon()
        {
            Uri u = this.Url;
            System.Net.WebClient webclient = new System.Net.WebClient();
            System.IO.MemoryStream MemoryStream = new System.IO.MemoryStream(webclient.DownloadData("http://www.google.com/s2/favicons?domain=" + u.Host));
            
            webclient.Dispose();
            return Image.FromStream(MemoryStream);
        }

        [Browsable(true), Category("Appearance"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the Favicon of the specified url.")]
        public Image GetFavicon(string url)
        {
            Uri u = new Uri(url);
            System.Net.WebClient webclient = new System.Net.WebClient();
            System.IO.MemoryStream MemoryStream = new System.IO.MemoryStream(webclient.DownloadData("http://www.google.com/s2/favicons?domain=" + u.Host));
            webclient.Dispose();
            return Image.FromStream(MemoryStream);
        }

        /// <summary>
        /// Navigates to the previous page in the page history, if available.
        /// </summary>
        /// <returns>Success value.</returns>
        public bool GoBack()
        {
            bool retVal = CanGoBack;
            webView.goBack();
            return retVal;
        }

        /// <summary>
        /// Navigates to the next page in the page history, if available.
        /// </summary>
        /// <returns>Success value.</returns>
        public bool GoForward()
        {
            bool retVal = CanGoForward;
            webView.goForward();
            return retVal;
        }

        /// <summary>
        /// Displays the View Source Form to the user
        /// </summary>
        /// <returns>Success value.</returns>
        public bool GetSourceCode()
        {
            bool srcgot = true;
            SourceCodeForm srcfrm = new SourceCodeForm();
            srcfrm.fastColoredTextBox1.Language = FastColoredTextBoxNS.Language.HTML;
            try
            {
                srcfrm.fastColoredTextBox1.Text = DocumentText;
            }
            catch { srcgot = false; }
            srcfrm.Show();
            return srcgot;
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Gets the status text of the current WebView.")]
        public string StatusText = string.Empty;

        /// <summary>
        /// Find method
        /// </summary>
        public int Find(string text, out uint totalmatches, int forward = 1,int Case = 0, int Wrap = 1, bool highlite = true)
        {
            WebView.markAllMatchesForText(text,Case,Convert.ToInt32(highlite) ,500,out totalmatches);
            return webView.searchFor(text,forward,Case,Wrap); 
        }
        
        /// <summary>
        /// Reloads the current web page.
        /// </summary>
        public void Reload()
        {
            if (webView.isLoading() == 1)
                webView.mainFrame().stopLoading();
            webView.mainFrame().reload();
        }


        /// <summary>
        /// Stops loading the current web page and any resources associated 
        /// with it.
        /// </summary>
        public void Stop()
        {
                if (webView.isLoading() != 0)
                {
                    webView.mainFrame().stopLoading();
                }
        }

        /// <summary>
        /// Returns the result of running a script.
        /// </summary>
        /// <param name="Script">The script to run.</param>
        /// <returns></returns>
        public string StringByEvaluatingJavaScriptFromString(string Script)
        {
            return webView.stringByEvaluatingJavaScriptFromString(Script);
        }

        /// <summary>
        /// Gets the underlying WebKit WebView object used by this instance of WebKitBrowser.
        /// </summary>
        /// <returns>The WebView object.</returns>
        public object GetWebViewAsObject()
        {
            return webView;
        }

        // printing methods

        /// <summary>
        /// Prints the document using the current print and page settings.
        /// </summary>
        public void Print()
        {
            WebBrowser br = new WebBrowser();
            br.ScriptErrorsSuppressed = false;
            br.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(br_DocumentCompleted);
            br.Tag = "Print";
            System.IO.File.WriteAllText(Path.GetTempPath() + @"\owstemp.html", DocumentText);
            br.Navigate(Path.GetTempPath() + @"\owstemp.html");
        }

        /// <summary>
        /// Displays a Page Setup dialog box with the current page and print settings.
        /// </summary>
        public void ShowPageSetupDialog()
        {
            WebBrowser br = new WebBrowser();
            br.ScriptErrorsSuppressed = true;
            br.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(br_DocumentCompleted);
            br.Tag = "PageSetup";
            System.IO.File.WriteAllText(Application.StartupPath + @"\owstemp.html", DocumentText);
            br.Navigate(Application.StartupPath + @"\owstemp.html");
        }

        /// <summary>
        /// Displays a Print dialog box.
        /// </summary>
        public void ShowPrintDialog()
        {
            WebBrowser br = new WebBrowser();
            br.ScriptErrorsSuppressed = true;
            br.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(br_DocumentCompleted);
            br.Tag = "PrintDlg";
            System.IO.File.WriteAllText(Application.StartupPath + @"\owstemp.html", DocumentText);
            br.Navigate(Application.StartupPath + @"\owstemp.html");
        }

        void br_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            WebBrowser s = (sender as WebBrowser);
            foreach (HtmlElement el in s.Document.GetElementsByTagName("img"))
                if ((el.GetAttribute("src").StartsWith("http") == false))
                   el.SetAttribute("src", "http://" + s.Url.Host + el.GetAttribute("src").Replace("..", ""));
            
            string c =  ((string)s.Tag);
            if (c == "PrintDlg")
                s.ShowPrintDialog();
            if (c == "PrintPreviewDlg")
                s.ShowPrintPreviewDialog();
            if (c == "Print")
                s.Print();
            if (c == "PageSetup")
                s.ShowPageSetupDialog();
            s.Dispose();
            if (File.Exists(Application.StartupPath + @"\owstemp.html"))
                File.Delete(Application.StartupPath + @"\owstemp.html");
        }

        /// <summary>
        /// Displays a Print Preview dialog box.
        /// </summary>
        public void ShowPrintPreviewDialog()
        {
            WebBrowser br = new WebBrowser();
            br.ScriptErrorsSuppressed = true;
            br.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(br_DocumentCompleted);
            br.Tag = "PrintPreviewDlg";
            System.IO.File.WriteAllText(Application.StartupPath + @"\owstemp.html", DocumentText);
            br.Navigate(Application.StartupPath + @"\owstemp.html");
        }

        public GlobalPreferences Preferences
        {
            get
            {
                if (preferences == null)
                    preferences = new GlobalPreferences(this);

                return preferences;
            }
        }
        #endregion Public Methods

        #region Headers
        [Description("Returns the responded HTTP headers of the current URL. The first string resembles the Header's key and the second its value. For editing a value please use WebKitBrowser.SetHeader")]
        public Array Headers()
        {
            try
            {
                string Url = this.Url.ToString();
                WebRequest WebRequestObject = HttpWebRequest.Create(Url);
                WebResponse ResponseObject = WebRequestObject.GetResponse();

                List<Header> Headers = new List<Header>();

                foreach (string HeaderKey in ResponseObject.Headers)
                    Headers.Add(new Header(HeaderKey, ResponseObject.Headers[HeaderKey]));

                ResponseObject.Close();

                return Headers.ToArray();
            }
            catch { throw new Exception("A problem occured when trying to get the headers of the current Url. It is recommended that you report this problem as an issue at the Google Code page of OpenWebKitSharp so as to get support for your problem"); }
        }

        [Description("Adds a header with the specified value")]
        public void AddHeader(Header header)
        {
            WebView.mainFrame().dataSource().request().addValue(header.Value, header.Field);
        }

        [Description("Sets a header's field with the specified value")]
        public void SetHeader(Header header, string value)
        {
            WebView.mainFrame().dataSource().request().setValue(value, header.Field);
        }

        [Description("Gets the header with the specified field")]
        public Header GetHeader(string field)
        {
            return new Header(field, WebView.mainFrame().dataSource().request().valueForHTTPHeaderField(field));
        }

        [Description("Gets a value of a header")]
        public string GetHeaderValue(string field)
        {
            return WebView.mainFrame().dataSource().request().valueForHTTPHeaderField(field);
        }
        #endregion
    }
    internal class _fav
    {
        internal Uri u { get; set; }
        internal Document d { get; set; }
        public _fav(Uri u, Document d)
        {
            this.u = u;
            this.d = d;
        }
    }
}
