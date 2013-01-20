using System;
using System.Globalization;
using System.Text;
using System.IO;

namespace MiniHttpd
{
	internal class ChunkedStream : ImmediateResponseStream
	{

		public ChunkedStream(Stream outputStream) : base(outputStream)
		{
		}

		public override void Write(byte[] buffer, int offset, int count)
		{
			byte[] lengthLine = Encoding.UTF8.GetBytes(count.ToString("x", CultureInfo.InvariantCulture) + "\r\n");
			base.outputStream.Write(lengthLine, 0, lengthLine.Length);
			base.Write(buffer, offset, count, false);
			base.outputStream.Write(new byte[] {13, 10}, 0, 2);
			base.outputStream.Flush();
		}
	}
}
