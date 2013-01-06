using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using WebKit.Interop;

namespace WebKit.Appearance
{
    internal delegate void ChangeFontFamily(string newfamily);
    internal delegate void ChangeFontSize(int newsize);
    public class AppearanceSettings
    {
        public WebKitBrowser Browser;

        public AppearanceSettings(WebKitBrowser browser)
        {
            this.Browser = browser;
        }

        private WebKitFont dfont { get; set; }
        public WebKitFont DefaultFont
        {
            get 
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return new WebKitFont(Browser.WebView.preferences().standardFontFamily(), Browser.WebView.preferences().defaultFontSize());
            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setStandardFontFamily(value.Family);
                Browser.WebView.preferences().setDefaultFontSize(value.Size);
                dfont = value;
                dfont.FamilyChanged += new ChangeFontFamily(dvalue_FamilyChanged);
                dfont.SizeChanged += new ChangeFontSize(dvalue_SizeChanged);
            }
        }

        void dvalue_SizeChanged(int newsize)
        {
            Browser.WebView.preferences().setDefaultFontSize(newsize);
        }

        void dvalue_FamilyChanged(string newfamily)
        {
            if (Browser == null)
                throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
            Browser.WebView.preferences().setStandardFontFamily(newfamily);
        }

        private WebKitFont ffont { get; set; }
        public WebKitFont FixedFont
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                
                return new WebKitFont(Browser.WebView.preferences().fixedFontFamily(), Browser.WebView.preferences().defaultFixedFontSize());
            }
            set 
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setFixedFontFamily(value.Family);
                Browser.WebView.preferences().setDefaultFixedFontSize(value.Size);
                ffont = value;
                ffont.FamilyChanged += new ChangeFontFamily(ffont_FamilyChanged);
                ffont.SizeChanged += new ChangeFontSize(ffont_SizeChanged);
            }
        }

        void ffont_SizeChanged(int newsize)
        {
            if (Browser == null)
                throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
            Browser.WebView.preferences().setDefaultFixedFontSize(newsize);
        }

        void ffont_FamilyChanged(string newfamily)
        {
            if (Browser == null)
                throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
            Browser.WebView.preferences().setFixedFontFamily(newfamily);
        }
        public string SansSerifFontFamily
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return Browser.WebView.preferences().sansSerifFontFamily();
            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setSansSerifFontFamily(value);
            }
        }

        public string SerifFontFamily
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return Browser.WebView.preferences().serifFontFamily();
            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setSerifFontFamily(value);
            }
        }

        public string FantasyFontFamily
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return Browser.WebView.preferences().fantasyFontFamily();
            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setFantasyFontFamily(value);
            }
        }

        public float MinimumFontSize
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return (float)Browser.WebView.preferences().minimumFontSize();
            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setMinimumFontSize(Convert.ToInt32(value));
            }
        }

        public float MinimumLogicalFontSize
        {
            get
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                return (float)Browser.WebView.preferences().minimumLogicalFontSize();

            }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setMinimumLogicalFontSize(Convert.ToInt32(value));
            }
        }

        public FontSmoothingType FontSmoothing
        {
            get { return Browser.WebView.preferences().fontSmoothing(); }
            set
            {
                if (Browser == null)
                    throw new InvalidOperationException("The correspondent WebKitBrowser object has not been set or is null.");
                Browser.WebView.preferences().setFontSmoothing(value);
            }
        }
    }
    public class WebKitFont
    {
        internal event ChangeFontFamily FamilyChanged = delegate { };
        internal event ChangeFontSize SizeChanged = delegate { };

        private int _s;
        private string _f;
        public string Family { get { return _f; } set { _f = value; FamilyChanged(value); } }
        public int Size 
        {
            get { return _s;  } 
            set 
            {
                _s = value;
                SizeChanged(value); 
            } 
        }

        public Font ToSystemFont()
        {
            return new Font(Family, Size);
        }

        public WebKitFont(string Family, int Size)
        {
            this._f = Family;
            this._s = Size;
        }

        public WebKitFont(Font font)
        {
            this._f = font.FontFamily.ToString();
            this._s = (int)font.Size;
        }
    }
}
