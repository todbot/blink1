using System;
using System.Collections;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace MiniHttpd
{
	/// <summary>
	/// An HTTP listener.
	/// </summary>
	public class HttpServer : MarshalByRefObject, IDisposable
	{

		#region Constructors

		/// <summary>
		/// Creates an <see cref="HttpServer"/> on the default port and address.
		/// </summary>
		public HttpServer() : this(80)
		{
		}

		/// <summary>
		/// Creates an <see cref="HttpServer"/> on the specified port and default address.
		/// </summary>
		/// <param name="port">An available port between 1 and 65535. Specify 0 to use any open port.</param>
		public HttpServer(int port) : this(IPAddress.Any, port)
		{
		}

		/// <summary>
		/// Creates an <see cref="HttpServer"/> on the specified port and address.
		/// </summary>
		/// <param name="localAddress">An <see cref="IPAddress"/> on which to listen for HTTP requests.</param>
		/// <param name="port">An available port between 1 and 65535. Specify 0 to use any open port.</param>
		public HttpServer(IPAddress localAddress, int port)
		{
			this.port = port;
			this.localAddress = localAddress;

			ServerUri = new Uri("http://" +
				Dns.GetHostName() +
				(port != 80 ? ":" + port.ToString(System.Globalization.CultureInfo.InvariantCulture) : "")
				);
			
			System.Reflection.AssemblyName name = System.Reflection.Assembly.GetExecutingAssembly().GetName();
			this.serverName = name.Name + "/" + name.Version.ToString();

			idleTimer = new Timer(new TimerCallback(TimerCallback), null, 0, 1000);

			try
			{
				this.authenticator = new BasicAuthenticator();
			}
			catch(NotImplementedException)
			{
				//TODO: make an even simpler authenticator for .net implementations without md5
			}
			catch(MemberAccessException)
			{
			}
		}

		/// <summary>
		/// Disposes the server if it hasn't already been disposed.
		/// </summary>
		~HttpServer()
		{
			Dispose();
		}

		#endregion

		#region IDisposable Members

		/// <summary>
		/// Occurs when the server is disposed.
		/// </summary>
		public event EventHandler Disposed;

		bool isDisposed;

		/// <summary>
		/// Shuts down and disposes the server.
		/// </summary>
		public virtual void Dispose()
		{
			Stop();

			if(isDisposed)
				return;
			isDisposed = true;

			idleTimer.Dispose();

			if(Disposed != null)
				Disposed(this, null);
		}

		#endregion

		#region Server Settings

		int port;
		/// <summary>
		/// Gets or sets the port on which to listen to HTTP requests. Specify 0 to use any open port.
		/// </summary>
		public int Port
		{
			get
			{
				return port;
			}
			set
			{
				if(isRunning)
					throw new InvalidOperationException("Port cannot be changed while the server is running.");
				port = value;

				UriBuilder uri = new UriBuilder(ServerUri);
				uri.Port = port;
				ServerUri = uri.Uri;
			}
		}

		/// <summary>
		/// Gets or sets the server's host name.
		/// </summary>
		public string HostName
		{
			get
			{
				return ServerUri.Host;
			}
			set
			{
				UriBuilder uri = new UriBuilder(ServerUri);
				uri.Host = value;
				ServerUri = uri.Uri;
			}
		}

		IPAddress localAddress;
		/// <summary>
		/// Gets or sets the IP address on which to listen to HTTP requests.
		/// </summary>
		public IPAddress LocalAddress
		{
			get
			{
				return localAddress;
			}
			set
			{
				if(isRunning)
					return;
				localAddress = value;
			}
		}

		/// <summary>
		/// Gets the highest HTTP version recognized by the server.
		/// </summary>
		public static string HttpVersion
		{
			get
			{
				return "1.1";
			}
		}

		string serverName;
		/// <summary>
		/// Gets or sets the name of the server.
		/// </summary>
		public string ServerName
		{
			get
			{
				return serverName;
			}
			set
			{
				serverName = value;
			}
		}

		Thread listenerThread;
		/// <summary>
		/// Gets the thread on which the listener is operating.
		/// </summary>
		public Thread ListenerThread
		{
			get
			{
				return listenerThread;
			}
		}

		/// <summary>
		/// Occurs when the server's <see cref="Uri"/> changes.
		/// </summary>
		public event EventHandler ServerUriChanged;
		Uri serverUri;

		/// <summary>
		/// Gets or sets the server's <see cref="Uri"/>.
		/// </summary>
		public Uri ServerUri
		{
			get
			{
				return serverUri;
			}
			set
			{
				serverUri = value;
				this.relUriCache.Clear();
				if(ServerUriChanged != null)
					ServerUriChanged(this, null);
			}
		}

		double timeout = 100000;
		/// <summary>
		/// Gets or sets the time, in milliseconds, of the time after which a client is idle for that the client should be disconnected.
		/// </summary>
		public double Timeout
		{
			get
			{
				return timeout;
			}
			set
			{
				timeout = value;
			}
		}

		int uriCacheMax = 1000;
		/// <summary>
		/// Gets or sets the maximum size of the URI cache.
		/// </summary>
		public int UriCacheMax
		{
			get
			{
				return uriCacheMax;
			}
			set
			{
				uriCacheMax = value;
				if(absUriCache.Count > value)
					absUriCache.Clear();
				if(relUriCache.Count > value)
					relUriCache.Clear();
				if(uriHostsCount > value)
				{
					uriHostsCount = 0;
					uriHosts.Clear();
				}
			}
		}

		bool logRequests;
		/// <summary>
		/// Gets or sets a value indicating whether the server should log HTTP requests.
		/// </summary>
		public bool LogRequests
		{
			get
			{
				return logRequests;
			}
			set
			{
				logRequests = value;
			}
		}

		bool logConnections;
		/// <summary>
		/// Gets or sets a value indicating whether the server should log client connections and disconnections.
		/// </summary>
		public bool LogConnections
		{
			get
			{
				return logConnections;
			}
			set
			{
				logConnections = value;
			}
		}

		bool requireAuthentication;

		/// <summary>
		/// Gets or sets a value indicating whether the server requires authentication for its resources to be accessed.
		/// </summary>
		public bool RequireAuthentication
		{
			get
			{
				return requireAuthentication;
			}
			set
			{
				requireAuthentication = value;
			}
		}

		string authenticateRealm;
		/// <summary>
		/// Gets or sets a value of the realm presented to the user when authenticating.
		/// </summary>
		public string AuthenticateRealm
		{
			get
			{
				return authenticateRealm;
			}
			set
			{
				authenticateRealm = value;
			}
		}

		long maxPostLength = 4*1024*1024;
		/// <summary>
		/// Gets or sets the maximum length of content that the client can post.
		/// </summary>
		public long MaxPostLength
		{
			get
			{
				return maxPostLength;
			}
			set
			{
				maxPostLength = value;
			}
		}

		#endregion

		#region Caches

		Hashtable absUriCache = new Hashtable();
		Hashtable relUriCache = new Hashtable();

		internal Uri GetAbsUri(string uri)
		{
			Uri ret;
			lock(absUriCache)
			{
				ret = absUriCache[uri] as Uri;
				if(ret == null)
				{
					if(absUriCache.Count > uriCacheMax)
						absUriCache.Clear();
					ret = new Uri(uri, true);
					absUriCache[uri] = ret;
				}
			}
			return ret;
//			return new Uri(uri);
		}

		internal Uri GetRelUri(string uri)
		{
			Uri ret;
			lock(relUriCache)
			{
				ret = relUriCache[uri] as Uri;
				if(ret == null)
				{
					if(relUriCache.Count > uriCacheMax)
						relUriCache.Clear();
					ret = new Uri(serverUri, uri, true);
					relUriCache[uri] = ret;
				}
			}
			return ret;
//			return new Uri(serverUri, uri);
		}

		Hashtable uriHosts = new Hashtable();
		int uriHostsCount;

		internal Uri GetHostUri(string host, string uri)
		{
			Uri ret;

			lock(uriHosts)
			{
				Hashtable uris = uriHosts[host] as Hashtable;
				if(uris == null)
				{
					uris = new Hashtable();
					uriHosts.Add(host, uris);
				}
				ret = uris[uri] as Uri;
				if(ret == null)
				{
					if(uriHostsCount > uriCacheMax)
					{
						uriHosts.Clear();
						uriHostsCount = 0;
					}
					//BUG: UriBuilder .ctor needs bool dontEscape parameter
//					UriBuilder ub = new UriBuilder(uri);
//					string[] hostSplit = host.Split(':');
//					if(hostSplit.Length == 1)
//					{
//						ub.Host = hostSplit[0];
//					}
//					else if(hostSplit.Length == 2)
//					{
//						ub.Host = hostSplit[0];
//						ub.Port = int.Parse(hostSplit[1], System.Globalization.NumberStyles.Integer, System.Globalization.CultureInfo.InvariantCulture);
//					}
//					else
//						throw new FormatException();
//					ret = ub.Uri;
					ret = new Uri(new Uri("http://" + host), uri, true);
					uris[uri] = ret;
				}
			}

			return ret;
		}

		#endregion

		#region Listener

		TcpListener listener;

		bool isRunning;
		bool stop;

		/// <summary>
		/// Gets a value indicating whether the server is currently listening for connections.
		/// </summary>
		public bool IsRunning
		{
			get
			{
				return isRunning;
			}
		}

		/// <summary>
		/// Occurs when the server is started.
		/// </summary>
		public event EventHandler Started;

		/// <summary>
		/// Occurs when the server is about to stop.
		/// </summary>
		public event EventHandler Stopping;

		/// <summary>
		/// Occurs when the server is stopped.
		/// </summary>
		public event EventHandler Stopped;

		/// <summary>
		/// Starts listening for connections.
		/// </summary>
		public void Start()
		{
			if(isRunning)
				return;

			Log.WriteLine("Server: " + ServerName);
			Log.WriteLine("CLR: " + Environment.Version);

			listenerThread = new Thread(new ThreadStart(DoListen));
			
			listener = new TcpListener(localAddress, port);
			listener.Start();

			Port = ((IPEndPoint)listener.LocalEndpoint).Port;

			isRunning = true;

			if(Started != null)
				Started(this, null);

			Log.WriteLine("Server running at " + ServerUri);

			listenerThread.Start();
		}

		/// <summary>
		/// Stops listening for connections.
		/// </summary>
		public void Stop()
		{
			if(!isRunning)
				return;
			Log.WriteLine("Server stopping");
			stop = true;
			if(listener != null)
				listener.Stop();

			if(Stopping != null)
				Stopping(this, null);
			
			try
			{
				JoinListener();
			}
			catch(MemberAccessException)
			{
			}
			catch(NotImplementedException)
			{
			}

			Log.WriteLine("Server stopped");

			if(Stopped != null)
				Stopped(this, null);
		}

		void JoinListener()
		{
			listenerThread.Join();
		}

		void DoListen()
		{
			try
			{

				while(!stop)
				{
					HttpClient client;
					try
					{
						client = new HttpClient(listener.AcceptSocket(), this);
					}
					catch(IOException)
					{
						continue;
					}
					catch(SocketException)
					{
						continue;
					}
					client.Disconnected += new EventHandler(client_Disconnected);
					if(ClientConnected != null)
						ClientConnected(this, new ClientEventArgs(client));
					if(logConnections)
						Log.WriteLine("Connected: " + client.RemoteAddress);
				}
			}
#if !DEBUG
			catch(SocketException e)
			{
				Log.WriteLine("Error: " + e.ToString());
			}
#endif
			finally
			{
				stop = false;
				listener.Stop();
				listener = null;
				isRunning = false;
			}
		}

		#endregion

		#region Client Events

		Timer idleTimer;

		internal event EventHandler OneHertzTick;

		void TimerCallback(object state)
		{
			if(OneHertzTick != null)
				OneHertzTick(this, null);
			
		}

		/// <summary>
		/// Represents an event which occurs when the client's state changes.
		/// </summary>
		public delegate void ClientEventHandler(object sender, ClientEventArgs e);

		/// <summary>
		/// Occurs when a client connects to the server.
		/// </summary>
		public event ClientEventHandler ClientConnected;
		/// <summary>
		/// Occurs when a client is disconnected from the server.
		/// </summary>
		public event ClientEventHandler ClientDisconnected;

		private void client_Disconnected(object sender, EventArgs e)
		{
			HttpClient client = sender as HttpClient;
			if(logConnections)
				Log.WriteLine("Disconnected: " + client.RemoteAddress);
			if(ClientDisconnected != null)
				ClientDisconnected(this, new ClientEventArgs(client));
		}

		/// <summary>
		/// Represents an event which occurs when an HTTP request is received.
		/// </summary>
		public delegate void RequestEventHandler(object sender, RequestEventArgs e);

		/// <summary>
		/// Occurs when any request is received, valid or invalid.
		/// </summary>
		public event RequestEventHandler RequestReceived;
		/// <summary>
		/// Occurs when a valid request to which a response can be made is received.
		/// </summary>
		public event RequestEventHandler ValidRequestReceived;
		/// <summary>
		/// Occurs when an invalid request to which no response other than an error can be made is received.
		/// </summary>
		public event RequestEventHandler InvalidRequestReceived;

		IAuthenticator authenticator;

		/// <summary>
		/// Gets or sets an <see cref="IAuthenticator"/> object responsible for authenticating all requests.
		/// </summary>
		public IAuthenticator Authenticator
		{
			get
			{
				return authenticator;
			}
			set
			{
				authenticator = value;
			}
		}

		internal void OnRequestReceived(HttpClient client, HttpRequest request)
		{
			try
			{
				RequestEventArgs args = new RequestEventArgs(client, request);

				if(RequestReceived != null)
					RequestReceived(this, args);
				if(request.IsValidRequest)
				{
					if(logRequests)
						//BUG: Uri.ToString() decodes a url encoded string for a second time; % disappears
						Log.WriteLine("Request: " + client.RemoteAddress + " " + request.Uri);
					if(ValidRequestReceived != null)
						ValidRequestReceived(this, args);
				}
				else
				{
					if(InvalidRequestReceived != null)
						InvalidRequestReceived(this, args);
				}
			}
			finally
			{
			}
		}

		#endregion

		#region Logging

		TextWriter log = InitializeLog();

		static TextWriter InitializeLog()
		{
			TextWriter log;
			// Initialize the log to output to the console if it is available on the platform, otherwise initialize to null stream writer.
			try
			{
				log = GetConsoleLog();
			}
			catch(MemberAccessException)
			{
				log = TextWriter.Null;
			}
			catch(NotImplementedException)
			{
				log = TextWriter.Null;
			}

			return log;
		}

		static TextWriter GetConsoleLog()
		{
			return Console.Out;
		}

		/// <summary>
		/// Gets or sets the <see cref="TextWriter"/> to which to write logs.
		/// </summary>
		public TextWriter Log
		{
			get
			{
				return log;
			}
			set
			{
				log = value;
			}
		}

		#endregion
	}
}