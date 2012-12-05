using System;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a file, directory or other resource with a name and a parent directory.
	/// </summary>
	/// <remarks>
	/// All objects inheriting this interface should be serializable.
	/// </remarks>
	public interface IResource : IDisposable
	{
		/// <summary>
		/// Gets the name of the entry.
		/// </summary>
		string Name
		{
			get;
		}

		/// <summary>
		/// Gets the parent directory of the object.
		/// </summary>
		IDirectory Parent
		{
			get;
		}
	}
}
