using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;

namespace WebKit
{
    class WebEditingDelegate : IWebEditingDelegate
    {
        private WebKitBrowser owner;
        public WebEditingDelegate(WebKitBrowser browser)
        {
            this.owner = browser;
        }


        public void checkGrammarOfString(WebView view, string text, int length, out IEnumWebGrammarDetails grammarDetails, out int badGrammarLocation, out int badGrammarLength)
        {
            throw new NotImplementedException();
        }

        public void checkSpellingOfString(WebView view, string text, int length, out int misspellingLocation, out int misspellingLength)
        {
            throw new NotImplementedException();
        }

        public void closeSpellDocument(WebView view)
        {
            throw new NotImplementedException();
        }

        public int doPlatformCommand(WebView WebView, string command)
        {
            throw new NotImplementedException();
        }

        public IEnumSpellingGuesses guessesForWord(string word)
        {
            throw new NotImplementedException();
        }

        public void ignoreWordInSpellDocument(WebView view, string word)
        {
            throw new NotImplementedException();
        }

        public void learnWord(string word)
        {
            throw new NotImplementedException();
        }

        public void preflightChosenSpellServer()
        {
            throw new NotImplementedException();
        }

        public int sharedSpellCheckerExists()
        {
            throw new NotImplementedException();
        }

        public int shouldApplyStyle(WebView WebView, IDOMCSSStyleDeclaration style, IDOMRange range)
        {
            throw new NotImplementedException();
        }

        public int shouldBeginEditingInDOMRange(WebView WebView, IDOMRange range)
        {
            throw new NotImplementedException();
        }

        public int shouldChangeSelectedDOMRange(WebView WebView, IDOMRange currentRange, IDOMRange proposedRange, _WebSelectionAffinity selectionAffinity, int stillSelecting)
        {
            throw new NotImplementedException();
        }

        public int shouldChangeTypingStyle(WebView WebView, IDOMCSSStyleDeclaration currentStyle, IDOMCSSStyleDeclaration proposedStyle)
        {
            throw new NotImplementedException();
        }

        public int shouldDeleteDOMRange(WebView WebView, IDOMRange range)
        {
            throw new NotImplementedException();
        }

        public int shouldEndEditingInDOMRange(WebView WebView, IDOMRange range)
        {
            throw new NotImplementedException();
        }

        public void shouldInsertNode(WebView WebView, IDOMNode node, IDOMRange range, _WebViewInsertAction action)
        {
            throw new NotImplementedException();
        }

        public int shouldInsertText(WebView WebView, string text, IDOMRange range, _WebViewInsertAction action)
        {
            throw new NotImplementedException();
        }

        public void showSpellingUI(int show)
        {
            throw new NotImplementedException();
        }

        public int spellingUIIsShowing()
        {
            throw new NotImplementedException();
        }

        public IWebUndoManager undoManagerForWebView(WebView WebView)
        {
            throw new NotImplementedException();
        }

        public void updateGrammar()
        {
            throw new NotImplementedException();
        }

        public void updateSpellingUIWithGrammarString(string @string, int location, int length, string userDescription, ref string guesses, int guessesCount)
        {
            throw new NotImplementedException();
        }

        public void updateSpellingUIWithMisspelledWord(string word)
        {
            throw new NotImplementedException();
        }

        public void webViewDidBeginEditing(IWebNotification notification)
        {
            throw new NotImplementedException();
        }

        public void webViewDidChange(IWebNotification notification)
        {
            throw new NotImplementedException();
        }

        public void webViewDidChangeSelection(IWebNotification notification)
        {
            throw new NotImplementedException();
        }

        public void webViewDidChangeTypingStyle(IWebNotification notification)
        {
            throw new NotImplementedException();
        }

        public void webViewDidEndEditing(IWebNotification notification)
        {
            throw new NotImplementedException();
        }
    }
}
