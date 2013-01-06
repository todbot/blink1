using System;
using System.Globalization;

namespace MiniHttpd
{
	/// <summary>
	/// Represents a byte range to be used by the HTTP/1.1 protocol.
	/// </summary>
	public struct ByteRange
	{
		/// <summary>
		/// Creates a new <see cref="ByteRange"/> from the specified string.
		/// </summary>
		/// <param name="parse">A byte-range string given by an HTTP/1.1 header value.</param>
		public ByteRange(string parse)
		{
			if(!parse.StartsWith("bytes="))
				throw new FormatException();

			parse = parse.Substring("bytes=".Length);

			string[] ranges = parse.Split('-');
			if(ranges.Length != 2)
				throw new FormatException();

            string a = ranges[0].Trim();
			string b = ranges[1].Trim();

			bool aNull = false;
			bool bNull = false;

			first = 0;
			last = 0;

			try
			{

				if(a.Length == 0 && b.Length == 0)
					throw new FormatException();
				else if(a.Length != 0 && b.Length == 0)
				{
					this.first = long.Parse(a, NumberStyles.Integer, CultureInfo.InvariantCulture);
					this.last = -1;
					bNull = true;
				}
				else if(a.Length == 0 && b.Length != 0)
				{
					this.first = -1;
					this.last = long.Parse(a, NumberStyles.Integer, CultureInfo.InvariantCulture);
					aNull = true;
				}
				else if(a.Length != 0 && b.Length != 0)
				{
					this.first = long.Parse(a, NumberStyles.Integer, CultureInfo.InvariantCulture);
					this.last = long.Parse(a, NumberStyles.Integer, CultureInfo.InvariantCulture);
				}
			}
			catch
			{
				throw new FormatException();
			}

			if(!aNull && first < 0)
				throw new FormatException();
			if(!bNull && last < 0)
				throw new FormatException();
			if(!aNull && !bNull && first > last)
				throw new FormatException();

		}

		/// <summary>
		/// Creates a new <see cref="ByteRange"/> from the specified values.
		/// </summary>
		/// <param name="first">The index of the first byte.</param>
		/// <param name="last">The index of the last byte.</param>
		public ByteRange(long first, long last)
		{
			this.first = first;
			this.last = last;
		}

		long first;
		long last;

		/// <summary>
		/// Gets the index of the first byte.
		/// </summary>
		public long First
		{
			get
			{
				return first;
			}
		}

		/// <summary>
		/// Gets the index of the last byte.
		/// </summary>
		public long Last
		{
			get
			{
				return last;
			}
		}

	}
}
