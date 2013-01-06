using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using WebKit.Interop;
using WebKit.DOM;
using System.Runtime.InteropServices;

// Contribution by: Philippe Asmar (Kryptonite Co.)

namespace WebKit
{
    public enum ContextMenuType
    {
        Image = 0,
        Link = 1,
        ImageAndLink = 2,
        JavaScript = 3,
        Body = 4,
        Input = 5,
        Form = 6,
        Button = 7,
        TextSelected = 8
    }
    public class ContextMenuManager
    {
        public WebKitBrowser Owner;
        public BodyContextMenu BodyMenu;
        public TextContextMenu SelectedTextMenu;
        public LinkAndImageContextMenu LinkOrImageContextMenu;
        public LinkContextMenu LinkContextMenu;
        public ImageContextMenu ImageContextMenu;
        public event ShowContextMenu ShowContextMenu = delegate { };
        public ContextMenuManager(WebKitBrowser browser)
        {
            this.Owner = browser;
            this.BodyMenu = new BodyContextMenu(browser);
            this.SelectedTextMenu = new TextContextMenu(browser);
            this.LinkContextMenu = new LinkContextMenu(browser);
            this.LinkOrImageContextMenu = new LinkAndImageContextMenu(browser);
            this.ImageContextMenu = new ImageContextMenu(browser);
        }

        internal void FireShowContextMenu()
        {
            Point screenOrigin = Owner.PointToScreen(new Point(0, 0));
            Point currentpoint = new System.Drawing.Point(Cursor.Position.X - screenOrigin.X, Cursor.Position.Y - screenOrigin.Y);

            ShowContextMenu(this.Owner, new ShowContextMenuEventArgs(currentpoint, AppropriateContextMenuType));

            if (Owner.UseDefaultContextMenu && Owner.UseCustomContextMenu == false)
            {
                if (this.AppropriateContextMenuType == ContextMenuType.Body || AppropriateContextMenuType == ContextMenuType.Button || AppropriateContextMenuType == ContextMenuType.Form)
                {
                    BodyMenu.Show(Owner, currentpoint);
                }
                if (this.AppropriateContextMenuType == ContextMenuType.TextSelected)
                    SelectedTextMenu.Show(Owner, currentpoint);
                if (this.AppropriateContextMenuType == ContextMenuType.ImageAndLink)
                {
                    LinkOrImageContextMenu.Show(Owner, currentpoint);
                    return;
                }
                if (this.AppropriateContextMenuType == ContextMenuType.Image)
                    ImageContextMenu.Show(Owner, currentpoint);
                if (this.AppropriateContextMenuType == ContextMenuType.Link)
                    LinkContextMenu.Show(Owner, currentpoint);
            }
        }


