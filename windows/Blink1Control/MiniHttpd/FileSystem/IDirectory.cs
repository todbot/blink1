using System;
using System.Collections;
using System.IO;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a directory which contains files and subdirectories.
	/// </summary>
	/// <remarks>
	/// Root directories should have a name of <c>null</c>.
	/// </remarks>
	public interface IDirectory : IResource
	{
		/// <summary>
		/// Returns the specified subdirectory.
		/// </summary>
		/// <param name="dir">The name of the subdirectory to retrieve.</param>
		/// <returns>An <see cref="IDirectory"/> representing the specified directory, or <c>null</c> if one doesn't exist.</returns>
		IDirectory GetDirectory(string dir);

		/// <summary>
		/// Returns the specified file.
		/// </summary>
		/// <param name="filename">The name of the file to retrieve.</param>
		/// <returns>An <see cref="IFile"/> representing the specified file, or <c>null</c> if one doesn't exist.</returns>
		IFile GetFile(string filename);

		/// <summary>
		/// Returns a collection of subdirectories available in the directory.
		/// </summary>
		/// <returns>An <see cref="ICollection"/> containing <see cref="IDirectory"/> objects available in the directory.</returns>
		ICollection GetDirectories();

		/// <summary>
		/// Returns a collection of files available in the directory.
		/// </summary>
		/// <returns>An <see cref="ICollection"/> containing <see cref="IFile"/> objects available in the directory.</returns>
		ICollection GetFiles();

		/// <summary>
		/// Returns the resource (file or directory) with the specified name.
		/// </summary>
		/// <param name="name">The name of the resource.</param>
		/// <returns>An IFile or IDirectory with the given name.</returns>
		IResource GetResource(string name);
	}
}
