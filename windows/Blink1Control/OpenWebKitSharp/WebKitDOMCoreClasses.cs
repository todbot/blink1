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

using System;
using System.Collections.Generic;
using System.Text;
using WebKit.Interop;

namespace WebKit.DOM
{
    public enum NodeType
    {
        Element = 1,
        Attribute = 2,
        Text = 3,
        CDATASection = 4,
        EntityReference = 5,
        Entity = 6,
        ProcessingInstruction = 7,
        Comment = 8,
        Document = 9,
        DocumentType = 10,
        DocumentFragment = 11,
        Notation = 12,
        XPathNamespace = 13
    }

    /// <summary>
    /// Represents an unordered list of DOM Nodes where elements are accessed by name.
    /// </summary>
    public class NamedNodeMap : IEnumerable<Node>
    {
        private IDOMNamedNodeMap namedNodeMap;

        /// <summary>
        /// NamedNodeMap constructor.
        /// </summary>
        /// <param name="NamedNodeMap">WebKit IDOMNamedNodeMap object.</param>
        protected NamedNodeMap(IDOMNamedNodeMap NamedNodeMap)
        {
            this.namedNodeMap = NamedNodeMap;
        }

        internal static NamedNodeMap Create(IDOMNamedNodeMap NamedNodeMap)
        {
            return new NamedNodeMap(NamedNodeMap);
        }

        #region NamedNodeMap Object Properties

        /// <summary>
        /// Gets the number of nodes in the collection.
        /// </summary>
        public int Length
        {
            get
            {
                return (int)namedNodeMap.length();
            }
        }

        #endregion

        #region Indexers

        /// <summary>
        /// Gets the item at the specified index.
        /// </summary>
        /// <param name="index">Index of the item to get.</param>
        /// <returns>Node at the specified index.</returns>
        public Node this[int index]
        {
            get
            {
                if (index < 0 || index >= (int)namedNodeMap.length())
                    return null;
                return Node.Create(namedNodeMap.item((uint)index));
            }
        }

        /// <summary>
        /// Gets the item with the specified name.
        /// </summary>
        /// <param name="name">Name of the item to get.</param>
        /// <returns>Node with the specified name.</returns>
        public Node this[string name]
        {
            get
            {
                return Node.Create(namedNodeMap.getNamedItem(name));
            }
        }

        /// <summary>
        /// Gets the item with the specified namespace and name.
        /// </summary>
        /// <param name="NSURI">Namespace of the item to get.</param>
        /// <param name="localName">Name of the item to get.</param>
        /// <returns>Node with the specified namespace and name.</returns>
        public Node this[string NSURI, string localName]
        {
            get
            {
                return Node.Create(namedNodeMap.getNamedItemNS(NSURI, localName));
            }
        }

        #endregion

        #region NamedNodeMap Object Methods

        /// <summary>
        /// Gets the item with the specified name.
        /// </summary>
        /// <param name="Name">Name of the item to get.</param>
        /// <returns>Node with the specified name.</returns>
        public Node GetNamedItem(string Name)
        {
            return this[Name];
        }

        /// <summary>
        /// Gets the item with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the item to get.</param>
        /// <param name="LocalName">Name of the item to get.</param>
        /// <returns>Node with the specified namespace and name.</returns>
        public Node GetNamedItemNS(string NamespaceURI, string LocalName)
        {
            return this[NamespaceURI, LocalName];
        }

        /// <summary>
        /// Gets the item at the specified index.
        /// </summary>
        /// <param name="Index">Index of the item to get.</param>
        /// <returns>Node at the specified index.</returns>
        public Node GetItem(int Index)
        {
            return this[Index];
        }

        /// <summary>
        /// Removes the item with the specified name from the collection.
        /// </summary>
        /// <param name="Name">Name of the item to remove.</param>
        /// <returns>The removed node.</returns>
        public Node RemoveNamedItem(string Name)
        {
            return Node.Create(namedNodeMap.removeNamedItem(Name));
        }

        /// <summary>
        /// Removes the item with the specified namespace and name from the collection.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the item to remove.</param>
        /// <param name="LocalName">Name of the item to remove.</param>
        /// <returns>The removed node.</returns>
        public Node RemoveNamedItemNS(string NamespaceURI, string LocalName)
        {
            return Node.Create(namedNodeMap.removeNamedItemNS(NamespaceURI, LocalName));
        }

        /// <summary>
        /// Adds the specified node to the collection.
        /// </summary>
        /// <param name="Node">The node to add.</param>
        /// <returns>The added node.</returns>
        public Node SetNamedItem(Node Node)
        {
            return Node.Create(namedNodeMap.setNamedItem((IDOMNode)Node.GetWebKitObject()));
        }

        /// <summary>
        /// Adds the specified node to the collection.
        /// </summary>
        /// <param name="Node">The node to add.</param>
        /// <returns>The added node.</returns>
        public Node SetNamedItemNS(Node Node)
        {
            return Node.Create(namedNodeMap.setNamedItemNS((IDOMNode)Node.GetWebKitObject()));
        }



        #endregion

        #region IEnumerable<Node> Members

        /// <summary>
        /// Gets an enumerator for the collection.
        /// </summary>
        /// <returns>An enumerator for the collection.</returns>
        public IEnumerator<Node> GetEnumerator()
        {
            for (uint i = 0; i < namedNodeMap.length(); ++i)
                yield return Node.Create(namedNodeMap.item(i));
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            foreach (Node node in this)
                yield return node;
        }

        #endregion
    }

    ///// <summary>
    ///// Represents a DOM Window.
    ///// </summary>
    //public class DOMWindow : WebKit.DOM.OpenWebKitSharpDOMEvents 
    //{
    //    private IDOMWindow window;

