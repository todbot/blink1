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

// Handles events relating to UI changes.  More info at 
// http://developer.apple.com/documentation/Cocoa/Reference/WebKit/Protocols/WebUIDelegate_Protocol

// TODO: most of these events aren't used at all (yet). Find out what they
// do and whether they are actually working in WebKit.
using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;
using System.Diagnostics;
using System.Drawing.Printing;
using System.Drawing;
using System.Drawing.Drawing2D;
using WebKit.DOM;

namespace WebKit
{
    internal delegate void CloseWindowRequest(WebView view);
    internal delegate void CreateWebViewWithRequestEvent(IWebURLRequest request, out WebView webView, bool popup = false);
    internal delegate void StatusTextChangedEvent(string statustext);  
    internal delegate void JavaScriptMessagePanel(string message, string defaulttext);
    internal delegate void RunJavaScriptAlertPanelWithMessageEvent(WebView sender, string message);
    internal delegate int RunJavaScriptConfirmPanelWithMessageEvent(WebView sender, string message);
    internal delegate int RunJavaScriptPromptBeforeUnload(WebView sender, string message);
    internal delegate void MouseDidMoveOverElement(WebView sender, IDOMNode element);
    internal delegate string RunJavaScriptTextInputPanelWithPromptEvent(WebView sender, string message, string defaultText);
    internal delegate bool AllowGeolocationRequest(WebView sender, webFrame frame, IWebSecurityOrigin orig);
    internal delegate void AddMessage(WebView sender, string mes, string u, int line, int error);
    internal class WebUIDelegate : IWebUIDelegate, IWebUIDelegatePrivate, IWebUIDelegatePrivate2, IWebUIDelegatePrivate3, IWebUIDelegatePrivate4 
    {
        public event CreateWebViewWithRequestEvent CreateWebViewWithRequest;
        public event StatusTextChangedEvent StatusTextChanged = delegate { };
        public event CloseWindowRequest CloseWindowRequest = delegate { };
        public event RunJavaScriptPromptBeforeUnload RunJavaScriptPromptBeforeUnload;
        public event RunJavaScriptAlertPanelWithMessageEvent RunJavaScriptAlertPanelWithMessage;
        public event RunJavaScriptConfirmPanelWithMessageEvent RunJavaScriptConfirmPanelWithMessage;
        public event RunJavaScriptTextInputPanelWithPromptEvent RunJavaScriptTextInputPanelWithPrompt;
        public event MouseDidMoveOverElement MouseDidMoveOverElement = delegate { };
        public event AllowGeolocationRequest GeolocationReq;
        public event AddMessage AddMessageToConsole = delegate { };
        private WebKitBrowser owner;

        public WebUIDelegate(WebKitBrowser browser)
        {
            this.owner = browser;
        }

        #region IWebUIDelegate Members

        public void addCustomMenuDrawingData(WebView sender, int hMenu)
        {
        }

        public int canRedo()
        {
            return 1;
        }

        public int canRunModal(WebView WebView)
        {
            return 1;
        }

        public void canTakeFocus(WebView sender, int forward, out int result)
        {
            throw new NotImplementedException();
        }

        public int canUndo()
        {
            return 1;
        }

        public void cleanUpCustomMenuDrawingData(WebView sender, int hMenu)
        {
        }

        public void contextMenuItemSelected(WebView sender, IntPtr item, CFDictionaryPropertyBag element)
        {
        }

        public int contextMenuItemsForElement(WebView sender, CFDictionaryPropertyBag element, int defaultItemsHMenu)
        {
            owner.CustomContextMenuManager.FireShowContextMenu();
            if (owner.UseDefaultContextMenu)
            {
                if (owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.Body || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.Button || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.Form || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.TextSelected || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.ImageAndLink || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.Image || owner.CustomContextMenuManager.AppropriateContextMenuType == ContextMenuType.Link)
                    return 0;
                else
                    return owner.UseDefaultContextMenu ? defaultItemsHMenu : 0; ;
            }
            else
            { return owner.UseDefaultContextMenu ? defaultItemsHMenu : 0; }
        }

