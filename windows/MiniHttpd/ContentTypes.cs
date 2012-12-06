using System;
using System.Collections;
using System.Globalization;
using Microsoft.Win32;

namespace MiniHttpd
{
	/// <summary>
	/// Provides a reference of common MIME content-types, and retrieves additional types from the Windows registry if available.
	/// </summary>
	public sealed class ContentTypes
	{
		private ContentTypes()
		{
		}

		static Hashtable InitContentTypes()
		{
			Hashtable extensionTypes = new Hashtable(
				new CaseInsensitiveHashCodeProvider(CultureInfo.InvariantCulture),
				new CaseInsensitiveComparer(CultureInfo.InvariantCulture)
				);
			#region Extensions from http://www.utoronto.ca/webdocs/HTMLdocs/Book/Book-3ed/appb/mimetype.html
			extensionTypes.Add(".bin", "application/octet-stream");
			extensionTypes.Add(".uu", "application/octet-stream");
			extensionTypes.Add(".exe", "application/octet-stream");
			extensionTypes.Add(".ai", "application/postscript");
			extensionTypes.Add(".eps", "application/postscript");
			extensionTypes.Add(".ps", "application/postscript");
			extensionTypes.Add(".latex", "application/x-latex");
			extensionTypes.Add(".ram", "application/x-pn-realaudio");
			extensionTypes.Add(".swf", "application/x-shockwave-flash");
			extensionTypes.Add(".tar", "application/x-tar");
			extensionTypes.Add(".tcl", "application/x-tcl");
			extensionTypes.Add(".tex", "application/x-tex");
			extensionTypes.Add(".zip", "application/zip");
			extensionTypes.Add(".rar", "application/rar");
			extensionTypes.Add(".au", "audio/basic");
			extensionTypes.Add(".mpa", "audio/x-mpeg");
			extensionTypes.Add(".abs", "audio/x-mpeg");
			extensionTypes.Add(".mpega", "audio/x-mpeg");
			extensionTypes.Add(".mp2a", "audio/x-mpeg-2");
			extensionTypes.Add(".mpa2", "audio/x-mpeg-2");
			extensionTypes.Add(".wma", "audio/x-ms-wma");
			extensionTypes.Add(".wav", "audio/x-wav");
			extensionTypes.Add(".jpeg", "image/jpeg");
			extensionTypes.Add(".jpg", "image/jpeg");
			extensionTypes.Add(".jpe", "image/jpeg");
			extensionTypes.Add(".tiff", "image/tiff");
			extensionTypes.Add(".tif", "image/tiff");
			extensionTypes.Add(".bmp", "image/x-ms-bmp");
			extensionTypes.Add(".png", "image/x-png");
			extensionTypes.Add(".pnm", "image/x-portable-anymap");
			extensionTypes.Add(".pbm", "image/x-portable-bitmap");
			extensionTypes.Add(".pgm", "image/x-portable-graymap");
			extensionTypes.Add(".ppm", "image/x-portable-pixmap");
			extensionTypes.Add(".xbm", "image/x-xbitmap");
			extensionTypes.Add(".xpm", "image/x-xpixmap");
			extensionTypes.Add(".xwd", "image/x-xwindowdump");
			extensionTypes.Add(".css", "text/css");
			extensionTypes.Add(".html", "text/html");
			extensionTypes.Add(".htm", "text/html");
			extensionTypes.Add(".js", "text/javascript");
			extensionTypes.Add(".ls", "text/javascript");
			extensionTypes.Add(".mocha", "text/javascript");
			extensionTypes.Add(".txt", "text/plain");
			extensionTypes.Add(".bat", "text/plain");
			extensionTypes.Add(".c", "text/plain");
			extensionTypes.Add(".cpp", "text/plain");
			extensionTypes.Add(".c++", "text/plain");
			extensionTypes.Add(".cc", "text/plain");
			extensionTypes.Add(".h", "text/plain");
			extensionTypes.Add(".log", "text/plain");
			extensionTypes.Add(".cs", "text/plain");
			extensionTypes.Add(".vb", "text/plain");
			extensionTypes.Add(".mpeg", "video/mpeg");
			extensionTypes.Add(".mpg", "video/mpeg");
			extensionTypes.Add(".mpe", "video/mpeg");
			extensionTypes.Add(".mpv2", "video/mpeg-2");
			extensionTypes.Add(".mp2v", "video/mpeg-2");
			extensionTypes.Add(".qt", "video/quicktime");
			extensionTypes.Add(".mov", "video/quicktime");
			extensionTypes.Add(".asf", "video/x-ms-asf");
			extensionTypes.Add(".asx", "video/x-ms-asx");
			extensionTypes.Add(".wmv", "video/x-ms-wmv");
			extensionTypes.Add(".avi", "video/x-msvideo");
			#endregion
			return extensionTypes;
		}

		static Hashtable extensionTypes = InitContentTypes();

		/// <summary>
		/// Get a MIME content-type from a file extension.
		/// </summary>
		/// <param name="extension">A file extension.</param>
		/// <returns>A MIME compatible file file-type.</returns>
		public static string GetExtensionType(string extension)
		{
			string ret = extensionTypes[extension] as string;
			if(ret == null)
			{
				try
				{
					ret = GetContentTypeFromRegistry(extension);
				}
				catch(MemberAccessException)
				{
				}
				catch(NotImplementedException)
				{
				}
				finally
				{
					if(ret != null)
						extensionTypes[extension] = ret;
				}
			}
			return ret;
		}

		static string GetContentTypeFromRegistry(string extension)
		{
			RegistryKey classroot = Registry.ClassesRoot;
			RegistryKey extkey = classroot.OpenSubKey(extension, false);
			if(extkey == null)
				return null;
			object type = extkey.GetValue("Content Type");
			if(!(type is string))
				return null;
			return type as string;
		}
	}
}