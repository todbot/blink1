using System;
using System.IO;
using System.Collections;
using System.Runtime.Serialization;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a directory which can be populated with subdirectories and files programmatically.
	/// </summary>
	[Serializable]
	public class VirtualDirectory : IDirectory
	{
		/// <summary>
		/// Creates a new virtual directory.
		/// </summary>
		/// <param name="name">The name of the directory as it will be seen in the directory.</param>
		/// <param name="parent">An <see cref="IDirectory" /> specifying the parent directory. This value should be <c>null</c> if this directory is to be the root directory.</param>
		public VirtualDirectory(string name, IDirectory parent)
		{
			this.name = name;
			this.parent = parent;
			directories = new Hashtable();
			files = new Hashtable();
		}

		/// <summary>
		/// Creates a new virtual directory to use as the root directory.
		/// </summary>
		public VirtualDirectory() : this("/", null)
		{
		}

		Hashtable directories;
		Hashtable files;
		string name;
		IDirectory parent;

		void CheckExistence(string name)
		{
			if(directories.ContainsKey(name))
				throw new DirectoryException("A directory of the name \"" + name + "\" already exists");
			if(files.ContainsKey(name))
				throw new DirectoryException("A file of the name \"" + name + "\" already exists");

		}

		/// <summary>
		/// Adds a subdirectory to the directory.
		/// </summary>
		/// <param name="directory">An <see cref="IDirectory" /> specifying the directory to add. The directory's parent must be the directory to which the directory is added.</param>
		public void AddDirectory(IDirectory directory)
		{
			CheckExistence(directory.Name);

			if(directory.Parent != this)
				throw new DirectoryException("The directory's parent must be the directory to which it is added");

			directories.Add(directory.Name, directory);
		}

		/// <summary>
		/// Adds a physical subdirectory to the directory.
		/// </summary>
		/// <param name="path">The full path of the directory to add.</param>
		/// <returns>The newly created <see cref="DriveDirectory"/>.</returns>
		public DriveDirectory AddDirectory(string path)
		{
			DriveDirectory directory = new DriveDirectory(path, this);
			CheckExistence(directory.Name);

			directories.Add(directory.Name, directory);

			return directory;
		}

		/// <summary>
		/// Adds a physical subdirectory to the directory with a specific name.
		/// </summary>
		/// <param name="alias">The name of the subdirectory to add as it will be seen in the directory.</param>
		/// <param name="path">The full path of the directory to add.</param>
		/// <returns>The newly created <see cref="DriveDirectory"/>.</returns>
		public DriveDirectory AddDirectory(string alias, string path)
		{
			DriveDirectory directory = new DriveDirectory(alias, path, this);
			CheckExistence(directory.Name);

			directories.Add(directory.Name, directory);

			return directory;
		}

		/// <summary>
		/// Adds a physical file to the directory.
		/// </summary>
		/// <param name="path">The full path of the file to add.</param>
		/// <returns>The newly created <see cref="DriveFile"/>.</returns>
		public DriveFile AddFile(string path)
		{
			DriveFile file = new DriveFile(path, this);
			AddFile(file);
			return file;
		}

		/// <summary>
		/// Adds a file to the directory.
		/// </summary>
		/// <param name="file">An <see cref="IFile"/> representing the file to add. The file's parent must be the directory to which the file is added.</param>
		public void AddFile(IFile file)
		{
			CheckExistence(file.Name);

			if(file.Parent != this)
				throw new DirectoryException("The file's parent must be the directory to which it is added.");

			files.Add(file.Name, file);
		}

		/// <summary>
		/// Removes a file or subdirectory from the directory.
		/// </summary>
		/// <param name="name">The name of the file or subdirectory to remove.</param>
		public void Remove(string name)
		{
			if(files.ContainsKey(name))
			{
				(files[name] as IFile).Dispose();
				files.Remove(name);
			}
			else if(directories.Contains(name))
			{
				(directories[name] as IDirectory).Dispose();
				directories.Remove(name);
			}
		}

		#region IDirectory Members

		/// <summary>
		/// Returns the specified subdirectory.
		/// </summary>
		/// <param name="dir">The name of the subdirectory to retrieve.</param>
		/// <returns>An <see cref="IDirectory"/> representing the specified directory, or <c>null</c> if one doesn't exist.</returns>
		public IDirectory GetDirectory(string dir)
		{
			if(dir == "/")
				return this;
			return directories[dir] as IDirectory;
		}

		/// <summary>
		/// Returns the specified file.
		/// </summary>
		/// <param name="filename">The name of the file to retrieve.</param>
		/// <returns>An <see cref="IFile"/> representing the specified file, or <c>null</c> if one doesn't exist.</returns>
		public IFile GetFile(string filename)
		{
			return files[filename] as IFile;
		}

		/// <summary>
		/// Returns a collection of subdirectories available in the directory.
		/// </summary>
		/// <returns>An <see cref="ICollection"/> containing <see cref="IDirectory"/> objects available in the directory.</returns>
		public ICollection GetDirectories()
		{
			return directories.Values;
		}

		/// <summary>
		/// Returns a collection of files available in the directory.
		/// </summary>
		/// <returns>An <see cref="ICollection"/> containing <see cref="IFile"/> objects available in the directory.</returns>
		public ICollection GetFiles()
		{
			return files.Values;
		}

		/// <summary>
		/// Gets the name of the directory.
		/// </summary>
		public string Name
		{
			get
			{
				return name;
			}
		}

		/// <summary>
		/// Gets the parent directory of the directory.
		/// </summary>
		public IDirectory Parent
		{
			get
			{
				return parent;
			}
		}

		/// <summary>
		/// Returns the resource with the given name.
		/// </summary>
		/// <param name="name">The name of the resource.</param>
		/// <returns>A resource with the given name or <c>null</c> if one isn't available.</returns>
		public IResource GetResource(string name)
		{
			IFile file = GetFile(name);
			if(file != null)
				return file;
			return GetDirectory(name);
		}

		#endregion

		#region IDisposable Members

		/// <summary>
		/// Dispose all containing files and directories.
		/// </summary>
		public virtual void Dispose()
		{
			foreach(IFile file in files.Values)
				file.Dispose();

			foreach(IDirectory dir in directories.Values)
				dir.Dispose();

			files.Clear();
			directories.Clear();
		}

		#endregion

		/// <summary>
		/// Returns the name of the directory.
		/// </summary>
		/// <returns>The name of the directory.</returns>
		public override string ToString()
		{
			return name;
		}
	}
}