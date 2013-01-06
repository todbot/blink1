using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using WebKit;
using WebKit.Interop;

namespace WebKit
{
    public class GlobalPreferences
    {
        private static string _DownloadsFolder = string.Empty;
        private static bool _WillHandleDownloadsManually = false;
        private bool _enableWebSecurity = true;
        private static string _ApplicationName = "OpenWebKitSharp";
        private  WebKitCookieStorageAcceptPolicy _CookiesPolicy;
        private  bool _WillGetFavicon = true;
        private  bool _AutomaticallyCheckForPhishingBehavior = true;
        private bool _useFontSmoothing = false;
        private bool _dnsprefetch = true;
        private  Encoding _initialEncoding = null;
        private static string _WebKitPath = System.Windows.Forms.Application.StartupPath;
        internal WebKitBrowser browser;
        internal static bool HasRunTerminal = false;
        internal WebPreferencesClass preferences { get { return (WebPreferencesClass)browser.WebView.preferences(); } set { } }
        public bool EnableWebKitWebSecurity
        {
            get { return _enableWebSecurity; }
            set 
            {
                _enableWebSecurity = value;
                preferences.setWebSecurityEnabled(Convert.ToInt32(value));
            }
        }

        public bool IgnoreSSLErrors { get; set; }

        public bool EnableDNSPrefetching
        {
            get { return _dnsprefetch; }
            set { browser.Preferences.preferences.setDNSPrefetchingEnabled(Convert.ToInt32(value)); _dnsprefetch = value; }
        }
        public GlobalPreferences(WebKitBrowser browser)
        {
            WebKitBrowser.activationContext.Activate();
            this.browser = browser;
            WebKitBrowser.activationContext.Deactivate();
        }
        public static string WebKitPath
        {
            get
            {
                return _WebKitPath;
            }
            set
            {
                _WebKitPath = value;
            }
        }

        public bool UseFontSmoothing
        {
            get { return _useFontSmoothing; }
            set 
            {
                _useFontSmoothing = value;
                if (value == true)
                {
                    preferences.setFontSmoothing(FontSmoothingType.FontSmoothingTypeStrong);
                }
                else
                    preferences.setFontSmoothing(FontSmoothingType.FontSmoothingTypeWindows);
            }
        }

        public  Encoding InitialEncoding
        {
            get { return _initialEncoding; }
            set { _initialEncoding = value; }
        }

        public static string DownloadsFolder 
        {
            get 
            { return _DownloadsFolder;} 
            set 
            { _DownloadsFolder = value;}
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Specifies whether TextBoxResizing is enabled.")]
        public  bool AllowTextBoxResizing
        {
            get
            {
                return Convert.ToBoolean(preferences.textAreasAreResizable());
            }
            set
            {
                preferences.setTextAreasAreResizable(Convert.ToInt32(value));
            }
        }
        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Specifies whether Cache is enabled.")]
        public  bool UseCache
        {
            get
            {
                return Convert.ToBoolean(preferences.usesPageCache());
            }
            set
            {
                preferences.setUsesPageCache(Convert.ToInt32(value));
            }
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("(Spell Checking is not enabled) Specifies whether the document should always be checked for spelling.")]
        public  bool AlwaysCheckSpelling
        {
            get
            {
                return Convert.ToBoolean(preferences.continuousSpellCheckingEnabled());                
            }
            set
            {
                preferences.setContinuousSpellCheckingEnabled(Convert.ToInt32(value));           
            }
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Specifies whether Plugins are enabled.")]
        public  bool AllowPlugins
        {
            get
            {
                 return Convert.ToBoolean(preferences.arePlugInsEnabled());
            }
            set
            {
                 preferences.setPlugInsEnabled(Convert.ToInt32(value));
            }
        }

        [Browsable(true), Category("Behavior"), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Description("Specifies whether Images are enabled.")]
        public  bool LoadImages
        {
            get
            {
                return Convert.ToBoolean(preferences.loadsImagesAutomatically());
            }
            set
            {
                preferences.setLoadsImagesAutomatically(Convert.ToInt32(value));
            }
        }

        public static bool WillHandleDownloadsManually { get { return _WillHandleDownloadsManually; } set { _WillHandleDownloadsManually = value; } }
        public static  string ApplicationName { get { return _ApplicationName; } set { _ApplicationName = value; } }
        public  WebKitCookieStorageAcceptPolicy CookiesAcceptPolicy { get { return _CookiesPolicy; } set { _CookiesPolicy = value; } }
        public  bool WillGetFavicon { get { return _WillGetFavicon; } set { _WillGetFavicon = value; } }
        public  bool AutomaticallyCheckForPhishingBehavior { get { return _AutomaticallyCheckForPhishingBehavior; } set { AutomaticallyCheckForPhishingBehavior = value; } }
    }
}
