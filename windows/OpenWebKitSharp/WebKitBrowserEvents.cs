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

// Various event handlers and argument classes used by WebKitBrowser to
// communicate with the client.

using System;
using WebKit.Interop;
using System.Collections.Generic;
using WebKit.DOM;
using System.Drawing;
using System.Net;
using System.ComponentModel;

namespace WebKit
{
    #region Event handler delegates

    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.Error event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A WebKitBrowserErrorEventArgs that contains the event data.</param>
    public delegate void WebKitBrowserErrorEventHandler (object sender, WebKitBrowserErrorEventArgs e);
    public delegate void WebKitBrowserNavigatingEventHandler(object sender, WebKitBrowserNavigatingEventArgs e);
    public delegate void StatusTextChanged (object sender, WebKitBrowserStatusChangedEventArgs e);
    public delegate void WillSubmitForm(object sender, FormDelegateFormEventArgs e);
    public delegate void TextDidChangeInArea(object sender, FormDelegateElementEventArgs e);
    public delegate void TextDidChangeInField(object sender, FormDelegateElementEventArgs e);
    public delegate void TextFieldDidBeginEditing(object sender, FormDelegateElementEventArgs e);
    public delegate void TextFieldDidEndEditing(object sender, FormDelegateElementEventArgs e);
    public delegate void PluginFailedEventHandler(object sender, PluginFailedErrorEventArgs e);
    public delegate void HeadersAvailableEventHandler(object sender, HeadersAvailableEventArgs e);
    public delegate void GeolocationRequest(object sender, GeolocationRequestEventArgs e);
    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.FileDownloadBegin event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A FileDownloadBeginEventArgs that contains the event data.</param>
    public delegate void FileDownloadBeginEventHandler (object sender, FileDownloadBeginEventArgs e);

    public delegate void DangerousSiteDetected(object sender, EventArgs e);

    public delegate void FaviconAvailable(object sender, FaviconAvailableEventArgs e);
    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.NewWindowRequest event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A NewWindowRequestEventArgs that contains the event data.</param>
    public delegate void NewWindowRequestEventHandler (object sender, NewWindowRequestEventArgs e);
    public delegate void ProgressChangedEventHandler (object sender, ProgressChangesEventArgs e);
    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.NewWindowCreated event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A NewWindowCreatedEventArgs that contains the event data.</param>
    public delegate void NewWindowCreatedEventHandler (object sender, NewWindowCreatedEventArgs e);
    
    
    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.PopupCreated event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A NewWindowCreatedEventArgs that contains the event data.</param>
    public delegate void PopupCreatedEventHandler(object sender, NewWindowCreatedEventArgs e);
    
    
    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.ShowJavaScriptPormptBeforeUnload event. 
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A ShowJavaScriptAlertPanelEventArgs that contains the event data.</param>
    public delegate void ShowJavaScriptPromptBeforeUnloadEventHandler(object sender, ShowJavaScriptPromptBeforeUnloadEventArgs e);

    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.ShowJavaScriptAlertPanel event. 
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A ShowJavaScriptAlertPanelEventArgs that contains the event data.</param>
    public delegate void ShowJavaScriptAlertPanelEventHandler(object sender, ShowJavaScriptAlertPanelEventArgs e);

    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.ShowJavaScriptConfirmPanel event. 
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A ShowJavaScriptConfirmPanelEventArgs that contains the event data.</param>
    public delegate void ShowJavaScriptConfirmPanelEventHandler(object sender, ShowJavaScriptConfirmPanelEventArgs e);

    /// <summary>
    /// Represents the method that will handle the WebKitBrowser.ShowJavaScriptPromptPanel event. 
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A ShowJavaScriptPromptPanelEventArgs that contains the event data.</param>
    public delegate void ShowJavaScriptPromptPanelEventHandler(object sender, ShowJavaScriptPromptPanelEventArgs e);

    public delegate void CanGoBackChanged(object sender, CanGoBackChangedEventArgs e);
    public delegate void CanGoForwardChanged(object sender, CanGoForwardChangedEventArgs e);
    public delegate void MouseDidMoveOverElementEventHandler(object sender, MouseDidMoveOverElementEventArgs e);
    public delegate void ShowContextMenu(object sender, ShowContextMenuEventArgs e);
    public delegate void BrowserRightClick(object sender, BrowserRightClickEventArgs e);
    #endregion

    #region EventArgs classes

