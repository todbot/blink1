using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;
using WebKit.DOM;

/* WebFormDelegate provides the user with data and information about
 * form submitting and form data editing.
 *
 * Events available in the WebKitBrowser class
 */

namespace WebKit
{
    internal delegate void TextChangedInArea(HTMLElement element);
    internal delegate void TextChangedInField(HTMLElement element);
    internal delegate void TextFieldBeginEditing(HTMLElement element);
    internal delegate void TextFieldEndEditing(HTMLElement element);
    internal delegate void SubmitForm(HTMLElement form, IWebFormSubmissionListener listener);
    class WebFormDelegate : IWebFormDelegate 
    {
        public event TextChangedInArea TextChangedInArea = delegate { };
        public event TextChangedInField TextChangedInField = delegate { };
        public event TextFieldBeginEditing TextFieldBeginEditing = delegate { };
        public event TextFieldEndEditing TextFieldEndEditing = delegate { };
        public event SubmitForm SubmitForm = delegate { };
        private WebKitBrowser _browser;
        public WebFormDelegate(WebKitBrowser owner)
        {
            this._browser = owner;
        }
        public int doPlatformCommand(IDOMHTMLInputElement element, string command, webFrame frame)
        {
            return 0;
        }

        public int shouldHandleEvent(IDOMHTMLInputElement element, IntPtr @event, webFrame frame)
        {
            throw new NotImplementedException();
        }

        public void textDidChangeInTextArea(IDOMHTMLTextAreaElement element, webFrame frame)
        {
            TextChangedInArea((HTMLElement)HTMLElement.Create(element));
        }

        public void textDidChangeInTextField(IDOMHTMLInputElement element, webFrame frame)
        {
            TextChangedInField((HTMLElement)HTMLElement.Create(element));
        }

        public void textFieldDidBeginEditing(IDOMHTMLInputElement element, webFrame frame)
        {
            TextFieldBeginEditing((HTMLElement)HTMLElement.Create(element));
        }

        public void textFieldDidEndEditing(IDOMHTMLInputElement element, webFrame frame)
        {
            TextFieldEndEditing((HTMLElement)HTMLElement.Create(element));
        }
        
        public void willSubmitForm(webFrame frame, webFrame sourceFrame, IDOMElement form, CFDictionaryPropertyBag values, IWebFormSubmissionListener listener)
        {
            SubmitForm((HTMLElement)HTMLElement.Create(form),listener);
        }
    }
}
