using System;
using System.Collections;
using System.Text;
using System.IO;
using System.Net;
using System.Globalization;
using System.Web;
using System.Web.Hosting;
using System.Web.Configuration;

namespace MiniHttpd.Aspx
{
	/// <summary>
	/// Summary description for WorkerRequest.
	/// </summary>
	internal class WorkerRequest : HttpWorkerRequest
	{
		public WorkerRequest(HttpRequest request, DriveFile file, string virtualDir, string physicalDir)
		{
			this.request = request;
			this.file = file;
			this.virtualDir = virtualDir;
			this.physicalDir = physicalDir;
		}

		HttpRequest request;
		DriveFile file;
		string virtualDir;
		string physicalDir;

		bool firstSend = true;

		public override void EndOfRequest()
		{
		}

		public override void FlushResponse(bool finalFlush)
		{
		}

		public override string GetHttpVerbName()
		{
			return request.Method;
		}

		public override string GetHttpVersion()
		{
			return "HTTP/" + request.HttpVersion;
		}

		public override string GetLocalAddress()
		{
			return request.Server.ServerUri.Host;
		}

		public override int GetLocalPort()
		{
			return request.Server.Port;
		}

		public override string GetQueryString()
		{
			return request.Uri.Query;
		}

		public override byte[] GetQueryStringRawBytes()
		{
			return Encoding.Default.GetBytes(request.Uri.Query);
		}

		public override string GetRawUrl()
		{
			return request.Uri.PathAndQuery;
		}

		public override string GetRemoteAddress()
		{
			return request.Client.RemoteAddress;
		}

		public override int GetRemotePort()
		{
			return request.Client.RemotePort;
		}

		public override string GetServerVariable(string name)
		{
			return string.Empty;
		}

		public override string GetUriPath()
		{
			return request.Uri.AbsolutePath;
		}

		public override void SendKnownResponseHeader(int index, string value)
		{
			request.Response.SetHeader(HttpWorkerRequest.GetKnownResponseHeaderName(index), value);
		}

		void SendResponseFromFile(FileStream stream, long offset, long length)
		{
//			if(firstSend)
//			{
//				request.Response.BeginImmediateResponse();
//				firstSend = false;
//			}

			stream.Seek(offset, SeekOrigin.Begin);
			byte[] buffer = new byte[1024];
			int readLength;

			while((readLength = stream.Read(buffer, 0, (int)(length < buffer.Length ? length : buffer.Length))) != 0)
			{
				request.Response.ResponseContent.Write(buffer, 0, readLength);
				length-=readLength;
			}
		}

		public override void SendResponseFromFile(string filename, long offset, long length)
		{
			SendResponseFromFile(new FileStream(filename, FileMode.Open, FileAccess.Read, FileShare.ReadWrite), offset, length);
		}

		public override void SendResponseFromFile(System.IntPtr handle, long offset, long length)
		{
			SendResponseFromFile(new FileStream(handle, FileAccess.Read, false, 1024), offset, length);
		}

		public override void SendResponseFromMemory(byte[] data, int length)
		{
			if(firstSend)
			{
				request.Response.BeginImmediateResponse();
				firstSend = false;
			}
			request.Response.ResponseContent.Write(data, 0, length);
		}

		public override void SendResponseFromMemory(System.IntPtr data, int length)
		{
			if(firstSend)
			{
				request.Response.BeginImmediateResponse();
				firstSend = false;
			}

			byte[] buf = new byte[length];
			System.Runtime.InteropServices.Marshal.Copy(data, buf, 0, length);
			SendResponseFromMemory (buf, length);
		}

		public override void SendStatus(int statusCode, string statusDescription)
		{
			request.Response.ResponseCode = statusCode.ToString();
		}

		public override void SendUnknownResponseHeader(string name, string value)
		{
			request.Response.SetHeader(name, value);
		}

		public override void SendCalculatedContentLength(int contentLength)
		{
			request.Response.SetHeader("Content-Length", contentLength.ToString(CultureInfo.InvariantCulture));
		}

		public override string GetAppPath()
		{
			return virtualDir;
		}

		public override string GetAppPathTranslated()
		{
			return physicalDir;
		}

		public override void CloseConnection()
		{
			request.Client.Disconnect();
		}

		public override string GetFilePath()
		{
			return request.Uri.AbsolutePath;
		}

		public override string GetFilePathTranslated()
		{
			return file.Path;
		}

		public override string GetKnownRequestHeader(int index)
		{
			return request.Headers[HttpWorkerRequest.GetKnownRequestHeaderName(index)];
		}

		public override byte[] GetPreloadedEntityBody()
		{
			return request.PostData.ToArray();
		}

		public override int ReadEntityBody(byte[] buffer, int size)
		{
			//TODO: implement this
			return 0;
		}

		public override string GetProtocol()
		{
			return request.Protocol.ToString(CultureInfo.InvariantCulture).ToUpper(CultureInfo.InvariantCulture);
		}

		public override string GetUnknownRequestHeader(string name)
		{
			return request.Headers[name];
		}

		public override string[][] GetUnknownRequestHeaders()
		{
			ArrayList ret = new ArrayList();
			string[] keys = request.Headers.AllKeys;

			foreach(string key in keys)
				if(HttpWorkerRequest.GetKnownRequestHeaderIndex(key) < 0)
					ret.Add(new string[] {key, request.Headers[key]});

			return ret.ToArray(typeof(string[])) as string[][];
		}

		public override bool HeadersSent()
		{
			return request.Response.HeadersSent;
		}

		public override bool IsClientConnected()
		{
			return request.Client.IsConnected;
		}

		public override bool IsEntireEntityBodyIsPreloaded()
		{
			return request.ContentLength == request.PostData.Length;
		}

		public override bool IsSecure()
		{
			//TODO: HTTPS
			return false;
		}

		public override string MapPath(string virtualPath)
		{
			IPhysicalResource resource = (request.Server as AspxWebServer).NavigateToUrl(virtualPath) as IPhysicalResource;
			if(resource == null)
				return null;
			return resource.Path;
		}

		public override string GetPathInfo()
		{
			//TODO: implement this once HttpHandlers is ready
			return string.Empty;
		}

		public override string MachineConfigPath
		{
			get
			{
				return Path.Combine(Path.Combine(Path.GetDirectoryName(typeof(object).Assembly.Location.Replace('/', '\\')), "Config"), "machine.config");
			}
		}

		public override string MachineInstallDirectory
		{
			get
			{
				return System.Threading.Thread.GetDomain().GetData(".hostingInstallDir").ToString();
			}
		}

	}
}
