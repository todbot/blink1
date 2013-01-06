using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;

// General CSS Management
// At early stages of development

namespace WebKit.CSS
{
    #region WebKitDOMCSSClasses
    public class WebKitDOMCSSValue 
    {
        private IDOMCSSValue cssvalue;
        internal WebKitDOMCSSValue(IDOMCSSValue style)
        {
            this.cssvalue = style;
        }
        public object InvokeScriptMethod(string name, object[] args)
        {
            object arguments = args;
            return cssvalue.callWebScriptMethod(name, ref arguments, args.Length);
        }

        public string CSSText
        {
            get
            {
                return cssvalue.cssText();
            }
            set
            {
                cssvalue.setCssText(value);   
            }
        }

        public short CSSValueType
        {
            get
            {
                return Convert.ToInt16(cssvalue.cssValueType());
            }
        }

        public object EvaluateWebScript(string script)
        {
            return cssvalue.evaluateWebScript(script);
        }

        public void RemoveWebScriptKey(string name)
        {
            cssvalue.removeWebScriptKey(name);
        }


        public void SetException(string description)
        {
            cssvalue.setException(description);
        }

        public void SetWebScriptValueAtIndex(int index, object val)
        {
            cssvalue.setWebScriptValueAtIndex(Convert.ToUInt32(index), val);
        }

        public string StringRepresentation
        {
            get
            {
                return cssvalue.stringRepresentation();
            }
        }

        public void ThrowException(string exceptionMessage)
        {
            cssvalue.throwException(exceptionMessage);
        }

        public object WebScriptValueAtIndex(int index)
        {
            return cssvalue.webScriptValueAtIndex(Convert.ToUInt32(index));
        }
    }
    public class WebKitDOMCSSDeclarationStyle 
    {
        private IDOMCSSStyleDeclaration sheet;
        internal WebKitDOMCSSDeclarationStyle(IDOMCSSStyleDeclaration style)
        {
            this.sheet = style;
        }
        public object InvokeScriptMethod(string name, object[] args)
        {
            object arguments = args;
            return sheet.callWebScriptMethod(name, ref arguments, args.Length);
        }

        public string CSSText
        {
            get
            {
                return sheet.cssText();
            }
            set
            {
                sheet.setCssText(value);
            }
        }

        public object EvaluateWebScript(string script)
        {
            return sheet.evaluateWebScript(script);
        }

        public WebKitDOMCSSValue GetPropertyCSSValue(string propertyName)
        {
            return new WebKitDOMCSSValue(sheet.getPropertyCSSValue(propertyName));
        }

        public object GetWebKitObject()
        {
            return sheet;
        }
        public string PropertyPriority(string propertyName)
        {
            return sheet.getPropertyPriority(propertyName);
        }

        public string PropertyValue(string propertyName)
        {
            return sheet.getPropertyValue(propertyName);
        }

        public string GetValue(int index)
        {
            return sheet.item(Convert.ToUInt32(index));
        }

        public int Count
        {
            get
            { return Convert.ToInt32(sheet.length()); }
        }

        public WebKitDOMCSSRule ParentRule
        {
            get
            {
                return new WebKitDOMCSSRule(sheet.parentRule());
            }
        }

        public void RemoveProperty(string propertyName)
        {
            sheet.removeProperty(propertyName);
        }

        public void RemoveWebScriptKey(string name)
        {
            sheet.removeWebScriptKey(name);
        }

        public void SetException(string description)
        {
            sheet.setException(description);
        }

        public void SetProperty(string propertyName, string value, string priority)
        {
            sheet.setProperty(propertyName, value, priority);
        }

        public void SetWebScriptValueAtIndex(int index, object val)
        {
            sheet.setWebScriptValueAtIndex(Convert.ToUInt32(index), val);
        }

        public string StringRepresentation
        {
            get { return sheet.stringRepresentation(); }
        }

        public void ThrowException(string exceptionMessage)
        {
            sheet.throwException(exceptionMessage);
        }

        public object WebScriptValueAtIndex(int index)
        {
            return sheet.webScriptValueAtIndex(Convert.ToUInt32(index));
        }
    }
    public class WebKitDOMCSSStyle
    {
        private IDOMCSSStyleSheet sheet;
        internal WebKitDOMCSSStyle(IDOMCSSStyleSheet style)
        {
            this.sheet = style;
        }
        public object InvokeScriptMethod(string name, object[] args)
        {
            object arguments = args;
            return sheet.callWebScriptMethod(name, ref arguments, args.Length);
        }

        public WebKitDOMCSSRuleList CSSRules
        {
            get
            {
                return new WebKitDOMCSSRuleList(sheet.cssRules());
            }
        }

        public void DeleteRule(int index)
        {
            sheet.deleteRule(Convert.ToUInt32(index));
        }

        public object EvaluateWebScript(string script)
        {
            return sheet.evaluateWebScript(script);
        }
        public object GetWebKitObject()
        {
            return sheet;
        }
        public void InsertRule(string rule, int index)
        {
            sheet.insertRule(rule, Convert.ToUInt32(index));
        }

        public WebKitDOMCSSRule OwnerRule
        {
            get
            {
                return new WebKitDOMCSSRule(sheet.ownerRule());
            }
        }

        public void RemoveWebScriptKey(string name)
        {
            sheet.removeWebScriptKey(name);
        }

        public void SetException(string description)
        {
            sheet.setException(description);
        }

        public void SetWebScriptValueAtIndex(int index, object val)
        {
            sheet.setWebScriptValueAtIndex(Convert.ToUInt32(index), val);
        }