        public WebView createModalDialog(WebView sender, IWebURLRequest request)
        {
            if (owner.AllowNewWindows)
            {
                WebView view;
                CreateWebViewWithRequest(request, out view);
                return view;
            }
            else
            {
                return null;
            }
        }

        public WebView createWebViewWithRequest(WebView sender, IWebURLRequest request)
        {
            if (owner.AllowNewWindows)
            {
                WebView view;
                CreateWebViewWithRequest(request, out view);
                return view;
            }
            else
            {
                return null;
            }
        }

        public WebDragDestinationAction dragDestinationActionMaskForDraggingInfo(WebView WebView, IDataObject draggingInfo)
        {
            //if (owner.GetCurrentElement().Type == ElementType.Input)
            //    return WebDragDestinationAction.WebDragDestinationActionEdit;
            //else
            return WebDragDestinationAction.WebDragDestinationActionAny;
        }

        public WebDragSourceAction dragSourceActionMaskForPoint(WebView WebView, ref tagPOINT point)
        {
            Node e = owner.ElementAtPoint(new Point(point.x, point.y));
            if (e.Type == ElementType.Image)
                return WebDragSourceAction.WebDragSourceActionImage;
            else if (e.Type == ElementType.LinkOrUknown)
                return WebDragSourceAction.WebDragSourceActionLink;
            else if (!string.IsNullOrEmpty(owner.SelectedText))
                return WebDragSourceAction.WebDragSourceActionSelection;
            else
                return WebDragSourceAction.WebDragSourceActionAny;
        }

        public void drawCustomMenuItem(WebView sender, IntPtr drawItem)
        {
        }

        public void drawFooterInRect(WebView WebView, ref tagRECT rect, int drawingContext, uint pageIndex, uint pageCount)
        {
        }

        public void drawHeaderInRect(WebView WebView, ref tagRECT rect, int drawingContext)
        {
        }

        public string ftpDirectoryTemplatePath(WebView WebView)
        {
            return string.Empty;
        }

        public int hasCustomMenuImplementation()
        {
            return Convert.ToInt32(!owner.UseDefaultContextMenu);
        }

        public int isMenuBarVisible(WebView WebView)
        {
            throw new NotImplementedException();
        }

        public void makeFirstResponder(WebView sender, int responderHWnd)
        {
        }

        public void measureCustomMenuItem(WebView sender, IntPtr measureItem)
        {
        }

        public void mouseDidMoveOverElement(WebView sender, CFDictionaryPropertyBag elementInformation, uint modifierFlags)
        {
            object textcontent;
            elementInformation.RemoteRead("WebElementLinkURLKey", out textcontent, null, 0, null);
            if (!string.IsNullOrEmpty((string)textcontent))
            {
                StatusTextChanged((string)textcontent);
                owner.StatusText = (string)textcontent;
                if ((string)textcontent != string.Empty)
                    owner.LastSelectedLink = (string)textcontent;
            }
            else
            {
                StatusTextChanged(string.Empty);
                owner.StatusText = string.Empty;
            }
            object el;
            elementInformation.RemoteRead("WebElementDOMNodeKey", out el, null, 0, null);
                if (el is IDOMElement || el is IDOMHTMLElement || el is IDOMHTMLTextAreaElement || el is IDOMHTMLInputElement)
                {
                    owner._el = (Element)Node.Create(el as IDOMNode);
                }
            MouseDidMoveOverElement(sender, el as IDOMNode);
        }

        public void paintCustomScrollCorner(WebView WebView, ref _RemotableHandle hDC, tagRECT rect)
        {
        }

        public void paintCustomScrollbar(WebView WebView, ref _RemotableHandle hDC, tagRECT rect, WebScrollBarControlSize size, uint state, WebScrollbarControlPart pressedPart, int vertical, float value, float proportion, uint parts)
        {
        }

        public void printFrame(WebView WebView, webFrame frame)
        {
            owner.ShowPrintDialog();
        }

        public void redo()
        {
            owner.Redo();
        }

