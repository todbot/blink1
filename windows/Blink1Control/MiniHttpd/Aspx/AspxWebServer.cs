using System;
using System.Collections.Specialized;
using System.Net;
using System.IO;
using System.Reflection;
using System.Web;
using System.Web.Hosting;
using MiniHttpd;

namespace MiniHttpd.Aspx
{
	/// <summary>
	/// Represents an ASPX web server.
	/// </summary>
	public class AspxWebServer : HttpWebServer
	{
		/// <summary>
		/// Creates an ASP.NET capable web server on the default port, default address and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		public AspxWebServer()
		{
		}

		/// <summary>
		/// Creates an ASP.NET capable web server on the specified port, default address and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		public AspxWebServer(int port) : this(port, new VirtualDirectory("/", null))
		{
		}

		/// <summary>
		/// Creates an ASP.NET capable web server on the specified port, specified root and default address.
		/// </summary>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		/// <param name="root">An <see cref="IDirectory"/> specifying the root directory. Its <c>Parent</c> must be <c>null</c>.</param>
		public AspxWebServer(int port, IDirectory root) : this(IPAddress.Any, port, root)
		{
		}

		/// <summary>
		/// Creates an ASP.NET capable web server on the specified address, specified port and default <see cref="VirtualDirectory"/> root.
		/// </summary>
		/// <param name="localAddress">The local <see cref="IPAddress"/> to listen on.</param>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		public AspxWebServer(IPAddress localAddress, int port) : this(localAddress, port, new VirtualDirectory("/", null))
		{
		}

		/// <summary>
		/// Creates an ASP.NET capable web server on the specified address, port and root.
		/// </summary>
		/// <param name="localAddress">The local <see cref="IPAddress"/> to listen on.</param>
		/// <param name="port">An open port of range 1-65535 to listen on. Specify 0 to use any open port.</param>
		/// <param name="root">An <see cref="IDirectory"/> specifying the root directory. Its <c>Parent</c> must be <c>null</c>.</param>
		public AspxWebServer(IPAddress localAddress, int port, IDirectory root) : base(localAddress, port)
		{
			base.DefaultPages.Add("index.aspx");
			base.DefaultPages.Add("default.aspx");
			base.DefaultPages.Add("index.asmx");
			base.DefaultPages.Add("default.asmx");
			base.DefaultPages.Add("index.asax");
			base.DefaultPages.Add("default.asax");
		}

		/// <summary>
		/// Retursn the resource at the given relative URL.
		/// </summary>
		/// <param name="url">A relative URL to the resource to return.</param>
		/// <param name="throwOnError">A value indicating whether or not the method should throw an <see cref="HttpRequestException"/> if the resource is not available.</param>
		/// <returns></returns>
		protected override IResource NavigateToUrl(string url, bool throwOnError)
		{
			//TODO: implement this to work with ASPX HttpHandlers properly
			return base.NavigateToUrl (url, throwOnError);
		}

		static bool IsAspxFile(string path)
		{
			string ext = Path.GetExtension(path);
			if(string.Compare(ext, ".aspx", true, System.Globalization.CultureInfo.InvariantCulture) == 0 ||
				string.Compare(ext, ".asmx", true, System.Globalization.CultureInfo.InvariantCulture) == 0)
				return true;
			else
				return false;
		}

		/// <summary>
		/// Returns the file or index page for the given request and resource.
		/// </summary>
		/// <param name="request">The request to which to respond.</param>
		/// <param name="resource">The requested resource.</param>
		protected override void SendFileOrIndex(HttpRequest request, IResource resource)
		{
			for(IResource current = resource; current != null; current = current.Parent)
			{
				if(current is AspxAppDirectory)
				{
					AspxAppDirectory app = current as AspxAppDirectory;
					if(resource is DriveFile)
					{
						DriveFile file = resource as DriveFile;
						if(IsAspxFile(file.Path))
						{
							app.ProcessRequest(request, file);
							return;
						}
					}
					else if(resource is IDirectory)
					{
						IDirectory targetDir = resource as IDirectory;
						IFile defaultPage = null;
						foreach(string page in DefaultPages)
						{
							defaultPage = targetDir.GetFile(page);
							if(defaultPage != null)
							{
								DriveFile file = defaultPage as DriveFile;
								if(file != null && IsAspxFile(file.Path))
									throw new MovedException(request.Uri.AbsolutePath + defaultPage.Name);
								else
								{
									base.SendFileOrIndex(request, resource);
									return;
								}
							}
						}
					}
				}
			}

			base.SendFileOrIndex (request, resource);
		}
	}
}
