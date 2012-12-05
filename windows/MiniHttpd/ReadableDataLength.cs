using System;
using System.Globalization;

namespace MiniHttpd
{
	/// <summary>
	/// Provides a method to convert a byte length to a human-readable data length.
	/// </summary>
	public class ReadableDataLength
	{
		private ReadableDataLength()
		{
		}

		/// <summary>
		/// Converts a byte length to a human-readable data length.
		/// </summary>
		/// <param name="bytes">The byte length to convert.</param>
		/// <returns>A byte length in bytes, kb, mb, gb or tb followed by 2 decimal points.</returns>
		public static string Calculate(long bytes)
		{
			if(bytes < 1024*0.96)
				return bytes.ToString(CultureInfo.InvariantCulture) + " bytes";
			if(bytes < 1024*1024*0.96)
				return (bytes/(decimal)1024).ToString("0.00", CultureInfo.InvariantCulture) + " KB";
			if(bytes < 1024*1024*1024*0.96)
				return (bytes/(decimal)(1024*1024)).ToString("0.00", CultureInfo.InvariantCulture) + " MB";
			if(bytes < 1024*1024*1024*(decimal)1024*(decimal)0.5)
				return (bytes/(decimal)(1024*1024*1024)).ToString("0.00", CultureInfo.InvariantCulture) + " GB";

			return (bytes/(decimal)(1024*1024*1024*(decimal)1024)).ToString("0.00", CultureInfo.InvariantCulture) + " TB";
		}
	}
}