        public string StringRepresentation
        {
            get
            {
                return sheet.stringRepresentation();
            }
        }

        public void ThrowException(string exceptionMessage)
        {
            sheet.throwException(exceptionMessage);
        }

        public object WebScriptValueAtIndex(int index)
        {
            return sheet.webScriptValueAtIndex(Convert.ToUInt32(index));
        }
    }
    public class WebKitDOMCSSRuleList
    {
        private IDOMCSSRuleList list;
        internal WebKitDOMCSSRuleList(IDOMCSSRuleList csslist)
        {
            this.list = csslist;
        }

        public object InvokeScriptMethod(string name, object[] args)
        {
            object arguments = args;
            return list.callWebScriptMethod(name, ref arguments, args.Length);
        }

        public object EvaluateWebScript(string script)
        {
            return list.evaluateWebScript(script);
        }

        public WebKitDOMCSSRule GetItem(int index)
        {
            return new WebKitDOMCSSRule(list.item(Convert.ToUInt32(index)));
        }

        public int Count
        {
            get { return Convert.ToInt32(list.length()); }
        }
        public object GetWebKitObject()
        {
            return list;
        }
        public void RemoveWebScriptKey(string name)
        {
            list.removeWebScriptKey(name);
        }

        public void SetException(string description)
        {
            list.setException(description);
        }

        public void SetWebScriptValueAtIndex(int index, object val)
        {
            list.setWebScriptValueAtIndex(Convert.ToUInt32(index), val);
        }

        public string StringRepresentation
        {
            get
            {
                return list.stringRepresentation();
            }
        }

        public void ThrowException(string exceptionMessage)
        {
            list.throwException(exceptionMessage);
        }

        public object WebScriptValueAtIndex(int index)
        {
            return list.webScriptValueAtIndex(Convert.ToUInt32(index));
        }
    }
    public class WebKitDOMCSSRule
    {
        private IDOMCSSRule _rule;
        internal WebKitDOMCSSRule(IDOMCSSRule rule)
        {
            this._rule = rule;
        }
        public object InvokeScriptMethod(string name, object[] args)
        {
            object arguments = args;
            return _rule.callWebScriptMethod(name, ref arguments, args.Length);
        }

        public string CSSText
        {
            get
            {
                return _rule.cssText();
            }
            set
            {
                _rule.setCssText(value);
            }
        }

        public object EvaluateWebScript(string script)
        {
            return _rule.evaluateWebScript(script);
        }
        public object GetWebKitObject()
        {
            return _rule;
        }
        public WebKitDOMCSSRule ParentRule
        {
            get
            {
                return new WebKitDOMCSSRule(_rule.parentRule());
            }
        }

        public WebKitDOMCSSStyle ParentStyleSheet
        {
            get
            {
                return new WebKitDOMCSSStyle(_rule.parentStyleSheet());
            }
        }
        public void RemoveWebScriptKey(string name)
        {
            _rule.removeWebScriptKey(name);
        }

        public void SetException(string description)
        {
            _rule.setException(description);
        }

        public void SetWebScriptValueAtIndex(int index, object val)
        {
            _rule.setWebScriptValueAtIndex(Convert.ToUInt32(index), val);
        }

        public string StringRepresentation
        {
            get
            {
                return _rule.stringRepresentation();
            }
        }

        public void ThrowException(string exceptionMessage)
        {
            _rule.throwException(exceptionMessage);
        }

        public short Type
        {
            get
            {
                return Convert.ToInt16(_rule.type());
            }
        }

        public object WebScriptValueAtIndex(int index)
        {
            return _rule.webScriptValueAtIndex(Convert.ToUInt32(index));
        }
    }
    #endregion

    public class WebKitDOMCSSManager
    {
        private WebKitBrowser browser;
        public WebKitDOMCSSManager(WebKitBrowser owner)
        {
            this.browser = owner;
        }
        public WebKitDOMCSSDeclarationStyle GetStyleFromElement(DOM.Element element, string pseudoElement = "") // this feature has not been implemented yet
        {
            return new WebKitDOMCSSDeclarationStyle(browser.WebView.computedStyleForElement((IDOMElement)element.GetWebKitObject(), pseudoElement));
        }
        public void SetPageStyleSheetFromWeb(string url)
        {
            browser.WebView.preferences().setUserStyleSheetEnabled(1);
            browser.WebView.preferences().setUserStyleSheetLocation(url);
        }
        public void SetPageDefaultStyleSheet()
        {
            browser.WebView.preferences().setUserStyleSheetEnabled(0);
            browser.WebView.preferences().setUserStyleSheetLocation(string.Empty);
        }
        public void SetPageStyleSheetFromLocalFile(string filename)
        {
            string url = "file:///" + filename.Replace(" ", "%20").Replace(@"\", "/"); ;
            browser.WebView.preferences().setUserStyleSheetEnabled(1);
            browser.WebView.preferences().setUserStyleSheetLocation(url);
        }
        public void SetPageStyleSheetFromCSSDeclarationStyle(WebKitDOMCSSDeclarationStyle style) // not tested but should work
        {
            string path = Environment.GetFolderPath(Environment.SpecialFolder.InternetCache) + "temp" + new Random().Next(10000).ToString() + ".css";
            System.IO.File.WriteAllText(path, style.CSSText);
            SetPageStyleSheetFromLocalFile(path);
            
        }
        public WebKitDOMCSSDeclarationStyle GetDocumentStyle // this feature has not been implemented yet
        {
            get { return GetStyleFromElement(DOM.Element.Create((browser.Document.GetWebKitObject() as IDOMDocument).documentElement())); }
        }
    }
}