        public ContextMenuType AppropriateContextMenuType
        {
            get
            {
                if (Owner.CanCopyText)
                {
                    if (Owner.GetCurrentElement().Type == ElementType.Input)
                    {
                        return ContextMenuType.Input;
                    }
                    else
                        return ContextMenuType.TextSelected;
                }
                else
                {
                    if (Owner.GetCurrentElement().Type == DOM.ElementType.Body)
                        return ContextMenuType.Body;
                    else if (Owner.GetCurrentElement().Type == DOM.ElementType.Input)
                    {
                        if (Owner.GetCurrentElement().GetAttribute("type").ToString() == "submit")
                        {
                            return ContextMenuType.Button;
                        }
                        else
                        {
                            return ContextMenuType.Input;
                        }

                    }
                    else if (Owner.GetCurrentElement().Type == DOM.ElementType.Image)
                    {
                        if (Owner.StatusText == string.Empty)
                        {
                            return ContextMenuType.Image;
                        }
                        else
                        {
                            if (Uri.IsWellFormedUriString(Owner.LastSelectedLink, UriKind.RelativeOrAbsolute))
                            {
                                return ContextMenuType.ImageAndLink;
                            }
                            else
                            {
                                return ContextMenuType.Image;
                            }
                        }
                    }
                    else if (Owner.GetCurrentElement().Type == DOM.ElementType.FORM)
                        return ContextMenuType.Form;
                    if (Owner.StatusText != string.Empty && !Owner.StatusText.StartsWith(LanguageLoader.ConnectingWith.Substring(0, LanguageLoader.ConnectingWith.IndexOf('{') - 1)) && !Owner.StatusText.StartsWith(LanguageLoader.RenderingPage))
                    {
                        if (new Uri(Owner.LastSelectedLink) != null)
                        {
                            return ContextMenuType.Link;
                        }
                        else { return ContextMenuType.Body; }
                    }
                    else
                    {
                        if (!Owner.StatusText.StartsWith(LanguageLoader.Completed) && Owner.StatusText.Contains(" "))
                        {
                            if (Owner.StatusText.StartsWith("javascript:"))
                                return ContextMenuType.JavaScript;
                            else
                                return ContextMenuType.Body;
                        }
                        else
                        {
                            return ContextMenuType.Body;
                        }
                    }
                }
            }
        }

    }
    public class BodyContextMenu : ContextMenu
    {
        WebKitBrowser owner;
        public BodyContextMenu(WebKitBrowser br)
        {
            this.owner = br;
            this.Popup += new EventHandler(BodyContextMenu_Popup);
            this.MenuItems.Add(LanguageLoader.NavBack).Click += new EventHandler(BodyContextMenuBack_Click);
            this.MenuItems.Add(LanguageLoader.NavForward).Click += new EventHandler(BodyContextMenuForward_Click);
            this.MenuItems.Add("-");
            this.MenuItems.Add(LanguageLoader.Reload).Click += new EventHandler(BodyContextMenu_Click);
            this.MenuItems.Add(LanguageLoader.Stop).Click += new EventHandler(BodyContextMenuStop_Click);
            this.MenuItems.Add("-");
            this.MenuItems.Add(LanguageLoader.ViewSource).Click += new EventHandler(BodyContextMenuStopViewSource_Click);
            this.MenuItems.Add("Inspector").Click +=new EventHandler(InspectorContextMenu);
        }

        void InspectorContextMenu(object sender, EventArgs e)
        {
            owner.ShowInspector();
        }

        void BodyContextMenuStopViewSource_Click(object sender, EventArgs e)
        {
            owner.GetSourceCode();
        }

        void BodyContextMenu_Popup(object sender, EventArgs e)
        {
            this.MenuItems[0].Text = LanguageLoader.NavBack;
            this.MenuItems[1].Text = LanguageLoader.NavForward;
            this.MenuItems[3].Text = LanguageLoader.Reload;
            this.MenuItems[4].Text = LanguageLoader.Stop;
            this.MenuItems[6].Text = LanguageLoader.ViewSource;

            this.MenuItems[0].Enabled = owner.CanGoBack;            
            this.MenuItems[1].Enabled = owner.CanGoForward;
            this.MenuItems[3].Enabled = !owner.IsBusy;
            this.MenuItems[4].Enabled = owner.IsBusy;
        }
        void BodyContextMenuForward_Click(object sender, EventArgs e)
        {
            owner.GoForward();
        }
        void BodyContextMenuBack_Click(object sender, EventArgs e)
        {
            owner.GoBack();
        }
        void BodyContextMenu_Click(object sender, EventArgs e)
        {
            owner.Reload();
        }
        void BodyContextMenuStop_Click(object sender, EventArgs e)
        {
            owner.Stop();
        }
    }
    public class TextContextMenu : ContextMenu
    {
        WebKitBrowser owner;
        public TextContextMenu(WebKitBrowser br)
        {
            this.owner = br;
            this.Popup += new EventHandler(TextContextMenu_Popup);
            this.MenuItems.Add(LanguageLoader.SearchWithGoogle).Click += new EventHandler(SearchContextMenu_Click);
            this.MenuItems.Add("-");
            this.MenuItems.Add(LanguageLoader.Copy).Click += new EventHandler(CopyContextMenu_Click);
        }

        void TextContextMenu_Popup(object sender, EventArgs e)
        {
            this.MenuItems[0].Text = LanguageLoader.SearchWithGoogle;
            this.MenuItems[2].Text = LanguageLoader.Copy;
        }
        void CopyContextMenu_Click(object sender, EventArgs e)
        {
            owner.CopySelectedText();
        }
        void SearchContextMenu_Click(object sender, EventArgs e)
        {
            owner.Navigate("http://www.google.com/search?q=" + owner.SelectedText);
        }
    }
    public class ImageContextMenu : ContextMenu
    {
        WebKitBrowser owner;
        public ImageContextMenu(WebKitBrowser br)
        {
            owner = br;
            this.MenuItems.Add("Open Image").Click += new EventHandler(ImageContextMenu1);
            this.MenuItems.Add("Open Image in New Window").Click += new EventHandler(ImageContextMenu2);
            this.MenuItems.Add("Download Image").Click += new EventHandler(ImageContextMenu3);
            this.MenuItems.Add("-");
            this.MenuItems.Add("Inspect").Click += new EventHandler(Inspect);
        }

