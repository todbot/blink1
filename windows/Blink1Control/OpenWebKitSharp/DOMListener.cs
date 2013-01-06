using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;

namespace WebKit
{
    internal class DOMListener : WebKit.DOM.OpenWebKitSharpDOMEvents
    {
        private DOMNode obj;

        internal DOMListener(DOMNode node)
        {
            if (node != null)
            {
                obj = node;

                IDOMEventListenerForFocus f = new IDOMEventListenerForFocus(node);
                f.Fired += new EventHandler(f_Fired);

                IDOMEventListenerForClicks c = new IDOMEventListenerForClicks(node);
                c.Fired += new EventHandler(c_Fired);

                IDOMEventListenerForkeydown kd = new IDOMEventListenerForkeydown(node);
                kd.Fired += new EventHandler(kd_Fired);

                IDOMEventListenerForkeyup ku = new IDOMEventListenerForkeyup(node);
                ku.Fired += new EventHandler(ku_Fired);

                IDOMEventListenerFormouseover mo = new IDOMEventListenerFormouseover(node);
                mo.Fired += new EventHandler(mo_Fired);

                IDOMEventListenerFormousedown md = new IDOMEventListenerFormousedown(node);
                md.Fired += new EventHandler(md_Fired);

                IDOMEventListenerFormouseup mu = new IDOMEventListenerFormouseup(node);
                mu.Fired += new EventHandler(mu_Fired);
            }
        }

        void mu_Fired(object sender, EventArgs e)
        {
            MouseUp(obj, new EventArgs());
        }

        void md_Fired(object sender, EventArgs e)
        {
            MouseDown(obj, new EventArgs());
        }

        void mo_Fired(object sender, EventArgs e)
        {
            MouseOver(obj, new EventArgs());
        }

        void ku_Fired(object sender, EventArgs e)
        {
            KeyUp(obj, new EventArgs());
        }

        void kd_Fired(object sender, EventArgs e)
        {
            KeyDown(obj, new EventArgs());
        }

        void c_Fired(object sender, EventArgs e)
        {
            MouseClick(obj, new EventArgs());
        }

        void f_Fired(object sender, EventArgs e)
        {
            Focus(obj, new EventArgs());
        }

        public event EventHandler MouseClick = delegate { };

        public event EventHandler MouseOver = delegate { };

        public event EventHandler KeyUp = delegate { };

        public event EventHandler KeyDown = delegate { };

        public event EventHandler Focus = delegate { };

        public event EventHandler MouseUp = delegate { };

        public event EventHandler MouseDown = delegate { };
    }
    internal class IDOMEventListenerForClicks : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerForClicks(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("click", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerForFocus : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerForFocus(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("focus", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerFormouseover : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerFormouseover(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("mouseover", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerForkeyup : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerForkeyup(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("keyup", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerFormouseup : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerFormouseup(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("mouseup", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerFormousedown : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerFormousedown(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("mousedown", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
    internal class IDOMEventListenerForkeydown : IDOMEventListener
    {
        internal event EventHandler Fired = delegate { };
        public IDOMEventListenerForkeydown(DOMNode nd)
        {
            ((IDOMEventTarget)nd).addEventListener("keydown", this, 0);
        }
        public object callWebScriptMethod(string name, ref object args, int cArgs)
        {
            throw new NotImplementedException();
        }

        public object evaluateWebScript(string script)
        {
            throw new NotImplementedException();
        }

        public void handleEvent(IDOMEvent evt)
        {
            Fired(this, new EventArgs());
        }

        public void removeWebScriptKey(string name)
        {
            throw new NotImplementedException();
        }

        public void setException(string description)
        {
            throw new NotImplementedException();
        }

        public void setWebScriptValueAtIndex(uint index, object val)
        {
            throw new NotImplementedException();
        }

        public string stringRepresentation()
        {
            throw new NotImplementedException();
        }

        public int throwException(string exceptionMessage)
        {
            throw new NotImplementedException();
        }

        public object webScriptValueAtIndex(uint index)
        {
            throw new NotImplementedException();
        }
    }
}
