using System;

namespace MiniHttpd
{
	/// <summary>
	/// Represents an object to authenticate a user.
	/// </summary>
	public interface IAuthenticator
	{
		/// <summary>
		/// Authenticates a user agaisnt the proscribed implementation.
		/// </summary>
		/// <param name="username">The username to authenticate.</param>
		/// <param name="password">The password to authenticate.</param>
		/// <returns>True if the user is authenticated, otherwise false.</returns>
		bool Authenticate(string username, string password);
	}
}
