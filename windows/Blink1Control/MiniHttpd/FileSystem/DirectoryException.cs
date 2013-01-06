using System;

namespace MiniHttpd
{
	/// <summary>
	/// The exception that is thrown when a virtual directory already contains an entry of the specified name.
	/// </summary>
	public class DirectoryException : System.IO.IOException
	{
		internal DirectoryException(string message) : base(message)
		{
		}

		internal DirectoryException()
		{
		}
	}
}