        void Inspect(object sender, EventArgs e)
        {
            owner.ShowInspector();
            owner.WebView.inspector().attach();
        }
        void ImageContextMenu1(object sender, EventArgs e)
        {
            owner.Navigate(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")));
        }
        void ImageContextMenu3(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebDownload d = new WebDownloadClass();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")), _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                d.initWithRequest(request, owner.downloadDelegate);
                owner.downloadDelegate.decideDestinationWithSuggestedFilename(d, URLToFileName(request.url(), true));
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        string FormatImageLink(string iurl)
        {
            if (Uri.IsWellFormedUriString(iurl, UriKind.Absolute))
                return iurl;
            else
                return "http://" + owner.Url.Host + iurl;
        }
        void ImageContextMenu2(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebURLRequest request = new WebURLRequestClass();

                request.initWithURL(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")), _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                owner.uiDelegate.createWebViewWithRequest((WebView)owner.GetWebViewAsObject(), request);
                WebKitBrowser.activationContext.Deactivate();
            });
        }

        string URLToFileName(string url, bool image = false)
        {
            if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                url = "http://" + owner.Url.Host + url;
            Uri u = new Uri(url);
            string fn = u.Segments[u.Segments.Length - 1];
            if (!fn.EndsWith("/"))
            {
                return u.Segments[u.Segments.Length - 1];
            }
            else
                if (image)
                    return "Unrecognised Download.jpg";
                else
                    return "Unrecognised Download.file";
        }

        private delegate TResult Fn<TResult>();
        private delegate void Fn();

        private TResult _ownerInvoke<TResult>(Fn<TResult> method)
        {
            if (owner.InvokeRequired)
                return (TResult)owner.Invoke(method);
            else
                return method();
        }

        private void _ownerInvoke(Fn method)
        {
            if (owner.InvokeRequired)
                owner.Invoke(method);
            else
                method();
        }
    }
    public class LinkContextMenu : ContextMenu
    {
        WebKitBrowser owner;
        public LinkContextMenu(WebKitBrowser br)
        {
            this.owner = br;
            this.MenuItems.Add("Open Link").Click += new EventHandler(LinkContextMenu_Click);
            this.MenuItems.Add("Open Link in New Window").Click += new EventHandler(LinkContextMenu2_Click);
            this.MenuItems.Add("Download Linked File").Click += new EventHandler(LinkContextMenu3_Click);
            this.MenuItems.Add("Copy Link").Click += new EventHandler(CopyContextMenu_Click);
            this.MenuItems.Add("-");
            this.MenuItems.Add("Inspect").Click += new EventHandler(Inspect);
        }

