using System;
using System.Text;
using System.DirectoryServices;
using System.Collections.Specialized;

namespace MiniHttpd
{
	/// <summary>
	/// An authenticator that authenticates against an Active Directory server.
	/// </summary>
	[Serializable]
	public class LdapAuthenticator : IAuthenticator
	{
		/// <summary>
		/// Creates a new <see cref="LdapAuthenticator" /> based on the given base path.
		/// </summary>
		/// <param name="basePath">Base path to search users for in Active Directory.</param>
		public LdapAuthenticator(string basePath)
		{
			this.basePath = basePath;
		}

		[NonSerialized]
		NameValueCollection cache = new NameValueCollection();
		[NonSerialized]
		DateTime lastTimeout = DateTime.Now;

		string basePath;

		/// <summary>
		/// Gets the base path to search users for in Active Directory.
		/// </summary>
		public string BasePath
		{
			get
			{
				return basePath;
			}
		}

		[NonSerialized]
		TimeSpan maxCacheAge = new TimeSpan(0, 1, 0, 0, 0);
		
		TimeSpan MaxCacheAge
		{
			get
			{
				return maxCacheAge;
			}
			set
			{
				maxCacheAge = value;
			}
		}
		
		/// <summary>
		/// Resets the login cache.
		/// </summary>
		public void ResetCache()
		{
			cache.Clear();
			lastTimeout = DateTime.Now;
		}

		#region IAuthenticator Members

		/// <summary>
		/// Authenticates against an LDAP server.
		/// </summary>
		/// <param name="username">The username to authenticate.</param>
		/// <param name="password">The password of the given user.</param>
		/// <returns>True if the user is successfully authenticated, otherwise false.</returns>
		public bool Authenticate(string username, string password)
		{
				
			if(lastTimeout.Add(maxCacheAge) < DateTime.Now)
				ResetCache();

			if(string.Compare(cache[username], password, true) == 0)
				return true;

			try
			{
				new DirectoryEntry(basePath, username, password);
				cache.Add(username, password);
			}
			catch
			{
				return false;
			}
			return true;
		}

		#endregion
	}
}
