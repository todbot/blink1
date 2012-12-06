#if DEBUG
//#define DUMP
#endif

using System;
using System.Net;
using System.IO;
using System.Net.Sockets;
using System.Text;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a client connection in an HTTP/1.x transaction.
	/// </summary>
	public class HttpClient : MarshalByRefObject, IDisposable
	{
		internal HttpClient(Socket socket, HttpServer server)
		{
			this.server = server;
			this.socket = socket;
			IPEndPoint remoteEndPoint = (IPEndPoint)socket.RemoteEndPoint;
			isConnected = true;
			this.remoteAddress = remoteEndPoint.Address.ToString();
			this.remotePort = remoteEndPoint.Port;

			server.Stopping += new EventHandler(server_Stopping);

			stream = new NetworkStream(socket, true);

			try
			{
				stream.BeginRead(buffer, 0, buffer.Length, new AsyncCallback(OnReceiveData), this);
				
			}
			catch(IOException)
			{
				Disconnect();
				throw;
			}
			server.OneHertzTick += new EventHandler(server_OneHertzTick);
		}

		/// <summary>
		/// Disposes the client if it hasn't already been disposed.
		/// </summary>
		~HttpClient()
		{
			Disconnect();
		}

		/// <summary>
		/// Returns a hash code generated from the client's IP address.
		/// </summary>
		/// <returns></returns>
		public override int GetHashCode()
		{
			return remoteAddress.GetHashCode();
		}

		/// <summary>
		/// Returns a value indicating whether this client is the same instance as another.
		/// </summary>
		/// <param name="obj">Another <see>HttpClient</see> to which to compare this object.</param>
		/// <returns>True if this is the same instance of the <see>HttpClient</see>, otherwise false.</returns>
		public override bool Equals(object obj)
		{
			return this == obj;
		}

		/// <summary>
		/// Occurs when a client is disconnected.
		/// </summary>
		public event EventHandler Disconnected;

		/// <summary>
		/// Gets a value indicating whether the client is currently connected.
		/// </summary>
		public bool IsConnected
		{
			get
			{
				return isConnected;
			}
		}

		/// <summary>
		/// Gets the IP address of the client.
		/// </summary>
		public string RemoteAddress
		{
			get
			{
				return remoteAddress;
			}
		}

		/// <summary>
		/// Gets the port through which the client is connected.
		/// </summary>
		public int RemotePort
		{
			get
			{
				return remotePort;
			}
		}

		#region IDisposable Members

		/// <summary>
		/// Disconnects the client.
		/// </summary>
		public void Dispose()
		{
			Disconnect();
		}

		#endregion

		static int FindByte(byte[] buf, int offset, int count, byte b)
		{
			for(int i = offset; i < buf.Length && count > 0; i++, count--)
				if(buf[i] == b)
					return i;
			return -1;
		}

		internal HttpServer server;
		internal NetworkStream stream;
		Socket socket;
		byte[] buffer = new byte[512];
		bool isConnected;
		HttpRequest request;
		StringBuilder textBuf;
		double idleTime;
		bool preventTimeout;
		string remoteAddress;
		int remotePort;

		System.Threading.Thread processingThread;

		static string EscapeNewLines(string value)
		{
			return value.Replace("\r", "\\r").Replace("\n", "\\n");
		}

		void OnReceiveData(IAsyncResult ar)
		{
			if(processingThread == null)
				processingThread = System.Threading.Thread.CurrentThread;
			lock(socket)
			{
				try
				{
					idleTime = 0;
					if(!isConnected)
						return;

					int dataLen = stream.EndRead(ar);

#if DUMP
					System.Diagnostics.Debug.Write(Encoding.Default.GetString(buffer, 0, dataLen));
					System.Diagnostics.Debug.WriteLine("+++++++++++++++++++++++");
#endif

					if(dataLen < 1)
					{
						Disconnect();
						return;
					}

					if(textBuf != null)
						if(textBuf.Length > 0x8000)
						{
							Disconnect();
							return;
						}

					for(int bufPos = 0; bufPos < dataLen; bufPos++)
					{
						if(request == null)
						{
							request = new HttpRequest(this);
							textBuf = new StringBuilder(buffer.Length);
						}

						if(request.Mode == HttpRequest.DataMode.Text)
						{
							int len = FindByte(buffer, bufPos, dataLen, 10);
							if(len != -1)
							{
								textBuf.Append(Encoding.Default.GetString(buffer, bufPos, len+1-bufPos));
								request.ProcessLine(textBuf.ToString().Trim('\r', '\n'));
								textBuf.Length = 0;
								bufPos = len;
							}
							else
							{
								textBuf.Append(Encoding.Default.GetString(buffer, bufPos, dataLen - bufPos));
								bufPos = dataLen;
							}
						}

						if(request.Mode == HttpRequest.DataMode.Binary)
						{
							request.ProcessData(buffer, bufPos, dataLen - bufPos);
							bufPos = dataLen;
						}

						if(request.IsRequestFinished)
						{
							preventTimeout = true;
							bool forceDisconnect = false;
							try
							{
								server.OnRequestReceived(this, request);
							}
							catch(HttpRequestException e)
							{
								request.Response.ResponseCode = e.ResponseCode;
								//TODO: custom error messages, fix forceDisconnect prior to implementing HTTPS
								if(e.ResponseCode == "500")
									server.Log.WriteLine(e.ToString());
								forceDisconnect = true;
							}
							request.SendResponse();
							ConnectionMode modeTemp = request.ConnectionMode;

							string requestVer = request.HttpVersion;

							request.Dispose();
							request = null;
							idleTime = 0;
							preventTimeout = false;

							//TODO: don't disconnect 1.0... wget is being sending weird headers, do this workaround for now
							if(modeTemp == ConnectionMode.Close || forceDisconnect || requestVer == "1.0")
							{
								Disconnect();
								return;
							}
						}
					}

					if(isConnected)
						stream.BeginRead(buffer, 0, buffer.Length, new AsyncCallback(OnReceiveData), this);
				}
				catch(SocketException)
				{
					Disconnect();
				}
				catch(IOException e)
				{
					Disconnect();
					if(!(e.InnerException is SocketException))
					{
#if !DEBUG
						server.Log.WriteLine("Error: " + e.ToString());
#else
						throw;
#endif
					}
				}
				catch(System.Threading.ThreadAbortException)
				{
					Disconnect();
				}
#if !DEBUG
				catch(Exception e)
				{
					server.Log.WriteLine("Error: " + e.ToString());
					Disconnect();
				}
#endif
			}
		}

		internal void Disconnect()
		{
			if(isConnected)
			{
				lock(socket)
				{
					if(Disconnected != null)
						Disconnected(this, null);
					if(request != null)
						request.Dispose();
					stream.Close();
					socket.Close();
					isConnected = false;
					GC.SuppressFinalize(this);
				}
			}
		}

		private void server_Stopping(object sender, EventArgs e)
		{
			if(processingThread != null)
				processingThread.Abort();
		}

		private void server_OneHertzTick(object sender, EventArgs e)
		{
			idleTime += 1000;
			if(idleTime >= server.Timeout && !preventTimeout)
				Disconnect();
		}
	}
}