    public class GeolocationRequestEventArgs : EventArgs 
    {
        public bool Allow { get; set; }
        public string Url { get; internal set; }
        public IWebSecurityOrigin Origin { get; internal set; }
        public GeolocationRequestEventArgs(string u, bool a, IWebSecurityOrigin o)
        {
            Url = u;
            Allow = a;
            Origin = o;
        }
    }
    public class MouseDidMoveOverElementEventArgs : EventArgs
    {
        public Node Element;
        public MouseDidMoveOverElementEventArgs(Node el)
        {
            this.Element = el;
        }
    }
    public class HeadersAvailableEventArgs : EventArgs
    {
        private WebMutableURLRequest req { get; set; }
        public HeadersAvailableEventArgs(Uri url, WebMutableURLRequest reqest)
        {
            this.Url = url;
            this.req = reqest;
        
        }
        [Description("Returns the responded HTTP headers of the current URL. The first string resembles the Header's key and the second its value. For editing a value please use WebKitBrowser.SetHeader")]
        public Array Headers { get; internal set;}

        [Description("The URL that the headers belong to")]
        public Uri Url { get; internal set; }

        [Description("Adds a header with the specified value")]
        public void AddHeader(Header header)
        {
            req.addValue(header.Value, header.Field);
        }

        [Description("Sets a header's field with the specified value")]
        public void SetHeader(Header header, string value)
        {
            req.setValue(value, header.Field);
        }

        [Description("Get a value of a header")]
        public string GetHeaderValue(string field)
        {
            return req.valueForHTTPHeaderField(field);
        }
    }
    public class WebKitBrowserNavigatingEventArgs : CancelEventArgs
    {
        public WebKitBrowserNavigatingEventArgs(Uri url, string targetFrameName, string mime)
        {
            this.Url = url;
            this.TargetFrameName = targetFrameName;
            this.MimeType = mime;
            this.Cancel = false;
        }
        
        public string TargetFrameName { get; internal set;  }
        public string MimeType { get; internal set; }
        public Uri Url { get; internal set; }
    }

    public class WebKitBrowserStatusChangedEventArgs : EventArgs
    {
        public string StatusText = string.Empty;
        public WebKitBrowserStatusChangedEventArgs(string status)
        {
            StatusText = status;
        }
    }
    
    public class CanGoBackChangedEventArgs : EventArgs
    {
        public bool CanGoBack { get; internal set; }
        public CanGoBackChangedEventArgs(bool cangoback)
        {
            this.CanGoBack = cangoback;
        }
    }
    public class CanGoForwardChangedEventArgs : EventArgs
    {
        public bool CanGoForward { get; internal set; }
        public CanGoForwardChangedEventArgs(bool cangoforward)
        {
            this.CanGoForward = cangoforward;
        }
    }
    public class PluginFailedErrorEventArgs : EventArgs
    {
        public string ErrorDescription;
        public PluginFailedErrorEventArgs(string error)
        {
            this.ErrorDescription = error;
        }
    }
    public class ShowContextMenuEventArgs : EventArgs
    {
        public Point Location { get; internal set; }
        public WebKit.ContextMenuType MenuType { get; internal set; }
        public ShowContextMenuEventArgs(Point location, ContextMenuType type)
        {
            this.Location = location;
            this.MenuType = type;
        }
    }
    public class BrowserRightClickEventArgs : EventArgs
    {
        public BrowserRightClickEventArgs()
        {

        }
    }
    /// <summary>
    /// Provides data for the WebKitBrowser.Error event.
    /// </summary>
    public class WebKitBrowserErrorEventArgs : EventArgs
    {
        /// <summary>
        /// Gets a description of the error that occurred.
        /// </summary>
        public string Description { get; private set; }

        /// <summary>
        /// Initializes a new instance of the WebKitBrowserErrorEventArgs class.
        /// </summary>
        /// <param name="description">A description of the error that occurred.</param>
        public WebKitBrowserErrorEventArgs(string description)
        {
            this.Description = description;
        }
    }

    public class FormDelegateFormEventArgs : EventArgs
    {
        public DOM.HTMLElement Form { get; private set; }
        public IWebFormSubmissionListener Listener { get; private set; }
        public List<WebKitFormData> FormDataItems { get; private set; }
        public FormDelegateFormEventArgs(DOM.HTMLElement form, IWebFormSubmissionListener listener, List<WebKitFormData> l)
        {
            this.Form = form;
            this.Listener = listener;
            this.FormDataItems = l;
        }
    }
    public class FormDelegateElementEventArgs : EventArgs
    {
        public DOM.HTMLElement Element { get; private set; }
        public FormDelegateElementEventArgs(DOM.HTMLElement element)
        {
            this.Element = element;
        }
    }

    

    public class FaviconAvailableEventArgs : EventArgs
    {
        public System.Drawing.Icon Favicon;
        public FaviconAvailableEventArgs(System.Drawing.Icon icon)
        {
            this.Favicon = icon;
        }
    }
    /// <summary>
    /// Provides data for the WebKitBrowser.FileDownloadBegin event.
    /// </summary>
    public class FileDownloadBeginEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the url of the download.
        /// </summary>
        
        public string Url { get; private set; }

        /// <summary>
        /// Gets the suggested filename of the download.
        /// </summary>
        public string SuggestedFileName { get; private set; }

