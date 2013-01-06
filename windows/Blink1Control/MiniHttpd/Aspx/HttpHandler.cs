using System;
using System.Text;
using System.Text.RegularExpressions;

namespace MiniHttpd.Aspx
{
	/// <summary>
	/// Summary description for HttpHandler.
	/// </summary>
	[Serializable]
	public class HttpHandler : MarshalByRefObject
	{
		internal HttpHandler(string verb, string path, string type, bool validate)
		{
			this.verb = verb;
			this.path = path;
			this.type = type;
			this.validate = validate;

			StringBuilder pattern = new StringBuilder();

			string[] verbs = verb.Split(',');
			foreach(string s in verbs)
			{
				pattern.Append("(");
				pattern.Append(WildcardToRegex(s.Trim()));
				pattern.Append(")|");
			}

			if(pattern.Length != 0)
				pattern.Remove(pattern.Length-1, 1);

			verbsRegex = new Regex(pattern.ToString(), RegexOptions.Compiled | RegexOptions.CultureInvariant);

			pattern = new StringBuilder();

			string[] paths = path.Split(';');
			foreach(string s in paths)
			{
				pattern.Append("(");
				pattern.Append(WildcardToRegex(s.Trim()));
				pattern.Append(")|");
			}

			if(pattern.Length != 0)
				pattern.Remove(pattern.Length-1, 1);

			pathsRegex = new Regex(pattern.ToString(), RegexOptions.Compiled | RegexOptions.CultureInvariant);
		}

		string verb;
		string path;
		string type;
		bool validate;

		Regex verbsRegex;
		Regex pathsRegex;

		/// <summary>
		/// Gets the HTTP method handlers that qualify this handler.
		/// </summary>
		public string Verb
		{
			get
			{
				return verb;
			}
		}

		/// <summary>
		/// Gets the request paths that qualify this handler.
		/// </summary>
		public string Path
		{
			get
			{
				return path;
			}
		}

		/// <summary>
		/// Gets the type of the object to handle the request.
		/// </summary>
		public string Type
		{
			get
			{
				return type;
			}
		}

		/// <summary>
		/// Gets a value indicating whether the handler should be validated beforehand.
		/// </summary>
		public bool Validate
		{
			get
			{
				return validate;
			}
		}

		static string WildcardToRegex(string pattern)
		{
			return Regex.Escape(pattern).
				Replace("\\*", ".*").
				Replace("\\?", ".");
		}

		/// <summary>
		/// Determines if a method/path pair is a match to this handler.
		/// </summary>
		/// <param name="method">The HTTP method of the request.</param>
		/// <param name="path">The path of the request.</param>
		/// <returns>True if the method/path pair is a match to this handler, otherwise false.</returns>
		public bool IsMatch(string method, string path)
		{
			return verbsRegex.IsMatch(method) && pathsRegex.IsMatch(path);
		}

		/// <summary>
		/// Gets a value indicating whether two objects have the same matching conditions.
		/// </summary>
		/// <param name="obj">An <see>HttpHandler</see> object to compare this <see>HttpHandler </see> with.</param>
		/// <returns>True if the two objects have the same matching conditions, otherwise false.</returns>
		public override bool Equals(object obj)
		{
			HttpHandler other = obj as HttpHandler;
			if(other == null)
				return false;

			if(this.verb == other.verb && this.path == other.path)
				return true;
			else
				return false;
		}

		/// <summary>
		/// Returns a hash code generated from the hash codes of the verb and path of this instance.
		/// </summary>
		/// <returns>A hash code value.</returns>
		public override int GetHashCode()
		{
			unchecked
			{
				return unchecked(verb.GetHashCode() * path.GetHashCode());
			}
		}

	}
}
