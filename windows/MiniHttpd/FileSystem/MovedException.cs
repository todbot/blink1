using System;

namespace MiniHttpd
{
	/// <summary>
	/// Exception thrown when a request must be redirected.
	/// </summary>
	public class MovedException : HttpRequestException
	{
		/// <summary>
		/// Creates a new <see cref="MovedException"/> object.
		/// </summary>
		/// <param name="newPath">The new URL to redirect to.</param>
		public MovedException(string newPath) : base("302")
		{
			this.newPath = newPath;
		}

		string newPath;
		
		/// <summary>
		/// Gets the new URL to redirect to.
		/// </summary>
		public string NewPath
		{
			get
			{
				return newPath;
			}
		}
	}
}
