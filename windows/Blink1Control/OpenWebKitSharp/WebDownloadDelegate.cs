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

// TODO: currently, calling WebDownload.cancel() in the 'decideDestinationWithSuggestedFilename'
// method causes a crash - I've seen examples of this method being used here with no problems.
// Find out whether this is a bug in webkit or what else is going on. Various useful information at 
// http://developer.apple.com/documentation/Cocoa/Conceptual/URLLoadingSystem/Tasks/UsingNSURLDownload.html
// Also note that currently (as of revision ~45680), download stuff is not implemented in the 
// cairo build of WebKit yet.


using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;
using WebKit.DOM;
using System.Windows.Forms;
using System.Diagnostics;

namespace WebKit
{
    internal delegate void DecideDestinationWithSuggestedFilenameEvent(WebDownload download, string fileName);
    internal delegate void DidBeginEvent(WebDownload download);
    internal delegate void DidCancelAuthenticationChallengeEvent(WebDownload download, IWebURLAuthenticationChallenge challenge);
    internal delegate void DidCreateDestinationEvent(WebDownload download, string destination);
    internal delegate void DidFailWithErrorEvent(WebDownload download, WebError error);
    internal delegate void DidFinishEvent(WebDownload download);
    internal delegate void DidReceiveAuthenticationChallengeEvent(WebDownload download, IWebURLAuthenticationChallenge challenge);
    internal delegate void DidReceiveDataOfLengthEvent(WebDownload download, uint length);
    internal delegate void DidReceiveResponseEvent(WebDownload download, IWebURLResponse response);
    internal delegate int ShouldDecodeSourceDataOfMIMETypeEvent(WebDownload download, string encodingType);
    internal delegate void WillResumeWithResponseEvent(WebDownload download, IWebURLResponse response, long fromByte);
    internal delegate void WillSendRequestEvent(WebDownload download, WebMutableURLRequest request, IWebURLResponse redirectResponse, out WebMutableURLRequest finalRequest);

    internal class WebDownloadDelegate : IWebDownloadDelegate
    {
        public event DecideDestinationWithSuggestedFilenameEvent DecideDestinationWithSuggestedFilename = delegate { };
        public event DidBeginEvent DidBegin = delegate { };
        public event DidCancelAuthenticationChallengeEvent DidCancelAuthenticationChallenge = delegate { };
        public event DidCreateDestinationEvent DidCreateDestination = delegate { };
        public event DidFailWithErrorEvent DidFailWithError = delegate { };
        public event DidFinishEvent DidFinish = delegate { };
        public event DidReceiveAuthenticationChallengeEvent DidReceiveAuthenticationChallenge = delegate { };
        public event DidReceiveDataOfLengthEvent DidReceiveDataOfLength = delegate { };
        public event DidReceiveResponseEvent DidReceiveResponse = delegate { };
        public event WillResumeWithResponseEvent WillResumeWithResponse = delegate { };

        public WebDownloadDelegate(WebKitBrowser browser)
        {
            this.parent = browser;
        }
        private WebKitBrowser parent;
        #region IWebDownloadDelegate Members

        public void decideDestinationWithSuggestedFilename(WebDownload download, string fileName)
        {
            DecideDestinationWithSuggestedFilename(download, fileName);
        }

        public void didBegin(WebDownload download)
        {
            DidBegin(download);
        }

        public void didCancelAuthenticationChallenge(WebDownload download, IWebURLAuthenticationChallenge challenge)
        {
            DidCancelAuthenticationChallenge(download, challenge);
        }

        public void didCreateDestination(WebDownload download, string destination)
        {
            DidCreateDestination(download, destination);
        }

        public void didFailWithError(WebDownload download, WebError error)
        {
            DidFailWithError(download, error);
        }

        public void didFinish(WebDownload download)
        {
            DidFinish(download);
        }

        public void didReceiveAuthenticationChallenge(WebDownload download, IWebURLAuthenticationChallenge challenge)
        {
            DidReceiveAuthenticationChallenge(download, challenge);
        }

        public void didReceiveDataOfLength(WebDownload download, uint length)
        {
            DidReceiveDataOfLength(download, length);
        }

        public void didReceiveResponse(WebDownload download, WebURLResponse response)
        {  
            DidReceiveResponse(download, response);
        }

        public int shouldDecodeSourceDataOfMIMEType(WebDownload download, string encodingType)
        {
            // todo
            return 0;
        }

        public void willResumeWithResponse(WebDownload download, WebURLResponse response, long fromByte)
        {
            WillResumeWithResponse(download, response, fromByte);
        }

        public void willSendRequest(WebDownload download, WebMutableURLRequest request, WebURLResponse redirectResponse, out WebMutableURLRequest finalRequest)
        {
            finalRequest = request;
        }

        #endregion
    }
}
