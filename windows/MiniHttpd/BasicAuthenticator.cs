using System;
using System.Text;
using System.Security.Cryptography;
using System.Collections;
using System.Runtime.Serialization;

namespace MiniHttpd
{
	/// <summary>
	/// A basic authenticator using MD5 hashes.
	/// </summary>
	[Serializable]
	public class BasicAuthenticator : IAuthenticator
	{
		/// <summary>
		/// Add a user to the user list.
		/// </summary>
		/// <param name="username">The user name to add.</param>
		/// <param name="password">The password to associate with the user.</param>
		public void AddUser(string username, string password)
		{
			entries.Add(username, md5.ComputeHash(encoding.GetBytes(password)));
		}

		/// <summary>
		/// Add a user to the user list with the given MD5.
		/// </summary>
		/// <param name="username">The name of the user to add.</param>
		/// <param name="hash">The hash value of the user's password.</param>
		public void AddUserByHash(string username, byte[] hash)
		{
			entries.Add(username, hash);
		}

		/// <summary>
		/// Add a user to the user list with the given MD5.
		/// </summary>
		/// <param name="username">The name of the user to add.</param>
		/// <param name="hash">The hash value of the user's password.</param>
		public void AddUserByHash(string username, string hash)
		{
			entries.Add(username, HexToBytes(hash));
		}

		/// <summary>
		/// Remove a user from the user list.
		/// </summary>
		/// <param name="username">The name of the user to remove.</param>
		public void RemoveUser(string username)
		{
			entries.Remove(username);
		}

		/// <summary>
		/// Change a user's password.
		/// </summary>
		/// <param name="username">The name of the user whose name is to be changed.</param>
		/// <param name="current">The user's current password.</param>
		/// <param name="newPassword">The user's new password.</param>
		/// <exception>System.Security.SecurityException</exception>
		public void ChangePassword(string username, string current, string newPassword)
		{
			if(!Authenticate(username, current))
				throw new System.Security.SecurityException("User or password is incorrect.");

			entries[username] = md5.ComputeHash(encoding.GetBytes(newPassword));
		}

		/// <summary>
		/// Gets a collection of string containing the names of the users in the user list.
		/// </summary>
		public ICollection Users
		{
			get
			{
				return entries.Keys;
			}
		}

		/// <summary>
		/// Retrieves the password hash of a given user.
		/// </summary>
		/// <param name="username">The name of the user whose password is to be returned.</param>
		/// <returns>The MD5 hash of the password in hex format.</returns>
		public string GetPasswordHash(string username)
		{
			return BytesToHex(entries[username] as byte[]);
		}

		string BytesToHex(byte[] bytes)
		{
			System.Text.StringBuilder sb = new StringBuilder(bytes.Length*2);
			foreach(byte b in bytes)
				sb.Append(b.ToString("X2"));
			
			return sb.ToString();
		}

		byte[] HexToBytes(string hex)
		{
			if(hex.Length % 2 != 0)
				throw new ArgumentException("String must have an even length", "hex");

            byte[] bytes = new byte[hex.Length/2];
			for(int i = 0; i < bytes.Length; i++)
			{
				byte b = (byte)(GetNibble(hex[i*2]) << 4);
				b &= GetNibble(hex[i*2+1]);
			}

			return bytes;
		}

		byte GetNibble(char b)
		{
			if(b >= '0' && b <= '9')
				return (byte)(b - '0');
			else if(b >= 'A' && b <= 'F')
				return (byte)(b - 'A');
			else if(b >= 'a' && b <= 'f')
				return (byte)(b - 'a');
			else
				throw new FormatException();
		}

		/// <summary>
		/// Gets a value indicating whether the given user already exists in the list.
		/// </summary>
		/// <param name="username">The name of the user to check.</param>
		/// <returns>True if the user already exists in the list, otherwise false.</returns>
		public bool Exists(string username)
		{
			return entries[username] != null;
		}

		Hashtable entries = new Hashtable();

		[NonSerialized]
		Encoding encoding = new UTF8Encoding(false, false);

		[NonSerialized]
		MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();

		static bool BytesEquals(byte[] array1, byte[] array2)
		{
			if(array1.Length != array2.Length)
				return false;

			for(int i = 0; i < array1.Length; i++)
				if(array1[i] != array2[i])
					return false;

			return true;
		}

		#region IAuthenticator Members

		/// <summary>
		/// Authenticate the user against the user list.
		/// </summary>
		/// <param name="username">The name of the user to authenticate.</param>
		/// <param name="password">The user's password.</param>
		/// <returns>True if the user is successfully authenticated, otherwise false.</returns>
		public bool Authenticate(string username, string password)
		{
			if(username == null)
				return false;
			if(password == null)
				return false;

			if(!entries.ContainsKey(username))
				return false;

			if(BytesEquals(entries[username] as byte[], md5.ComputeHash(encoding.GetBytes(password))))
				return true;
			else
				return false;
		}

		#endregion
	}
}
