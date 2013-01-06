using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Collections.Specialized;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a web server with a tree-file structure.
	/// </summary>
	public class HttpWebServer : HttpServer
	{
		/// <summary>
		/// Creates a web server on the default port, default address and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		public HttpWebServer() : this(80)
		{
		}

		/// <summary>
		/// Creates a web server on the specified port, default address and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		public HttpWebServer(int port) : this(port, new VirtualDirectory("/", null))
		{
		}

		/// <summary>
		/// Creates a web server on the specified port, specified root and default address.
		/// </summary>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		/// <param name="root">An <see cref="IDirectory"/> specifying the root directory. Its <c>Parent</c> must be <c>null</c>.</param>
		public HttpWebServer(int port, IDirectory root) : this(IPAddress.Any, port, root)
		{
		}

		/// <summary>
		/// Creates a web server on the specified address, specified port and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		/// <param name="localAddress">The local <see cref="IPAddress"/> to listen on.</param>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		public HttpWebServer(IPAddress localAddress, int port) : this(localAddress, port, new VirtualDirectory("/", null))
		{
		}

		/// <summary>
		/// Creates a web server on the specified address, port and root.
		/// </summary>
		/// <param name="localAddress">The local <see cref="IPAddress"/> to listen on.</param>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		/// <param name="root">An <see cref="IDirectory"/> specifying the root directory. Its <c>Parent</c> must be <c>null</c>.</param>
		public HttpWebServer(IPAddress localAddress, int port, IDirectory root) : base(localAddress, port)
		{
			this.Root = root;

			this.ValidRequestReceived += new MiniHttpd.HttpServer.RequestEventHandler(server_ValidRequestReceived);

			defaultPages.Add("index.htm");
			defaultPages.Add("index.html");
			defaultPages.Add("default.htm");
			defaultPages.Add("default.html");
		}

		/// <summary>
		/// Disposes the server if it hasn't already been disposed.
		/// </summary>
		~HttpWebServer()
		{
			Dispose();
		}

		IDirectory root;
		ArrayList defaultPages = new ArrayList();
		IFile indexPage = new IndexPage();

		/// <summary>
		/// Event that occurs when the root directory is changed.
		/// </summary>
		public event EventHandler RootChanged;

		/// <summary>
		/// Gets or sets the root directory. The old root is disposed when a new value is assigned.
		/// </summary>
		public IDirectory Root
		{
			get
			{
				return root;
			}
			set
			{
				if(root != null)
					root.Dispose();
				if(value.Parent != null)
					throw new InvalidOperationException("Parent of root directory must be null.");
				root = value;

				if(RootChanged != null)
					RootChanged(this, null);
			}
		}

		/// <summary>
		/// Gets or sets an <see cref="ArrayList"/> of strings representing default page names.
		/// </summary>
		public ArrayList DefaultPages
		{
			get
			{
				return defaultPages;
			}
		}

		/// <summary>
		/// Gets or sets the index page to use.
		/// </summary>
		public IFile IndexPage
		{
			get
			{
				return indexPage;
			}
			set
			{
				indexPage = value;
			}
		}

		/// <summary>
		/// Returns the resource at the given relative URL.
		/// </summary>
		/// <param name="url">A relative URL to the resource to return.</param>
		/// <returns>A file corresponding to the given URL. Returns <c>null</c> if the resource is not available.</returns>
		public IResource NavigateToUrl(string url)
		{
			return NavigateToUrl(url, false);
		}

		/// <summary>
		/// Retursn the resource at the given relative URL.
		/// </summary>
		/// <param name="url">A relative URL to the resource to return.</param>
		/// <param name="throwOnError">A value indicating whether or not the method should throw an <see cref="HttpRequestException"/> if the resource is not available.</param>
		/// <returns></returns>
		protected virtual IResource NavigateToUrl(string url, bool throwOnError)
		{
			string[] pathNodes = GetUriPathNodes(url);

			foreach(string s in pathNodes)
				if(!IsValidFilename(s))
				{
					if(throwOnError)
						throw new HttpRequestException("404");
					else
						return null;
				}

			// First node (always '/')
			IDirectory currentDirectory = root;
			IFile targetFile = null;
			IDirectory targetDir = root;

			// Middle nodes
			for(int i = 1; i < pathNodes.Length-1; i++)
			{
				string node = pathNodes[i];
				string trimmedNode = node.Trim('/');

				currentDirectory = currentDirectory.GetDirectory(trimmedNode);
				if(currentDirectory == null)
				{
					if(throwOnError)
						throw new HttpRequestException("404");
					else
						return null;
				}
			}

			// Last node
			if(pathNodes.Length != 1)
			{
				string node = pathNodes[pathNodes.Length-1];
				string trimmedNode = node.Trim('/');

				targetFile = currentDirectory.GetFile(node);

				if(targetFile == null)
				{
					targetDir = currentDirectory.GetDirectory(trimmedNode);
					if(targetDir != null && trimmedNode == node)
					{
						if(throwOnError)
							throw new MovedException(url + "/");
						else
							return null;
					}
				}
			}

			if(targetFile != null)
				return targetFile;
			else if(targetDir != null)
				return targetDir;
			else
				throw new HttpRequestException("404");
		}

		/// <summary>
		/// Provides a list of characters deemed invalid in Windows NT file strings.
		/// </summary>
		public static readonly char[] InvalidFileChars = new char[] {'?', '"', '\\', '<', '>', '*', '|', ':'};

		/// <summary>
		/// Checks whether a file is a valid filename on the current OS.
		/// </summary>
		/// <param name="filename">The filename to be examined.</param>
		/// <returns>True if the filename is safe, otherwise false.</returns>
		public static bool IsValidFilename(string filename)
		{
			if(filename.IndexOfAny(InvalidFileChars) >= 0 || filename.IndexOfAny(Path.InvalidPathChars) >= 0 || filename.IndexOf("..") >= 0)
				return false;
			return true;
		}

		/// <summary>
		/// Returns the file or index page for the given request and resource.
		/// </summary>
		/// <param name="request">The request to which to respond.</param>
		/// <param name="resource">The requested resource.</param>
		protected virtual void SendFileOrIndex(HttpRequest request, IResource resource)
		{

			if(resource is IFile)
			{
				try
				{
					IFile targetFile = resource as IFile;
					request.Response.ContentType = targetFile.ContentType;
					targetFile.OnFileRequested(request, targetFile.Parent);
				}
				catch(FileNotFoundException)
				{
					throw new HttpRequestException("404");
				}
				catch(DirectoryNotFoundException)
				{
					throw new HttpRequestException("404");
				}
				catch(System.Security.SecurityException)
				{
					throw new HttpRequestException("403");
				}
				catch(PathTooLongException)
				{
					throw new HttpRequestException("414");
				}
				return;
			}
			else
			{
				IDirectory targetDir = resource as IDirectory;
				IFile defaultPage = null;
				foreach(string page in defaultPages)
				{
					defaultPage = targetDir.GetFile(page);
					if(defaultPage != null)
					{
						defaultPage.OnFileRequested(request, targetDir);
						request.Response.ContentType = defaultPage.ContentType;
						return;
					}
				}

				request.Response.ContentType = indexPage.ContentType;
				indexPage.OnFileRequested(request, targetDir);

				return;
			}
		}

		//TODO: a system for IFile and IDirectory handler extensibility

		private void server_ValidRequestReceived(object sender, RequestEventArgs e)
		{

			if(e.Request.Uri.AbsolutePath.IndexOfAny(new char[] {'*', '?'}) >= 0)
			{
				throw new HttpRequestException("401");
			}

			IResource resource;

			try
			{
				resource = NavigateToUrl(e.Request.Uri.AbsolutePath, true);

				SendFileOrIndex(e.Request, resource);
			}
			catch(MovedException ex)
			{
				if(this.LogRequests)
					Log.WriteLine("Redirect: {0} to {1}", e.Request.Uri.AbsoluteUri, ex.NewPath);
				e.Request.Response.SetHeader("Location", ex.NewPath);
				throw;
			}
		}

		static string[] GetUriPathNodes(string absolutePath)
		{
			ArrayList nodes = new ArrayList();
			string path = absolutePath.Replace('\\', '/');
			if(path.Length <= 1)
				return new string[] {UrlEncoding.Decode(path)};
			//	return new string[] {path};

			int start = 0;
			for(int i = 0; i < path.Length; i++)
			{
				if(path[i] == '/')
				{
					while(i < path.Length && path[i] == '/')
						i++;
					nodes.Add(UrlEncoding.Decode(path.Substring(start, i-start)));
					//nodes.Add(path.Substring(start, i-start));
					start = i;
				}
			}

			if(start != path.Length)
				nodes.Add(UrlEncoding.Decode(path.Substring(start, path.Length-start)));
			//nodes.Add(path.Substring(start, path.Length-start));

			return nodes.ToArray(typeof(string)) as string[];
		}

		/// <summary>
		/// Returns the full relative path of the specified directory as seen on the server.
		/// </summary>
		/// <param name="directory">The directory of which to retrieve the full path.</param>
		/// <returns>The full path of the specified directory.</returns>
		public static string GetDirectoryPath(IDirectory directory)
		{
			System.Text.StringBuilder path = new System.Text.StringBuilder();
			ArrayList pathList = new ArrayList();

			for(IDirectory dir = directory; dir != null; dir = dir.Parent)
				pathList.Add(dir.Name);
			pathList.Reverse();

			for(int i = 0; i < pathList.Count; i++)
				path.Append((pathList[i] as string) + "/");

			string ret = path.ToString();

			if(ret == "//")
				return "/";
			if(ret.StartsWith("//"))
				ret = ret.Substring(1);

			return ret;
		}

		/// <summary>
		/// Returns the <see cref="Uri"/> of a specified resource.
		/// </summary>
		/// <param name="resource">An <see cref="IResource"/> specifying the file for which to return a <see cref="Uri"/>.</param>
		/// <returns>A <see cref="Uri"/> to the specified file.</returns>
		public Uri GetUrl(IResource resource)
		{
			string url = UrlEncoding.Encode(GetDirectoryPath(resource.Parent).TrimStart('/') + resource.Name);
			if(resource is IDirectory)
				url += '/';
			return new Uri(ServerUri, url);
		}

		/// <summary>
		/// Dispose root directory.
		/// </summary>
		public override void Dispose()
		{
			root.Dispose();
			base.Dispose ();
		}

	}
}