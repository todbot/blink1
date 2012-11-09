using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;

namespace WebKit
{
    public class WebKitFormData
    {
        public DOM.HTMLElement Element;
        public string Value;
        public WebKitFormData(DOM.HTMLElement el, string value)
        {
            this.Element = el;
            this.Value = value;
        }
    }
}
