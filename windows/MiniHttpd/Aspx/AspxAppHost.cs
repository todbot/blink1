using System;
using System.IO;
using System.Web;
using System.Web.Hosting;

namespace MiniHttpd.Aspx
{
	/// <summary>
	/// Summary description for AspxAppHost.
	/// </summary>
	internal class AspxAppHost : MarshalByRefObject
	{
		public void ProcessRequest(HttpRequest request, DriveFile file, string virtualPath, string physicalDir)
		{
			HttpRuntime.ProcessRequest(new WorkerRequest(request, file, virtualPath, physicalDir));
		}

		public override object InitializeLifetimeService()
		{
			return null;
		}

		public void Unload()
		{
			HttpRuntime.UnloadAppDomain();
		}

	}
}
