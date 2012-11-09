using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;
using WebKit.DOM;

namespace WebKit
{
    class CustomUndoSystem
    {
        internal WebKitBrowser Owner { get; set; }
        internal Dictionary<string, List<string>> UndoDeedsN = new Dictionary<string, List<string>>();
        internal Dictionary<string, List<string>> UndoDeedsI = new Dictionary<string, List<string>>();
        internal IDOMHTMLInputElement node { get; set; }
        public Element Target
        {
            get { return Element.Create(node); }
        }
        public CustomUndoSystem(WebKitBrowser browser)
        {
            Owner = browser;
            Owner.Navigating += new WebKitBrowserNavigatingEventHandler(Owner_Navigating);
            Owner.TextChangeInArea += new TextDidChangeInArea(Owner_TextChangeInArea);
            Owner.TextChangeInField += new TextDidChangeInField(Owner_TextChangeInField);
        }
        //public bool CanUndo()
        //{
        //    if (node is IDOMHTMLInputElement)
        //    {
        //        try
        //        {
        //            string i = UndoDeedsI[node.GetAttribute("id")][0];
        //            return true;
        //        }
        //        catch
        //        {
        //            try
        //            {
        //                string i = UndoDeedsN[node.GetAttribute("name")][0];
        //                return true;
        //            }
        //            catch { return false; }
        //        }
        //    }
        //    else
        //    {
        //        return false;
        //    }
        //}
        //public bool CanRedo()
        //{
        //    if (node is IDOMHTMLInputElement)
        //    {
        //        try
        //        {
        //            string i = UndoDeedsI[node.GetAttribute("id")][0];
        //            return true;
        //        }
        //        catch
        //        {
        //            try
        //            {
        //                string i = UndoDeedsN[node.GetAttribute("name")][0];
        //                return true;
        //            }
        //            catch { return false; }
        //        }
        //    }
        //    else
        //    {
        //        return false;
        //    }
        //}
        public void Undo()
        {
            if (node is IDOMHTMLInputElement)
            {
                IDOMHTMLElement el = (IDOMHTMLElement)node;
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                {
                    int curind = UndoDeedsI[el.getAttribute("id")].IndexOf(node.value());
                    if (curind != 0)
                        (node as IDOMHTMLInputElement).setValue(UndoDeedsI[el.getAttribute("id")][curind - 1]);
                }
                else
                {
                    if (!string.IsNullOrEmpty(el.getAttribute("name")))
                    {
                        int curind = UndoDeedsN[el.getAttribute("name")].IndexOf(((IDOMHTMLInputElement)node).value());
                        if (curind != 0)
                            (node as IDOMHTMLInputElement).setValue(UndoDeedsN[el.getAttribute("name")][curind - 1]);
                     }
                }
            }
        }

        public void Redo()
        {
            if (node is IDOMHTMLInputElement)
            {
                IDOMHTMLElement el = (IDOMHTMLElement)node;
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                {
                    int curind = UndoDeedsI[el.getAttribute("id")].IndexOf(((IDOMHTMLInputElement)node).value());
                    if (curind < UndoDeedsN[el.getAttribute("name")].Count)
                        (node as IDOMHTMLInputElement).setValue(UndoDeedsI[el.getAttribute("id")][curind + 1]);
                }
                else
                {
                    if (!string.IsNullOrEmpty(el.getAttribute("name")))
                    {
                        int curind = UndoDeedsN[el.getAttribute("name")].IndexOf(((IDOMHTMLInputElement)node).value());
                        if (curind < UndoDeedsN[el.getAttribute("name")].Count)
                            (node as IDOMHTMLInputElement).setValue(UndoDeedsN[el.getAttribute("name")][curind + 1]);
                    }
                }
            }
        }
        void Owner_Navigating(object sender, WebKitBrowserNavigatingEventArgs e)
        {
            UndoDeedsN.Clear();
            UndoDeedsI.Clear();
        }

        void Owner_TextChangeInField(object sender, FormDelegateElementEventArgs e)
        {
            try
            {
                IDOMHTMLElement el = (IDOMHTMLElement)e.Element.GetWebKitObject();
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                    UndoDeedsI[el.getAttribute("id")].Add(((IDOMHTMLInputElement)e.Element.GetWebKitObject()).value()); 
                else
                    UndoDeedsN[el.getAttribute("name")].Add(((IDOMHTMLInputElement)e.Element.GetWebKitObject()).value());                 
            }
            catch
            {
                IDOMHTMLElement el = (IDOMHTMLElement)e.Element.GetWebKitObject();
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                {
                    UndoDeedsI.Add(el.getAttribute("id"), new List<string>());
                    UndoDeedsI[el.getAttribute("id")].Add(el.getAttribute("value"));
                }
                else
                {
                    UndoDeedsN.Add(el.getAttribute("name"), new List<string>());
                    UndoDeedsN[el.getAttribute("name")].Add(el.getAttribute("value"));
                }
            }
        }

        void Owner_TextChangeInArea(object sender, FormDelegateElementEventArgs e)
        {
            try
            {
                IDOMHTMLElement el = (IDOMHTMLElement)e.Element.GetWebKitObject();
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                    UndoDeedsI[el.getAttribute("id")].Add(((IDOMHTMLInputElement)e.Element.GetWebKitObject()).value());
                else
                    UndoDeedsN[el.getAttribute("name")].Add(((IDOMHTMLInputElement)e.Element.GetWebKitObject()).value());
            }
            catch
            {
                IDOMHTMLElement el = (IDOMHTMLElement)e.Element.GetWebKitObject();
                if (!string.IsNullOrEmpty(el.getAttribute("id")))
                {
                    UndoDeedsI.Add(el.getAttribute("id"), new List<string>());
                    UndoDeedsI[el.getAttribute("id")].Add(el.getAttribute("value"));
                }
                else
                {
                    UndoDeedsN.Add(el.getAttribute("name"), new List<string>());
                    UndoDeedsN[el.getAttribute("name")].Add(el.getAttribute("value"));
                }
            }
        }
    }
}