        public void registerUndoWithTarget(IWebUndoTarget target, string actionName, object actionArg)
        {
            //owner.WebView.undoManager().registerUndoWithTarget(target, 0, actionArg);
        }

        public void removeAllActionsWithTarget(IWebUndoTarget target)
        {
            //owner.WebView.undoManager().removeAllActionsWithTarget(target);
        }

        public int runBeforeUnloadConfirmPanelWithMessage(WebView sender, string message, webFrame initiatedByFrame)
        {
            return RunJavaScriptPromptBeforeUnload(sender, message);
        }

        public int runDatabaseSizeLimitPrompt(WebView WebView, string displayName, webFrame initiatedByFrame)
        {
            throw new NotImplementedException();
        }

        public void runJavaScriptAlertPanelWithMessage(WebView sender, string message)
        {
            RunJavaScriptAlertPanelWithMessage(sender, message);
        }

        public int runJavaScriptConfirmPanelWithMessage(WebView sender, string message)
        {
            return RunJavaScriptConfirmPanelWithMessage(sender, message);
        }

        public string runJavaScriptTextInputPanelWithPrompt(WebView sender, string message, string defaultText)
        {
            return RunJavaScriptTextInputPanelWithPrompt(sender, message, defaultText);

        }

        public void runModal(WebView WebView)
        {
        }

        public void runOpenPanelForFileButtonWithResultListener(WebView sender, IWebOpenPanelResultListener resultListener)
        {

        }

        public void setActionTitle(string actionTitle)
        {
        }

        public void setContentRect(WebView sender, ref tagRECT contentRect)
        {
        }

        public void setFrame(WebView sender, ref tagRECT frame)
        {
        }

        public void setMenuBarVisible(WebView WebView, int visible)
        {
        }

        public void setResizable(WebView sender, int resizable)
        {
        }

        public void setStatusBarVisible(WebView sender, int visible)
        {
        }

        public void setStatusText(WebView sender, string text)
        {
            
        }

        public void setToolbarsVisible(WebView sender, int visible)
        {
        }

        public void shouldPerformAction(WebView WebView, uint itemCommandID, uint sender)
        {
        }

        public void takeFocus(WebView sender, int forward)
        {
        }

        public void trackCustomPopupMenu(WebView sender, int hMenu, ref tagPOINT point)
        {

        }

        public void undo()
        {
            owner.Undo();
        }

        public int validateUserInterfaceItem(WebView WebView, uint itemCommandID, int defaultValidation)
        {
            throw new NotImplementedException();
        }

        public int webViewAreToolbarsVisible(WebView sender)
        {
            throw new NotImplementedException();
        }

        public void webViewClose(WebView sender)
        {
            CloseWindowRequest(sender);
        }

        public tagRECT webViewContentRect(WebView sender)
        {
            return ((WebViewClass)sender).visibleContentRect();
        }

        public int webViewFirstResponder(WebView sender)
        {
            return 0;
        }

        public void webViewFocus(WebView sender)
        {
            (sender as WebKit.Interop.WebViewClass).updateFocusedAndActiveState();
            owner.Focus();
            owner.Select();
        }

        public float webViewFooterHeight(WebView WebView)
        {
            return 0;
        }

        public tagRECT webViewFrame(WebView sender)
        {
            return ((WebViewClass)sender).visibleContentRect();
        }

        public float webViewHeaderHeight(WebView WebView)
        {
            return 0;
        }

        public int webViewIsResizable(WebView sender)
        {
            throw new NotImplementedException();
        }

        public int webViewIsStatusBarVisible(WebView sender)
        {
            return 0;
        }

        public tagRECT webViewPrintingMarginRect(WebView WebView)
        {
            PageSettings settings = owner.PageSettings;

            // WebKit specifies margins in 1000ths of an inch.
            // PrinterResolution.Y returns 0 for some reason,
            // on Adobe distiller anyway, so we'll use X for the moment.
            int dpi = settings.PrinterResolution.X;
            int marginLeft = settings.Margins.Left * 10;
            int marginRight = settings.Margins.Right * 10;
            int marginTop = settings.Margins.Top * 10;
            int marginBottom = settings.Margins.Bottom * 10;

            tagRECT rect = new tagRECT();
            rect.left = marginLeft;
            rect.top = marginTop;
            rect.right = marginRight;
            rect.bottom = marginBottom;
            return rect;
        }

