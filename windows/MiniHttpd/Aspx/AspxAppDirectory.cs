using System;
using System.Collections;
using System.Xml;
using System.Web;
using System.Web.Hosting;
using System.Reflection;
using System.IO;
using MiniHttpd;

namespace MiniHttpd.Aspx
{
	/// <summary>
	/// Represents a directory containing an ASPX web application.
	/// </summary>
	[Serializable]
	public class AspxAppDirectory : DriveDirectory
	{
		/// <summary>
		/// Creates a new <see cref="AspxAppDirectory"/> with the specified path and parent.
		/// </summary>
		/// <param name="path">The full path of the web application root.</param>
		/// <param name="parent">The parent directory to which this directory will belong.</param>
		public AspxAppDirectory(string path, IDirectory parent) : base(path, parent)
		{
			virtPath = HttpWebServer.GetDirectoryPath(this);

			CreateAssemblyInBin(path);

			CreateAppHost();

			configFileWatcher.Path = path;
			configFileWatcher.Filter = "Web.config";
			configFileWatcher.Created += new FileSystemEventHandler(configFileWatcher_Changed);
			configFileWatcher.Changed += new FileSystemEventHandler(configFileWatcher_Changed);
			configFileWatcher.Deleted += new FileSystemEventHandler(configFileWatcher_Changed);
			configFileWatcher.Renamed += new RenamedEventHandler(configFileWatcher_Renamed);

			configFileWatcher.EnableRaisingEvents = true;

			LoadWebConfig(System.IO.Path.Combine(path, "Web.config"));
		}

		/// <summary>
		/// Creates a root <see cref="AspxAppDirectory"/> with the specified path.
		/// </summary>
		/// <param name="path">The full path of the directory on disk.</param>
		public AspxAppDirectory(string path) : this (path, null)
		{
		}

		/// <summary>
		/// Shut down app domain and delete bin/minihttpd.dll.
		/// </summary>
		public override void Dispose()
		{
			appHost.Unload();
			if(binFolder != null)
			{
				if(Directory.Exists(binFolder))
				{
					string assemblyPath = System.IO.Path.Combine(
						binFolder,
						System.IO.Path.GetFileName(
							new Uri(Assembly.GetExecutingAssembly().CodeBase).LocalPath));
					if(File.Exists(assemblyPath))
						File.Delete(assemblyPath);

					if(Directory.GetFileSystemEntries(binFolder).Length == 0)
						Directory.Delete(binFolder);
				}
			}
			base.Dispose ();
		}

		string virtPath;
		AspxAppHost appHost;
		[NonSerialized] XmlDocument configFile;
		ArrayList httpHandlers = new ArrayList();

		string binFolder;

		FileSystemWatcher configFileWatcher = new FileSystemWatcher();

		internal string VirtualPath
		{
			get
			{
				return virtPath;
			}
		}

		void CreateAppHost()
		{
			appHost = ApplicationHost.CreateApplicationHost(typeof(AspxAppHost), virtPath, Path) as AspxAppHost;
		}

		internal void ProcessRequest(HttpRequest request, IFile file)
		{
			if(!(file is DriveFile))
				throw new ArgumentException("File must be available on disk.");
			try
			{
				appHost.ProcessRequest(request, file as DriveFile, virtPath, Path);
			}
			catch(AppDomainUnloadedException)
			{
				CreateAppHost();
				ProcessRequest(request, file);
			}
		}

		/// <summary>
		/// Copies the host assembly to the <c>bin</c> folder of the web application if it doesn't exist in the GAC.
		/// The assembly is needed by ASP.NET to access from the web app's domain.
		/// </summary>
		/// <param name="appPath">The full path of the web application directory.</param>
		void CreateAssemblyInBin(string appPath)
		{
			Assembly thisAssembly = Assembly.GetExecutingAssembly();

			if(!thisAssembly.GlobalAssemblyCache)
			{
				string copiedAssemblyPath = null;
				try
				{

					// Create the folder if it doesn't exist, flag it as hidden
					binFolder = System.IO.Path.Combine(appPath, "bin");
					if(!Directory.Exists(binFolder))
					{
						Directory.CreateDirectory(binFolder);
						File.SetAttributes(binFolder, FileAttributes.Hidden);
					}

					//TODO: implement httphandlers, lock httpHandlers

					// Delete the file if it exists, copy to bin
					string assemblyPath = new Uri(thisAssembly.CodeBase).LocalPath;
					copiedAssemblyPath = System.IO.Path.Combine(binFolder, System.IO.Path.GetFileName(assemblyPath));
					if(File.Exists(copiedAssemblyPath))
						File.Delete(copiedAssemblyPath);
					File.Copy(assemblyPath, copiedAssemblyPath);

				}
				catch(IOException)
				{
					if(!File.Exists(copiedAssemblyPath))
						throw;

					if(thisAssembly.FullName != AssemblyName.GetAssemblyName(copiedAssemblyPath).FullName)
						throw;
				}
			}
		}

		void LoadWebConfig(string path)
		{
			try
			{
				httpHandlers.Clear();
				configFile = new XmlDocument();
				configFile.Load(path);

				XmlNode handlersNode = configFile.DocumentElement.SelectSingleNode("/configuration/system.web/httpHandlers");
				if(handlersNode == null)
					return;
				
				lock(httpHandlers)
				{
					foreach(XmlNode node in handlersNode)
					{
						switch(node.Name)
						{
							case "add":
							{
								if(node.Attributes["verb"] == null)
									break;
								if(node.Attributes["path"] == null)
									break;
								if(node.Attributes["type"] == null)
									break;

								bool validate = false;
								
								try
								{
									if(node.Attributes["validate"] != null)
										validate = bool.Parse(node.Attributes["validate"].Value);
								}
								catch(FormatException)
								{
									validate = false;
								}

								HttpHandler handler = new HttpHandler(node.Attributes["verb"].Value, node.Attributes["path"].Value, node.Attributes["type"].Value, validate);
								httpHandlers.Remove(handler);
								httpHandlers.Add(handler);

								break;
							}
							case "remove":
							{
								if(node.Attributes["verb"] == null)
									break;
								if(node.Attributes["path"] == null)
									break;

								HttpHandler handler = new HttpHandler(node.Attributes["verb"].Value, node.Attributes["path"].Value, null, false);
								httpHandlers.Remove(handler);

								break;
							}
							case "clear":
							{
								httpHandlers.Clear();

								break;
							}
						}
					}
				}
			}
			catch(Exception)
			{
				httpHandlers.Clear();
				configFile = null;
			}
		}

		private void configFileWatcher_Changed(object sender, FileSystemEventArgs e)
		{
			LoadWebConfig(e.FullPath);
		}

		private void configFileWatcher_Renamed(object sender, RenamedEventArgs e)
		{
			LoadWebConfig(e.FullPath);
		}
	}
}