        void Inspect(object sender, EventArgs e)
        {
            owner.ShowInspector();
        }
        
        
        void CopyContextMenu_Click(object sender, EventArgs e)
        {
            owner.WebView.copyURL(owner.LastSelectedLink);
        }
        string URLToFileName(string url, bool image = false)
        {
            if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                url = "http://" + owner.Url.Host + url;
            Uri u = new Uri(url);
            string fn = u.Segments[u.Segments.Length - 1];
            if (!fn.EndsWith("/"))
            {
                return u.Segments[u.Segments.Length - 1];
            }
            else
                if (image)
                    return "Unrecognised Download.jpg";
                else
                    return "Unrecognised Download.file";
        }
        void LinkContextMenu3_Click(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebDownload d = new WebDownloadClass();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(owner.LastSelectedLink, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                d.initWithRequest(request, owner.downloadDelegate);
                owner.downloadDelegate.decideDestinationWithSuggestedFilename(d, URLToFileName(request.url()));
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        void LinkContextMenu2_Click(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(owner.LastSelectedLink, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                owner.uiDelegate.createWebViewWithRequest((WebView)owner.GetWebViewAsObject(), request);
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        void LinkContextMenu_Click(object sender, EventArgs e)
        {
            owner.Navigate(owner.LastSelectedLink);
        }
        private delegate TResult Fn<TResult>();
        private delegate void Fn();

        private TResult _ownerInvoke<TResult>(Fn<TResult> method)
        {
            if (owner.InvokeRequired)
                return (TResult)owner.Invoke(method);
            else
                return method();
        }

        private void _ownerInvoke(Fn method)
        {
            if (owner.InvokeRequired)
                owner.Invoke(method);
            else
                method();
        }
    }
    public class LinkAndImageContextMenu : ContextMenu
    {
        WebKitBrowser owner;
        public LinkAndImageContextMenu(WebKitBrowser br)
        {
            this.owner = br;
            this.MenuItems.Add("Open Link").Click += new EventHandler(LinkContextMenu_Click); 
            this.MenuItems.Add("Open Link in New Window").Click += new EventHandler(LinkContextMenu2_Click);
            this.MenuItems.Add("Download Linked File").Click += new EventHandler(LinkContextMenu3_Click);
            this.MenuItems.Add("Copy Link").Click += new EventHandler(CopyContextMenu_Click);
            this.MenuItems.Add("-");
            this.MenuItems.Add("Open Image").Click += new EventHandler(ImageContextMenu);
            this.MenuItems.Add("Open Image in New Window").Click += new EventHandler(ImageContextMenu2); 
            this.MenuItems.Add("Download Image").Click += new EventHandler(ImageContextMenu3);
            this.MenuItems.Add("-");
            this.MenuItems.Add("Inspect").Click +=new EventHandler(Inspect); 
        }

        void Inspect(object sender, EventArgs e)
        {
            owner.ShowInspector();
        }
        void ImageContextMenu(object sender, EventArgs e)
        {
            owner.Navigate(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")));
        }
        void ImageContextMenu3(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebDownload d = new WebDownloadClass();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")), _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                d.initWithRequest(request, owner.downloadDelegate);
                owner.downloadDelegate.decideDestinationWithSuggestedFilename(d, URLToFileName(request.url(), true));
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        string FormatImageLink(string iurl)
        {
            if (Uri.IsWellFormedUriString(iurl, UriKind.Absolute))
                return iurl;
            else
                return "http://" + owner.Url.Host + iurl;
        }
        void ImageContextMenu2(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebURLRequest request = new WebURLRequestClass();

                request.initWithURL(FormatImageLink(owner.GetCurrentElement().GetAttribute("src")), _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                owner.uiDelegate.createWebViewWithRequest((WebView)owner.GetWebViewAsObject(), request);
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        void CopyContextMenu_Click(object sender, EventArgs e)
        {
            owner.WebView.copyURL(owner.LastSelectedLink);
        }
        string URLToFileName(string url, bool image = false)
        {
            if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                url = "http://" + owner.Url.Host + url;
            Uri u = new Uri(url);
            string fn = u.Segments[u.Segments.Length - 1];
            if (!fn.EndsWith("/"))
            {
                return u.Segments[u.Segments.Length - 1];
            }
            else
                if (image)
                    return "Unrecognised Download.jpg";
                else
                    return "Unrecognised Download.file";
        }
        void LinkContextMenu3_Click(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebDownload d = new WebDownloadClass();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(owner.LastSelectedLink, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                d.initWithRequest(request, owner.downloadDelegate);
                owner.downloadDelegate.decideDestinationWithSuggestedFilename(d, URLToFileName(request.url()));
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        void LinkContextMenu2_Click(object sender, EventArgs e)
        {
            _ownerInvoke(() =>
            {
                WebKitBrowser.activationContext.Activate();
                WebURLRequest request = new WebURLRequestClass();
                request.initWithURL(owner.LastSelectedLink, _WebURLRequestCachePolicy.WebURLRequestUseProtocolCachePolicy, 60);
                owner.uiDelegate.createWebViewWithRequest((WebView)owner.GetWebViewAsObject(), request);
                WebKitBrowser.activationContext.Deactivate();
            });
        }
        void LinkContextMenu_Click(object sender, EventArgs e)
        {
            owner.Navigate(owner.LastSelectedLink);
        }
        private delegate TResult Fn<TResult>();
        private delegate void Fn();

        private TResult _ownerInvoke<TResult>(Fn<TResult> method)
        {
            if (owner.InvokeRequired)
                return (TResult)owner.Invoke(method);
            else
                return method();
        }

        private void _ownerInvoke(Fn method)
        {
            if (owner.InvokeRequired)
                owner.Invoke(method);
            else
                method();
        }
    }

}