        public void webViewShow(WebView sender)
        {
            (sender as WebViewClass).updateFocusedAndActiveState();
        }

        public string webViewStatusText(WebView sender)
        {
            return owner.StatusText;
        }

        public void webViewUnfocus(WebView sender)
        {
            owner.WebView.updateFocusedAndActiveState();
        }

        public void willPerformDragDestinationAction(WebView WebView, WebDragDestinationAction action, IDataObject draggingInfo)
        {

        }

        public IDataObject willPerformDragSourceAction(WebView WebView, WebDragSourceAction action, ref tagPOINT point, IDataObject pasteboard)
        {
            return null;
        }

        #endregion

        #region IWebUIDelegatePrivate Members
        public uint doDragDrop(WebView sender, IDataObject dataObject, IDropSource dropSource, uint okEffect)
        {
            return 1;
        }

        public IWebEmbeddedView embeddedViewWithArguments(WebView sender, webFrame frame, CFDictionaryPropertyBag arguments)
        {
            throw new NotImplementedException();
        }

        public void exceededDatabaseQuota(WebView sender, webFrame frame, WebSecurityOrigin origin, string databaseIdentifier)
        {
            
        }

        public void unused1()
        {
            
        }

        public void unused2()
        {
            
        }

        public void unused3()
        {
            
        }

        public void webViewAddMessageToConsole(WebView sender, string message, int lineNumber, string url, int isError)
        {
            AddMessageToConsole(sender, message, url, lineNumber, isError);
        }

        public void webViewClosing(WebView sender)
        {
            
        }

        public void webViewDidInvalidate(WebView sender)
        {
            owner.Invalidate();
        }

        public int webViewGetDlgCode(WebView sender, uint keyCode)
        {
            throw new NotImplementedException();
        }

        public void webViewLostFocus(WebView sender, int loseFocusToHWnd)
        {
            
        }

        public void webViewPainted(WebView sender)
        {
            
        }

        public void webViewReceivedFocus(WebView sender)
        {
        }

        public void webViewScrolled(WebView sender)
        {
            
        }
        public void webViewSetCursor(WebView sender, int cursor)
        {
            owner.Cursor = new System.Windows.Forms.Cursor((IntPtr)cursor);
            //NativeMethods.SendMessage(owner.webViewHWND, (uint)0x0020, (IntPtr)cursor, (IntPtr)cursor);
        }

        public int webViewShouldInterruptJavaScript(WebView sender)
        {
            return 0;
        }

        public WebView createWebViewWithRequest(WebView sender, IWebURLRequest request, CFDictionaryPropertyBag windowFeatures)
        {
            if (owner.AllowNewWindows)
            {
                WebView view;
                CreateWebViewWithRequest(request, out view, true);
                return view;
            }
            else
            {
                return null;
            }
        }

        public void decidePolicyForGeolocationRequest(WebView sender, webFrame frame, WebSecurityOrigin origin, IWebGeolocationPolicyListener listener)
        {
            if (GeolocationReq(sender, frame, origin) == true)
            {
                if (listener != null)
                {
                    listener.allow();
                }
            }
            else
                listener.deny();
        }

        public void drawBackground(WebView sender, int hDC, ref tagRECT dirtyRect)
        {
            
        }


        public void didPressMissingPluginButton(IDOMElement __MIDL__IWebUIDelegatePrivate30000)
        {
            owner.OnMissingPlugin(__MIDL__IWebUIDelegatePrivate30000);
        }


        public void enterFullScreenForElement(IDOMElement __MIDL__IWebUIDelegatePrivate40001)
        {
            
        }

        public void exitFullScreenForElement(IDOMElement __MIDL__IWebUIDelegatePrivate40002)
        {
            
        }

        public int supportsFullScreenForElement(IDOMElement __MIDL__IWebUIDelegatePrivate40000, int withKeyboard)
        {
            return 1;
        }
        #endregion
    }
}