        /// <summary>
        /// Returns the Download object as WebKit.Interop.WebDownload
        /// </summary>
        public WebDownload Download { get; private set; }
        /// <summary>
        /// Initializes a new instance of the FileDownloadBeginEventArgs class.
        /// </summary>

        public FileDownloadBeginEventArgs(string url, string filename, WebDownload d)
        {
            this.Url = url;
            this.SuggestedFileName = filename;
            this.Download = d;
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.NewWindowRequest event. 
    /// </summary>
    public class NewWindowRequestEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the Url that the new window will attempt to navigate to.
        /// </summary>
        public string Url { get; private set; }

        /// <summary>
        /// Initializes a new instance of the NewWindowRequestEventArgs class.
        /// </summary>
        /// <param name="url">The Url that the new window will attempt to navigate to.</param>
        public NewWindowRequestEventArgs(string url)
        {
            this.Url = url;
        }
    }
    public class ProgressChangesEventArgs : EventArgs
    {
        public double MaxProgress = 1.0;
        public double EstimatedProgress = 0.0;
        public int Percent;
        public ProgressChangesEventArgs(double progress)
        {
            this.EstimatedProgress = progress;
            this.Percent = Convert.ToInt32(this.EstimatedProgress) / Convert.ToInt32(this.MaxProgress); 
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.NewWindowCreated event.
    /// </summary>
    public class NewWindowCreatedEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the WebKitBrowser showing the contents of the new window.
        /// </summary>
        public WebKitBrowser WebKitBrowser { get; private set; }

        /// <summary>
        /// Initializes a new instance of the NewWindowCreatedEventArgs class.
        /// </summary>
        /// <param name="browser">The WebKitBrowser showing the contents of the new window.</param>
        public NewWindowCreatedEventArgs(WebKitBrowser browser)
        {
            WebKitBrowser = browser;
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.ShowJavaScriptAlertPanel event.
    /// </summary>
    /// 
    public class ShowJavaScriptPromptBeforeUnloadEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the message to be shown in the alert panel.
        /// </summary>
        public string Message { get; private set; }

        /// <summary>
        /// Gets or sets the return value for the confirm panel.
        /// </summary>
        public bool ReturnValue { get; set; }

        /// <summary>
        /// Initializes a new instance of the ShowJavaScriptConfirmPanelEventArgs class.
        /// </summary>
        /// <param name="message">The message to be shown in the confirm panel.</param>
        public ShowJavaScriptPromptBeforeUnloadEventArgs(string message)
        {
            Message = message;
            ReturnValue = false;
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.ShowJavaScriptAlertPanel event.
    /// </summary>
    public class ShowJavaScriptAlertPanelEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the message to be shown in the alert panel.
        /// </summary>
        public string Message { get; private set; }

        /// <summary>
        /// Initializes a new instance of the ShowJavaScriptAlertPanelEventArgs class.
        /// </summary>
        /// <param name="message">The message to be shown in the alert panel.</param>
        public ShowJavaScriptAlertPanelEventArgs(string message)
        {
            Message = message;
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.ShowJavaScriptConfirmPanel event.
    /// </summary>
    public class ShowJavaScriptConfirmPanelEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the message to be shown in the confirm panel.
        /// </summary>
        public string Message { get; private set; }
        
        /// <summary>
        /// Gets or sets the return value for the confirm panel.
        /// </summary>
        public bool ReturnValue { get; set; }

        /// <summary>
        /// Initializes a new instance of the ShowJavaScriptConfirmPanelEventArgs class.
        /// </summary>
        /// <param name="message">The message to be shown in the confirm panel.</param>
        public ShowJavaScriptConfirmPanelEventArgs(string message)
        {
            Message = message;
            ReturnValue = false;
        }
    }

    /// <summary>
    /// Provides data for the WebKitBrowser.ShowJavaScriptPromptPanel event.
    /// </summary>
    public class ShowJavaScriptPromptPanelEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the message to be shown in the prompt panel.
        /// </summary>
        public string Message { get; private set; }

        /// <summary>
        /// Gets the default value of the prompt panel.
        /// </summary>
        public string DefaultValue { get; private set; }

        /// <summary>
        /// Gets or sets the return value for the prompt panel.
        /// </summary>
        public string ReturnValue { get; set; }

        /// <summary>
        /// Initializes a new instance of the ShowJavaScriptConfirmPanelEventArgs class.
        /// </summary>
        /// <param name="message">The message to be shown in the prompt panel.</param>
        /// <param name="defaultValue">The default value to be shown in the prompt panel.</param>
        public ShowJavaScriptPromptPanelEventArgs(string message, string defaultValue)
        {
            Message = message;
            DefaultValue = defaultValue;
            
            ReturnValue = defaultValue;
        }
    }
    #endregion
}
