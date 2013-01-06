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

// Handles frame-level events that occur from loading a web page.  More info at
// http://developer.apple.com/documentation/Cocoa/Reference/WebKit/Protocols/IWebFrameLoadDelegate_Protocol

using System;
using System.Collections.Generic;
using System.Text;
using WebKit;
using WebKit.Interop;
using System.Diagnostics;

namespace WebKit
{
    // Delegate definitions IWebFrameLoadDelegate events
    internal delegate void DidCancelClientRedirectForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void DidChangeLocationWithinPageForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void DidCommitLoadForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void DidFailLoadWithErrorEvent(WebView WebView, IWebError error, IWebFrame frame);
    internal delegate void DidFailProvisionalLoadWithErrorEvent(WebView WebView, IWebError error, IWebFrame frame);
    internal delegate void DidFinishLoadForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void DidRecieveIconEvent(WebView WebView, int hBitMap, IWebFrame frame);
    internal delegate void DidRecieveServerRedirectForProvisionalLoadForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void DidRecieveTitleEvent(WebView WebView, string title, IWebFrame frame);
    internal delegate void DidStartProvisionalLoadForFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void WillCloseFrameEvent(WebView WebView, IWebFrame frame);
    internal delegate void WillPerformClientRedirectToURLEvent(WebView WebView, string url, double delaySeconds, DateTime fireDate, IWebFrame frame);
    internal delegate void WindowScriptObjectAvailableEvent(WebView WebView, IntPtr context, IntPtr windowScriptObject);
    internal delegate void DidClearWindowObjectEvent(WebView WebView, IntPtr context, IntPtr windowScriptObject, IWebFrame frame);

    internal class WebFrameLoadDelegate : IWebFrameLoadDelegate
    {
        public event DidCancelClientRedirectForFrameEvent DidCancelClientRedirectForFrame = delegate { };
        public event DidChangeLocationWithinPageForFrameEvent DidChangeLocationWithinPageForFrame = delegate { };
        public event DidCommitLoadForFrameEvent DidCommitLoadForFrame = delegate { };
        public event DidFailLoadWithErrorEvent DidFailLoadWithError = delegate { };
        public event DidFailProvisionalLoadWithErrorEvent DidFailProvisionalLoadWithError = delegate { };
        public event DidFinishLoadForFrameEvent DidFinishLoadForFrame = delegate { };
        public event DidRecieveIconEvent DidRecieveIcon = delegate { };
        public event DidRecieveServerRedirectForProvisionalLoadForFrameEvent DidRecieveServerRedirectForProvisionalLoadForFrame = delegate { };
        public event DidRecieveTitleEvent DidRecieveTitle = delegate { };
        public event DidStartProvisionalLoadForFrameEvent DidStartProvisionalLoadForFrame = delegate { };
        public event WillCloseFrameEvent WillCloseFrame = delegate { };
        public event WillPerformClientRedirectToURLEvent WillPerformClientRedirectToURL = delegate { };
        public event WindowScriptObjectAvailableEvent WindowScriptObjectAvailable = delegate { };
        public event DidClearWindowObjectEvent DidClearWindowObject = delegate { };

        #region IWebFrameLoadDelegate Members

        public void didCancelClientRedirectForFrame(WebView WebView, webFrame frame)
        {
            DidCancelClientRedirectForFrame(WebView, frame);
        }

        public void didChangeLocationWithinPageForFrame(WebView WebView, webFrame frame)
        {
            DidChangeLocationWithinPageForFrame(WebView, frame);
        }

        public void didCommitLoadForFrame(WebView WebView, webFrame frame)
        {
            DidCommitLoadForFrame(WebView, frame); 
        }

        public void didFailLoadWithError(WebView WebView, WebError error, webFrame forFrame)
        {
            DidFailLoadWithError(WebView, error, forFrame);
        }

        public void didFailProvisionalLoadWithError(WebView WebView, WebError error, webFrame frame)
        {
            DidFailProvisionalLoadWithError(WebView, error, frame);
        }

        public void didFinishLoadForFrame(WebView WebView, webFrame frame)
        {
            DidFinishLoadForFrame(WebView, frame);
        }

        public void didReceiveIcon(WebView WebView, int hBitmap, webFrame frame)
        {
            DidRecieveIcon(WebView, hBitmap, frame);
        }

        public void didReceiveServerRedirectForProvisionalLoadForFrame(WebView WebView, webFrame frame)
        {
            DidRecieveServerRedirectForProvisionalLoadForFrame(WebView, frame);
        }

        public void didReceiveTitle(WebView WebView, string title, webFrame frame)
        {
            DidRecieveTitle(WebView, title, frame);
        }

        public void didStartProvisionalLoadForFrame(WebView WebView, webFrame frame)
        {
            DidStartProvisionalLoadForFrame(WebView, frame);
        }

        public void willCloseFrame(WebView WebView, webFrame frame)
        {
            WillCloseFrame(WebView, frame);
        }

        public void willPerformClientRedirectToURL(WebView WebView, string url, double delaySeconds, DateTime fireDate, webFrame frame)
        {
            WillPerformClientRedirectToURL(WebView, url, delaySeconds, fireDate, frame);
        }

        public void windowScriptObjectAvailable(WebView WebView, IntPtr context, IntPtr windowScriptObject)
        {
            WindowScriptObjectAvailable(WebView, context, windowScriptObject);
        }

        public void didClearWindowObject(WebView WebView, IntPtr context, IntPtr windowScriptObject, webFrame frame)
        {
            DidClearWindowObject(WebView, context, windowScriptObject, frame);
        }

        #endregion
    }
}
