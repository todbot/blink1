using System;
using System.Text;

namespace WebKit.Security
{
    public static class FishPhish
    {
        // Credits for this class go to the creator of OpenGeckoSharp and Daedalus, Eric Dong

        public static bool IsSitePhishing(WebKitBrowser browser, bool paypal = true, bool TaoBao = true, bool AliPay = true, bool FB = true)
        {        
            if (paypal)
            {
                if (checkPaypal(browser) == true)
                {
                    return true;
                }
            }
            if (checkTaobao(browser) == true)
            {
                if (TaoBao)
                {
                    return true;
                }
            }
            if (AliPay)
            {
                if (checkAlipay(browser) == true)
                {
                    return true;
                }
            }
            if (checkFacebook(browser) == true)
            {
                if (FB)
                {
                    return true;
                }
            }
            return false;
        }
        public static bool checkPaypal(WebKit.WebKitBrowser br)
        {
            try
            {
                string url = br.Url.Host;
                string content = br.DocumentText;
                if (url != "www.paypal.com")
                {
                    if (content.Contains("i/logo/paypal_logo.gif") || content.Contains("webscr?cmd=_home"))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            catch
            {
            }
            return false;
        }
        public static bool checkTaobao(WebKit.WebKitBrowser br)
        {
            try
            {
                string url = br.Url.Host;
                string content = br.DocumentText;
                if (url != "login.taobao.com")
                {
                    if (content.Contains("<p><em>106575258196</em></p>") || content.Contains("<h4>您需要安装安全控件，才可使用安全登录。</h4>") || content.Contains(";if(!''.replace(/^/,String)){while(") || content.Contains("<span class=\"visitor\" id=\"J_VisitorTips_1\">"))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            catch
            {
            }
            return false;
        }
        public static bool checkAlipay(WebKit.WebKitBrowser br)
        {
            try
            {
                string url = br.Url.Host;
                string content = br.DocumentText;
                if (url != "auth.alipay.com")
                {
                    if (content.Contains("banner/loginBanner-->") || content.Contains("h2 seed=\"auth-alipayMember\">") || content.Contains("login.jhtml?style=alipay&amp;goto=") || content.Contains("<form name=\"loginForm\" id=\"login\" action=\"https://auth.alipay.com/login/index.htm\" method=\"post\" > "))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            catch
            {
            }
            return false;
        }
        public static bool checkFacebook(WebKit.WebKitBrowser br)
        {
            try
            {
                string url = br.Url.ToString();
                string content = br.DocumentText;
                if (!url.Contains("www.facebook.com"))
                {
                    if (content.Contains("banner/loginBanner-->") || content.Contains("h2 seed=\"auth-alipayMember\">") || content.Contains("login.jhtml?style=alipay&amp;goto=") || content.Contains("<form name=\"loginForm\" id=\"login\" action=\"https://auth.alipay.com/login/index.htm\" method=\"post\" > "))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            catch
            {
            }
            return false;
        }
    }
}