    //    public object GetWebKitObject()
    //    {
    //        return window;
    //    }

    //    public DOMWindow(IDOMWindow win)
    //    {
    //        this.window = win;
    //    }

    //    public Document Document
    //    {
    //        get { return Document.Create(window.document()); }
    //    }
    //    /// <summary>
    //    /// Invokes a method in the scripting environment. 
    //    /// </summary>
    //    /// <param name="Method">The name of the method to invoke.</param>
    //    /// <param name="args">Arguments to pass to the method.</param>
    //    /// <returns>The return value of the method.</returns>
    //    public object InvokeScriptMethod(string Method, params object[] args)
    //    {
    //        object o = (object)args;
    //        return window.callWebScriptMethod(Method, ref o, args.Length);
    //    }

    //}

    public class NodeEventTarget
    {
        internal WebKit.DOMListener Listener { get; set; }
        protected NodeEventTarget(DOMNode nd)
        {
            DOMNode cl = (nd as DOMNode);
            Listener = new DOMListener(cl);
        }
    }
    /// <summary>
    /// Represents a DOM Node.
    /// </summary>
    public class Node : NodeEventTarget 
    {
        private IDOMNode node;
        /// <summary>
        /// Gets the underlying WebKit DOMNode object.
        /// </summary>
        /// <returns>WebKit DOMNode object representing this node.</returns>
        public object GetWebKitObject()
        {
            return node;
        }

        /// <summary>
        /// Node Constructor.
        /// </summary>
        /// <param name="Node">WebKit DOMNode object.</param>
        protected Node(IDOMNode Node) : base((DOMNode)Node)
        {
            this.node = Node;
            Listener.Focus += new EventHandler(Listener_Focus);
            Listener.KeyDown += new EventHandler(Listener_KeyDown);
            Listener.KeyUp += new EventHandler(Listener_KeyUp);
            Listener.MouseClick += new EventHandler(Listener_MouseClick);
            Listener.MouseOver += new EventHandler(Listener_MouseOver);
            Listener.MouseUp += new EventHandler(Listener_MouseUp);
            Listener.MouseDown += new EventHandler(Listener_MouseDown);
        }

        void Listener_MouseDown(object sender, EventArgs e)
        {
            if (MouseDown != null)
                MouseDown(this, new EventArgs());
        }

        void Listener_MouseUp(object sender, EventArgs e)
        {
            if (MouseUp != null)
                MouseUp(this, new EventArgs());
        }

        void Listener_MouseOver(object sender, EventArgs e)
        {
            if (MouseOver != null)
                MouseOver(this, new EventArgs());
        }

        void Listener_MouseClick(object sender, EventArgs e)
        {
            if (MouseClick != null)
            MouseClick(this, new EventArgs());
        }

        void Listener_KeyUp(object sender, EventArgs e)
        {
            if (KeyUp != null)
            KeyUp(this, new EventArgs());
        }


        void Listener_KeyDown(object sender, EventArgs e)
        {
            if (KeyDown != null)
            KeyDown(this, new EventArgs());
        }

        void Listener_Focus(object sender, EventArgs e)
        {
            if (Focus != null)
            Focus(this, new EventArgs());
        }

        public event EventHandler MouseClick = delegate { };

        public event EventHandler MouseOver = delegate { };

        public event EventHandler MouseUp = delegate { };

        public event EventHandler MouseDown = delegate { };

        public event EventHandler KeyUp = delegate { };

        public event EventHandler KeyDown = delegate { };

        public event EventHandler Focus = delegate { };
        public ElementType Type
        {
            get
            {
                if (node is IDOMElement)
                {
                    if ((node as IDOMElement).tagName() == "IMG")
                    {
                        return ElementType.Image;
                    }
                    else if ((node as IDOMElement).tagName() == "BODY")
                    {
                        return ElementType.Body;
                    }
                    else if ((node as IDOMElement).tagName() == "DIV")
                        return ElementType.DIV;
                    else if ((node as IDOMElement).tagName() == "FORM")
                        return ElementType.FORM;
                    else if ((node as IDOMElement).tagName() == "INPUT" || (node as IDOMElement).tagName() == "TEXTAREA")
                        return ElementType.Input;
                    else
                        return ElementType.LinkOrUknown;
                }
                else
                    return ElementType.LinkOrUknown;

            }
        }
        internal static Node Create(IDOMNode Node)
        {
            if (Node is IDOMDocument)
                return Document.Create(Node as IDOMDocument);
            else if (Node is IDOMAttr)
                return Attr.Create(Node as IDOMAttr);
            else if (Node is IDOMCharacterData)
                return CharacterData.Create(Node as IDOMCharacterData);
            else if (Node is IDOMElement)
                return Element.Create(Node as IDOMElement);
            else if (Node is IDOMDocumentType)
                return DocumentType.Create(Node as IDOMDocumentType);
            else if (Node is IDOMDocumentFragment)
                return DocumentFragment.Create(Node as IDOMDocumentFragment);
            else if (Node is IDOMEntityReference)
                return EntityReference.Create(Node as IDOMEntityReference);
            else if (Node is IDOMProcessingInstruction)
                return ProcessingInstruction.Create(Node as IDOMProcessingInstruction);
            else
                return new Node(Node);
        }

        #region Node Object Properties

        /// <summary>
        /// Gets a collection containing this node's children.
        /// </summary>
        public NodeList ChildNodes
        {
            get
            {
                return NodeList.Create(node.childNodes());
            }
        }

        /// <summary>
        /// Gets the first child of this node.
        /// </summary>
        public Node FirstChild
        {
            get
            {
                return Node.Create(node.firstChild());
            }
        }

