using System;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a resource that has a path that corresponds to a physical file.
	/// </summary>
	public interface IPhysicalResource
	{
		/// <summary>
		/// Gets the full path of the resource on disk.
		/// </summary>
		string Path
		{
			get;
		}
	}
}