        /// <summary>
        /// Gets the last child of this node.
        /// </summary>
        public Node LastChild
        {
            get
            {
                return Node.Create(node.lastChild());
            }
        }

        /// <summary>
        /// Gets the local part of the name of a node.
        /// </summary>
        public string LocalName
        {
            get
            {
                return node.localName();
            }
        }

        /// <summary>
        /// Gets the namespace URI of this node.
        /// </summary>
        public Uri NamespaceURI
        {
            get
            {
                Uri nsURI;
                Uri.TryCreate(node.namespaceURI(), UriKind.RelativeOrAbsolute, out nsURI);
                return nsURI;
            }
        }

        /// <summary>
        /// Gets the node immediately following this one.
        /// </summary>
        public Node NextSibling
        {
            get
            {
                return Node.Create(node.nextSibling());
            }
        }

        /// <summary>
        /// Gets the name of this node.
        /// </summary>
        public string NodeName
        {
            get
            {
                return node.nodeName();
            }
        }

        /// <summary>
        /// Gets the type of this node.
        /// </summary>
        public NodeType NodeType
        {
            get
            {
                return (NodeType)node.nodeType();
            }
        }

        /// <summary>
        /// Gets or sets the value of this node.
        /// </summary>
        public string NodeValue
        {
            get
            {
                return node.nodeValue();
            }
            set
            {
                node.setNodeValue(value);
            }
        }

        /// <summary>
        /// Gets the document that this node belongs to.
        /// </summary>
        public Document OwnerDocument
        {
            get
            {
                return Document.Create(node.ownerDocument());
            }
        }

        /// <summary>
        /// Gets the parent of this node.
        /// </summary>
        public Node ParentNode
        {
            get
            {
                return Node.Create(node.parentNode());
            }
        }

        /// <summary>
        /// Gets or sets the namespace prefix of this node.
        /// </summary>
        public string Prefix
        {
            get
            {
                return node.prefix();
            }
            set
            {
                node.setPrefix(value);
            }
        }

        /// <summary>
        /// Gets the node immediately preceding this one.
        /// </summary>
        public Node PreviousSibling
        {
            get
            {
                return Node.Create(node.previousSibling());
            }
        }

        /// <summary>
        /// Gets or sets the textual content of this node.
        /// </summary>
        public string TextContent
        {
            get
            {
                return node.textContent();
            }
            set
            {
                node.setTextContent(value);
            }
        }

        #endregion

        #region Node Object Methods

        /// <summary>
        /// Appends a new child node.
        /// </summary>
        /// <param name="NewChild">The node to append.</param>
        /// <returns>The appended node.</returns>
        public Node AppendChild(Node NewChild)
        {
            if (NewChild == null)
                throw new ArgumentNullException();
            return Node.Create(((node as IDOMNode).appendChild((IDOMNode)NewChild.node)));
        }

        /// <summary>
        /// Clones this node.
        /// </summary>
        /// <param name="Deep">Indicates whether to clone all children of the original node.</param>
        /// <returns>A clone of this node.</returns>
        public Node CloneNode(bool Deep)
        {
            return Node.Create((node as IDOMNode).cloneNode(Deep ? 1 : 0));
        }

        /// <summary>
        /// Gets a value indicating whether this node has any attributes.
        /// </summary>
        public bool HasAttributes
        {
            get
            {
                return ((IDOMNode)node).hasAttributes() != 0;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this node has any children.
        /// </summary>
        public bool HasChildNodes
        {
            get
            {
                return ((IDOMNode)node).hasChildNodes() != 0;
            }
        }

        /// <summary>
        /// Inserts a new node before an existing child node.
        /// </summary>
        /// <param name="NewChild">The node to insert.</param>
        /// <param name="RefChild">The existing child node.</param>
        /// <returns>The inserted node.</returns>
        public Node InsertBefore(Node NewChild, Node RefChild)
        {
            if (NewChild == null || RefChild == null)
                throw new ArgumentNullException();
            return Node.Create(((IDOMNode)node).insertBefore((IDOMNode)NewChild.node, (IDOMNode)RefChild.node));
        }

        /// <summary>
        /// Checks if a node is equal to this one.
        /// </summary>
        /// <param name="Node">The node to check.</param>
        /// <returns>A value indicating whether the nodes are equal.</returns>
        public bool IsEqualNode(Node Node)
        {
            return ((IDOMNode)node).isEqualNode((IDOMNode)Node.GetWebKitObject()) > 0;
        }

        /// <summary>
        /// Checks if a node is the same as this one.
        /// </summary>
        /// <param name="Node">The node to check.</param>
        /// <returns>A value indicating whether the nodes are the same.</returns>
        public bool IsSameNode(Node Node)
        {
            return ((IDOMNode)node).isSameNode((IDOMNode)Node.GetWebKitObject()) > 0;
        }

        /// <summary>
        /// Checks whether a specified feature is supported on this node.
        /// </summary>
        /// <param name="Feature">The feature to check.</param>
        /// <param name="Version">The feature version to check.</param>
        /// <returns>A value indicating whether the specified feature is supported.</returns>
        public bool IsSupported(string Feature, string Version)
        {
            return ((IDOMNode)node).isSupported(Feature, Version) > 0;
        }

        /// <summary>
        /// Removes a child node.
        /// </summary>
        /// <param name="OldChild">The child to remove.</param>
        /// <returns>The removed node.</returns>
        public Node RemoveChild(Node OldChild)
        {
            if (OldChild == null)
                throw new ArgumentNullException();
            return Node.Create(node.removeChild(OldChild.node));
        }

        /// <summary>
        /// Replaces a child node with a new node.
        /// </summary>
        /// <param name="NewChild">The new node.</param>
        /// <param name="OldChild">The node to be replaced.</param>
        /// <returns>The new node.</returns>
        public Node ReplaceChild(Node NewChild, Node OldChild)
        {
            if (NewChild == null || OldChild == null)
                throw new ArgumentNullException();
            return Node.Create(node.replaceChild(NewChild.node, OldChild.node));
        }

        #endregion
    }

    /// <summary>
    /// Represents a DOM Attribute.
    /// </summary>
    public class Attr : Node
    {
        private IDOMAttr attr;

        /// <summary>
        /// Attr constructor.
        /// </summary>
        /// <param name="Attr">WebKit IDOMAttr object.</param>
        protected Attr(IDOMAttr Attr)
            : base(Attr)
        {
            this.attr = Attr;
        }

        internal static Attr Create(IDOMAttr Attr)
        {
            return new Attr(Attr);
        }

        #region Attr Object Properties

        /// <summary>
        /// Gets whether the attribute value is set in the document.
        /// </summary>
        public bool Specified
        {
            get
            {
                return attr.specified() > 0;
            }
        }

        #endregion
    }

    /// <summary>
    /// Represents a DOM Node containing character data.
    /// </summary>
    public class CharacterData : Node
    {
        private IDOMCharacterData characterData;

        /// <summary>
        /// CharacterData constructor.
        /// </summary>
        /// <param name="CharacterData">WebKit IDOMCharacterData object.</param>
        protected CharacterData(IDOMCharacterData CharacterData)
            : base(CharacterData)
        {
            this.characterData = CharacterData;
        }

        internal static CharacterData Create(IDOMCharacterData CharacterData)
        {
            if (CharacterData is IDOMComment)
                return Comment.Create(CharacterData as IDOMComment);
            else if (CharacterData is IDOMText)
                return Text.Create(CharacterData as IDOMText);
            else
                return new CharacterData(CharacterData);
        }

        #region CharacterData Object Properties

        /// <summary>
        /// Gets or sets the data of this object.
        /// </summary>
        public string Data
        {
            get
            {
                return characterData.data();
            }
            set
            {
                characterData.setData(value);
            }
        }

        /// <summary>
        /// Gets the length of the data of this object.
        /// </summary>
        public int Length
        {
            get
            {
                return (int)characterData.length();
            }
        }

        #endregion

        #region CharacterData Object Methods

        /// <summary>
        /// Gets a substring of the specified length at the specified offset of the data.
        /// </summary>
        /// <param name="Offset">Offset of the data.</param>
        /// <param name="Count">Length of the substring.</param>
        /// <returns>The substring of data.</returns>
        public string SubstringData(int Offset, int Count)
        {
            return characterData.substringData((uint)Offset, (uint)Count);
        }

        /// <summary>
        /// Appends the specified data.
        /// </summary>
        /// <param name="Data">Data to append.</param>
        public void AppendData(string Data)
        {
            characterData.appendData(Data);
        }

        /// <summary>
        /// Deletes the substring of the specified length at the specified offset.
        /// </summary>
        /// <param name="Offset">Offset of the data.</param>
        /// <param name="Count">Length of substring to delete.</param>
        public void DeleteData(int Offset, int Count)
        {
            characterData.deleteData((uint)Offset, (uint)Count);
        }

        /// <summary>
        /// Inserts the specified data at the specified offset of the original data.
        /// </summary>
        /// <param name="Offset">Offset of the data.</param>
        /// <param name="Data">Data to insert.</param>
        public void InsertData(int Offset, string Data)
        {
            characterData.insertData((uint)Offset, Data);
        }

        /// <summary>
        /// Replaces the substring of the specified length at the specified offset with a new string.
        /// </summary>
        /// <param name="Offset">Offset of the data.</param>
        /// <param name="Count">Length of the substring to replace.</param>
        /// <param name="Data">The replacement data.</param>
        public void ReplaceData(int Offset, int Count, string Data)
        {
            characterData.replaceData((uint)Offset, (uint)Count, Data);
        }

        #endregion
    }

    /// <summary>
    /// Represents a DOM Comment.
    /// </summary>
    public class Comment : CharacterData
    {
        private IDOMComment comment;

        /// <summary>
        /// Comment constructor.
        /// </summary>
        /// <param name="Comment">WebKit IDOMComment object.</param>
        protected Comment(IDOMComment Comment)
            : base(Comment)
        {
            this.comment = Comment;
        }

        internal static Comment Create(IDOMComment Comment)
        {
            return new Comment(Comment);
        }
    }

    /// <summary>
    /// Represents a DOM Node containing text.
    /// </summary>
    public class Text : CharacterData
    {
        private IDOMText text;

        /// <summary>
        /// Text constructor.
        /// </summary>
        /// <param name="Text">WebKit IDOMText object.</param>
        protected Text(IDOMText Text)
            : base(Text)
        {
            this.text = Text;
        }

        internal static Text Create(IDOMText Text)
        {
            if (Text is IDOMCDATASection)
                return CDATASection.Create(Text as IDOMCDATASection);
            else
                return new Text(Text);
        }

        /// <summary>
        /// Splits the text node into two nodes at the specified offset.
        /// </summary>
        /// <param name="Offset">The offset of the text.</param>
        /// <returns>A Text node containing the text after the offset.</returns>
        public Text SplitText(int Offset)
        {
            return Text.Create(text.splitText((uint)Offset));
        }
    }

    /// <summary>
    /// Represents a CDATA section in a document.
    /// </summary>
    public class CDATASection : Text
    {
        private IDOMCDATASection cdataSection;

        /// <summary>
        /// CDATASection constructor.
        /// </summary>
        /// <param name="CDATASection">WebKit IDOMCDATASection object.</param>
        protected CDATASection(IDOMCDATASection CDATASection)
            : base(CDATASection)
        {
            this.cdataSection = CDATASection;
        }

        internal static CDATASection Create(IDOMCDATASection CDATASection)
        {
            return new CDATASection(CDATASection);
        }
    }

    /// <summary>
    /// Represents a DOM Document.
    /// </summary>
    public class Document : Node
    {
        private IDOMDocument document;

        /// <summary>
        /// Document constructor.
        /// </summary>
        /// <param name="Document">WebKit IDOMDocument object.</param>
        protected Document(IDOMDocument Document)
            : base(Document)
        {
            this.document = Document;
        }

        internal static Document Create(IDOMDocument Document)
        {
            return new Document(Document);
        }

        #region Document Object Properties

        
        /// <summary>
        /// Gets the Document Type Declaration associated with the document.
        /// </summary>
        public DocumentType DocType
        {
            get
            {
                return DocumentType.Create(document.doctype());
            }
        }

        /// <summary>
        /// Gets the DOM Implementation object that handles this document.
        /// </summary>
        public DocumentImpl Implementation
        {
            get
            {
                return DocumentImpl.Create(document.implementation());
            }
        }

        #endregion

        #region Document Object Methods

        /// <summary>
        /// Creates an attribute node with the specified name.
        /// </summary>
        /// <param name="Name">Name of the attribute.</param>
        /// <returns>The new attribute node.</returns>
        public Attr CreateAttribute(string Name)
        {
            return Attr.Create(document.createAttribute(Name));
        }

        /// <summary>
        /// Cretaes an attribute node with the specified name and namespace.
        /// </summary>
        /// <param name="NamespaceURI">Namespace name of the attribute.</param>
        /// <param name="Name">Name of the attribute.</param>
        /// <returns>The new attribute node.</returns>
        public Attr CreateAttributeNS(string NamespaceURI, string Name)
        {
            return Attr.Create(document.createAttributeNS(NamespaceURI, Name));
        }

        /// <summary>
        /// Creates a comment node with the specified data.
        /// </summary>
        /// <param name="Data">Comment data.</param>
        /// <returns>The new comment node.</returns>
        public Comment CreateComment(string Data)
        {
            return Comment.Create(document.createComment(Data));
        }

        /// <summary>
        /// Creates a CDATA section node with the specified data.
        /// </summary>
        /// <param name="Data">CDATA section data.</param>
        /// <returns>The new CDATA section node.</returns>
        public CDATASection CreateCDATASection(string Data)
        {
            return CDATASection.Create(document.createCDATASection(Data));
        }

        /// <summary>
        /// Creates an empty document fragment object.
        /// </summary>
        /// <returns>The new document fragment.</returns>
        public DocumentFragment CreateDocumentFragment()
        {
            return DocumentFragment.Create(document.createDocumentFragment());
        }

        /// <summary>
        /// Creates a DOM Element with the specified tag name.
        /// </summary>
        /// <param name="TagName">The tag name for the new element.</param>
        /// <returns>The new element.</returns>
        public Element CreateElement(string TagName)
        {
            return Element.Create(document.createElement(TagName));
        }

        /// <summary>
        /// Creates a DOM Element with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace name for the element.</param>
        /// <param name="QualifiedName">Name for the element.</param>
        /// <returns>The new element.</returns>
        public Element CreateElementNS(string NamespaceURI, string QualifiedName)
        {
            return Element.Create(document.createElementNS(NamespaceURI, QualifiedName));
        }

        /// <summary>
        /// Creates an entity reference object with the specified name.
        /// </summary>
        /// <param name="Name">Name of the entity reference object.</param>
        /// <returns>The new entity reference object.</returns>
        public EntityReference CreateEntityReference(string Name)
        {
            return EntityReference.Create(document.createEntityReference(Name));
        }

        /// <summary>
        /// Creates a processing instruction object with the specified target and data.
        /// </summary>
        /// <param name="Target">Target of the processing instruction.</param>
        /// <param name="Data">Data of the processing instruction.</param>
        /// <returns>The new processing instruction object.</returns>
        public ProcessingInstruction CreateProcessingInstruction(string Target, string Data)
        {
            return ProcessingInstruction.Create(document.createProcessingInstruction(Target, Data));
        }

        /// <summary>
        /// Creates a text node containing the specified textual data.
        /// </summary>
        /// <param name="Data">Textual data for the node.</param>
        /// <returns>The new node.</returns>
        public Text CreateTextNode(string Data)
        {
            return Text.Create(document.createTextNode(Data));
        }

        /// <summary>
        /// Returns the first element with the specified id.
        /// </summary>
        /// <param name="id">The id of the element.</param>
        /// <returns>Element with the specified id.</returns>
        public IDOMElement GetElementById(string id)
        {
            return document.getElementById(id);
        }

        /// <summary>
        /// Returns a collection containing all elements with the specified name.
        /// </summary>
        /// <param name="TagName">The name of the elements.</param>
        /// <returns>A NodeList containing all elements with the specified name.</returns>
        public NodeList GetElementsByTagName(string TagName)
        {
            return NodeList.Create(document.getElementsByTagName(TagName));
        }

        /// <summary>
        /// Returns a collection containing all elements with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">The namespace of the elements.</param>
        /// <param name="LocalName">The name of the elements.</param>
        /// <returns>A NodeList containing all elements with the specified namespace and name.</returns>
        public NodeList GetElementsByTagNameNS(string NamespaceURI, string LocalName)
        {
            return NodeList.Create(document.getElementsByTagNameNS(NamespaceURI, LocalName));
        }

        /// <summary>
        /// Imports a node from another document to this document.
        /// </summary>
        /// <param name="NodeToImport">The node to import.</param>
        /// <param name="Deep">Value indicating whether to create a deep copy of the node.</param>
        /// <returns>The imported node.</returns>
        public Node ImportNode(Node NodeToImport, bool Deep)
        {
            return Node.Create(document.importNode((IDOMNode)NodeToImport.GetWebKitObject(), Deep ? 1 : 0));
        }

        /// <summary>
        /// Invokes a method in the scripting environment. 
        /// </summary>
        /// <param name="Method">The name of the method to invoke.</param>
        /// <param name="args">Arguments to pass to the method.</param>
        /// <returns>The return value of the method.</returns>
        public object InvokeScriptMethod(string Method, params object[] args)
        {
            object o = (object)args;
            return document.callWebScriptMethod(Method, ref o, args.Length);
        }

        public NodeList Elements()
        {
            return this.GetElementsByTagName("*");
        }
        #endregion
    }

    /// <summary>
    /// Represents a DOM Element.
    /// </summary>
    public class Element : Node
    {
        private IDOMElement element;
        /// <summary>
        /// Element constructor.
        /// </summary>
        /// <param name="Element">WebKit IDOMElement object.</param>
        protected Element(IDOMElement Element)
            : base(Element)
        {
            this.element = Element;
        }

        /// <summary>
        /// Gets or sets the ID of the current element.
        /// </summary>
        public string ID
        {
            get
            {
                    return element.getAttribute("Id");
            }
            set
            {
                    element.setAttribute("Id", value);
            }
        }


        /// <summary>
        /// Gets or sets the Inner Text of the current element.
        /// </summary>
        public string InnerText
        {
            get
            {
                if (element is IDOMHTMLElement)
                    return (element as IDOMHTMLElement).innerText();
                else if (element is IDOMHTMLInputElement)
                    return (element as IDOMHTMLInputElement).value();
                else if (element is IDOMHTMLTextAreaElement)
                    return (element as IDOMHTMLTextAreaElement).innerText();
                else
                    return element.getAttribute("Value");
            }
            set
            {
                if (element is IDOMHTMLElement)
                   (element as IDOMHTMLElement).setInnerText(value);
                else if (element is IDOMHTMLInputElement)
                    (element as IDOMHTMLInputElement).setValue(value);
                else if (element is IDOMHTMLTextAreaElement)
                    (element as IDOMHTMLTextAreaElement).setInnerText(value);
                else
                    element.setAttribute("Value", value);
            }
        }

        
        internal static Element Create(IDOMElement Element)
        {
            if (Element is IDOMHTMLElement)
                return HTMLElement.Create(Element as IDOMHTMLElement);
            else
                return new Element(Element);
        }

        #region Element Object Properties

        /// <summary>
        /// Gets a NamedNodeMap of attributes for the element.
        /// </summary>
        public NamedNodeMap Attributes
        {
            get
            {
                return NamedNodeMap.Create(element.attributes());
            }
        }
        /// <summary>
        /// Gets a NamedNodeMap of attributes for the element.
        /// </summary>
        public NodeList GetElementsByTagName(string name)
        {
            return NodeList.Create(element.getElementsByTagName(name));
        }
        
        /// <summary>
        /// Gets the name of the element.
        /// </summary>
        public string TagName
        {
            get
            {
                return element.tagName();
            }
        }

        #endregion

        #region Element Object Methods

        /// <summary>
        /// Gets the value of the attribute with the specified name.
        /// </summary>
        /// <param name="Name">Name of the attribute.</param>
        /// <returns>Value of the attribute.</returns>
        public string GetAttribute(string Name)
        {
            return element.getAttribute(Name);
        }

        /// <summary>
        /// Gets the value of the attribute with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the attribute.</param>
        /// <param name="LocalName">Name of the attribute.</param>
        /// <returns>Value of the attribute.</returns>
        public string GetAttributeNS(string NamespaceURI, string LocalName)
        {
            return element.getAttributeNS(NamespaceURI, LocalName);
        }

        /// <summary>
        /// Gets the attribute node with the specified name.
        /// </summary>
        /// <param name="Name">Name of the attribute.</param>
        /// <returns>Attr node.</returns>
        public Attr GetAttributeNode(string Name)
        {
            return Attr.Create(element.getAttributeNode(Name));
        }

        /// <summary>
        /// Gets the attribute node with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the attribute.</param>
        /// <param name="LocalName">Name of the attribute.</param>
        /// <returns>Attr node.</returns>
        public Attr GetAttributeNodeNS(string NamespaceURI, string LocalName)
        {
            return Attr.Create(element.getAttributeNodeNS(NamespaceURI, LocalName));
        }

        /// <summary>
        /// Checks whether the element has an attribute matching the specified name.
        /// </summary>
        /// <param name="Name">Name of the attribute to check for.</param>
        /// <returns>Value indicating whether the element has the specified attribute.</returns>
        public bool HasAttribute(string Name)
        {
            return element.hasAttribute(Name) > 0;
        }

        /// <summary>
        /// Checks whether the element has an attribute matching the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the attribute to check for.</param>
        /// <param name="LocalName">Name of the attribute to check for.</param>
        /// <returns>Value indicating whether the element has the specified attribute.</returns>
        public bool HasAttributeNS(string NamespaceURI, string LocalName)
        {
            return element.hasAttributeNS(NamespaceURI, LocalName) > 0;
        }

        /// <summary>
        /// Removes the attribute with the specified name.
        /// </summary>
        /// <param name="Name">Name of the attribute.</param>
        public void RemoveAttribute(string Name)
        {
            element.removeAttribute(Name);
        }

        /// <summary>
        /// Removes the attribute with the specified namespace and name.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the attribute.</param>
        /// <param name="LocalName">Name of the attribute.</param>
        public void RemoveAttributeNS(string NamespaceURI, string LocalName)
        {
            element.removeAttributeNS(NamespaceURI, LocalName);
        }

        /// <summary>
        /// Removes the specified attribute.
        /// </summary>
        /// <param name="OldAttr">The attribute to remove.</param>
        /// <returns>The removed attribute.</returns>
        public Attr RemoveAttributeNode(Attr OldAttr)
        {
            return Attr.Create(element.removeAttributeNode((IDOMAttr)OldAttr.GetWebKitObject()));
        }

        /// <summary>
        /// Adds a new attribute with the specified name and value.
        /// </summary>
        /// <param name="Name">Name of the attribute.</param>
        /// <param name="Value">Value of the attribute.</param>
        public void SetAttribute(string Name, string Value)
        {
            element.setAttribute(Name, Value);
        }

        /// <summary>
        /// Adds a new attribute with the specified namespace, name and value.
        /// </summary>
        /// <param name="NamespaceURI">Namespace of the attribute.</param>
        /// <param name="QualifiedName">Name of the attribute.</param>
        /// <param name="Value">Value of the attribute.</param>
        public void SetAttributeNS(string NamespaceURI, string QualifiedName, string Value)
        {
            element.setAttributeNS(NamespaceURI, QualifiedName, Value);
        }

        /// <summary>
        /// Adds a new attribute node.
        /// </summary>
        /// <param name="NewAttr">The attribute node to add.</param>
        /// <returns>The added node.</returns>
        public Attr SetAttributeNode(Attr NewAttr)
        {
            return Attr.Create(element.setAttributeNode((IDOMAttr)NewAttr.GetWebKitObject()));
        }

        /// <summary>
        /// Adds a new attribute node.
        /// </summary>
        /// <param name="NewAttr">The attribute node to add.</param>
        /// <returns>The added node.</returns>
        public Attr SetAttributeNodeNS(Attr NewAttr)
        {
            return Attr.Create(element.setAttributeNodeNS((IDOMAttr)NewAttr.GetWebKitObject()));
        }

        /// <summary>
        /// Sets input focus to this element.
        /// </summary>
        public void Focus()
        {
            element.focus();
        }

        /// <summary>
        /// Removes input focus from this element.
        /// </summary>
        public void Blur()
        {
            element.blur();
        }

        #endregion
    }

    public enum ElementType
    {
        Body = 1,
        DIV = 2,
        FORM = 3,
        Image = 4,
        Input = 5,
        LinkOrUknown = 6
    }
    /// <summary>
    /// Represents an unordered list of DOM Nodes where elements are accessed by index.
    /// </summary>
    public class NodeList : IEnumerable<Node>
    {
        private IDOMNodeList nodeList;

        /// <summary>
        /// NodeList constructor.
        /// </summary>
        /// <param name="NodeList">WebKit IDOMNodeList object.</param>
        protected NodeList(IDOMNodeList NodeList)
        {
            this.nodeList = NodeList;
        }

        internal static NodeList Create(IDOMNodeList NodeList)
        {
            return new NodeList(NodeList);
        }

        

        /// <summary>
        /// Gets the number of nodes in the collection.
        /// </summary>
        public int Length
        {
            get
            {
                return (int)nodeList.length();
            }
        }

        /// <summary>
        /// Gets the node at the specified index in the collection.
        /// </summary>
        /// <param name="index">Index of the node to access.</param>
        /// <returns>The node at the specified index.</returns>
        public Node this[int index]
        {
            get
            {
                if (index < 0 || index >= (int)nodeList.length())
                    throw new IndexOutOfRangeException();
                return Node.Create(nodeList.item((uint)index));
            }
        }

        /// <summary>
        /// Gets the node at the specified index in the collection.
        /// </summary>
        /// <param name="Index">Index of the node to access.</param>
        /// <returns>The node at the specified index.</returns>
        public Node GetItem(int Index)
        {
            return this[Index];
        }

        #region IEnumerable<DOMNode> Members

        /// <summary>
        /// Gets an enumerator for the collection.
        /// </summary>
        /// <returns>An enumerator for the collection.</returns>
        public IEnumerator<Node> GetEnumerator()
        {
            for (uint i = 0; i < nodeList.length(); ++i)
                yield return Node.Create(nodeList.item(i));
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            foreach (Node node in this)
                yield return node;
        }

        #endregion
    }

    /// <summary>
    /// Represents the DOCTYPE of a DOM Document.
    /// </summary>
    public class DocumentType : Node
    {
        /// <summary>
        /// An empty document type.
        /// </summary>
        public static DocumentType None;

        private IDOMDocumentType documentType;

        /// <summary>
        /// DocumentType constructor.
        /// </summary>
        /// <param name="DocumentType">WebKit IDOMDocumentType object.</param>
        protected DocumentType(IDOMDocumentType DocumentType)
            : base(DocumentType)
        {
            this.documentType = DocumentType;
        }

        internal static DocumentType Create(IDOMDocumentType DocumentType)
        {
            return new DocumentType(DocumentType);
        }

        #region DocumentType Object Properties

        /// <summary>
        /// Gets a collection containing the entities declared in the DTD.
        /// </summary>
        public NamedNodeMap Entities
        {
            get
            {
                return NamedNodeMap.Create(documentType.entities());
            }
        }

        /// <summary>
        /// Gets the internal DTD as a string.
        /// </summary>
        public string InternalSubset
        {
            get
            {
                return documentType.internalSubset();
            }
        }

        /// <summary>
        /// Gets the name of the DTD.
        /// </summary>
        public string Name
        {
            get
            {
                return documentType.name();
            }
        }

        /// <summary>
        /// Gets a collection containing the notations declared in the DTD.
        /// </summary>
        public NamedNodeMap Notations
        {
            get
            {
                return NamedNodeMap.Create(documentType.notations());
            }
        }

        /// <summary>
        /// Gets the public identifier of the external DTD.
        /// </summary>
        public string PublicID
        {
            get
            {
                return documentType.publicId();
            }
        }

        /// <summary>
        /// Gets the system identifier of the external DTD.
        /// </summary>
        public string SystemID
        {
            get
            {
                return documentType.systemId();
            }
        }

        #endregion
    }

    /// <summary>
    /// Represents a DOM DocumentImplementation object.
    /// </summary>
    public class DocumentImpl
    {
        private IDOMImplementation implementation;

        /// <summary>
        /// DocumentImpl constructor.
        /// </summary>
        /// <param name="Implementation">WebKit IDOMImplementation object.</param>
        protected DocumentImpl(IDOMImplementation Implementation)
        {
            this.implementation = Implementation;
        }

        internal static DocumentImpl Create(IDOMImplementation Implementation)
        {
            return new DocumentImpl(Implementation);
        }

        #region DocumentImpl Object Methods

        /// <summary>
        /// Creates a new DOM Document of the specified doctype.
        /// </summary>
        /// <param name="NamespaceURI">Namespace URI.</param>
        /// <param name="QualifiedName">Qualified Name.</param>
        /// <param name="DocType">Type of the document.</param>
        /// <returns></returns>
        public Document CreateDocument(string NamespaceURI, string QualifiedName, DocumentType DocType)
        {
            return Document.Create(implementation.createDocument(NamespaceURI, QualifiedName, (IDOMDocumentType)DocType.GetWebKitObject()));
        }

        /// <summary>
        /// Creates an empty DocumentType node.
        /// </summary>
        /// <param name="QualifiedName">Name.</param>
        /// <param name="PublicID">Public ID.</param>
        /// <param name="SystemID">System ID.</param>
        /// <returns></returns>
        public DocumentType CreateDocumentType(string QualifiedName, string PublicID, string SystemID)
        {
            return DocumentType.Create(implementation.createDocumentType(QualifiedName, PublicID, SystemID));
        }

        /// <summary>
        /// Checks whether the DOM implementation implements the specified feature.
        /// </summary>
        /// <param name="Feature">Feature to check.</param>
        /// <param name="Version">feature version to check.</param>
        /// <returns>Value indicating whether the specified feature is implemented.</returns>
        public bool HasFeature(string Feature, string Version)
        {
            return implementation.hasFeature(Feature, Version) > 0;
        }

        #endregion
    }

    /// <summary>
    /// Represents a DOM DocumentFragment object.
    /// </summary>
    public class DocumentFragment : Node
    {
        private IDOMDocumentFragment documentFragment;

        /// <summary>
        /// DocumentFragment constructor.
        /// </summary>
        /// <param name="DocumentFragment">WebKit IDOMDocumentFragment object.</param>
        protected DocumentFragment(IDOMDocumentFragment DocumentFragment)
            : base(DocumentFragment)
        {
            this.documentFragment = DocumentFragment;
        }

        internal static DocumentFragment Create(IDOMDocumentFragment DocumentFragment)
        {
            return new DocumentFragment(DocumentFragment);
        }
    }

    /// <summary>
    /// Represents a DOM EntityReference object.
    /// </summary>
    public class EntityReference : Node
    {
        private IDOMEntityReference entityReference;

        /// <summary>
        /// EntityReference constructor.
        /// </summary>
        /// <param name="EntityReference">WebKit IDOMEntityReference object.</param>
        protected EntityReference(IDOMEntityReference EntityReference)
            : base(EntityReference)
        {
            this.entityReference = EntityReference;
        }

        internal static EntityReference Create(IDOMEntityReference EntityReference)
        {
            return new EntityReference(EntityReference);
        }
    }

    /// <summary>
    /// Represents a processing instruction.
    /// </summary>
    public class ProcessingInstruction : Node
    {
        private IDOMProcessingInstruction processingInstruction;

        /// <summary>
        /// ProcessingInstruction constructor.
        /// </summary>
        /// <param name="ProcessingInstruction">WebKit IDOMProcessingInstruction object.</param>
        protected ProcessingInstruction(IDOMProcessingInstruction ProcessingInstruction)
            : base(ProcessingInstruction)
        {
            this.processingInstruction = ProcessingInstruction;
        }

        internal static ProcessingInstruction Create(IDOMProcessingInstruction ProcessingInstruction)
        {
            return new ProcessingInstruction(ProcessingInstruction);
        }

        #region ProcessingInstruction Object Properties

        /// <summary>
        /// Gets or sets the content of this processing instruction.
        /// </summary>
        public string Data
        {
            get
            {
                return processingInstruction.data();
            }
            set
            {
                processingInstruction.setData(value);
            }
        }

        /// <summary>
        /// Gets the target of this processing instruction.
        /// </summary>
        public string Target
        {
            get
            {
                return processingInstruction.target();
            }
        }

        #endregion
        
    }
    internal interface OpenWebKitSharpDOMEvents
        {
            event EventHandler MouseClick;
            event EventHandler MouseOver;
            event EventHandler MouseUp;
            event EventHandler MouseDown;
            event EventHandler KeyUp;
            event EventHandler KeyDown;
            event EventHandler Focus;
        }
}
