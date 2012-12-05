using System;
using System.IO;

namespace MiniHttpd
{
	internal class ImmediateResponseStream : Stream
	{
		internal ImmediateResponseStream(Stream outputStream)
		{
			this.outputStream = outputStream;
		}

		protected Stream outputStream;

		long bytesSent;

		public long BytesSent
		{
			get
			{
				return bytesSent;
			}
		}

		public override bool CanRead
		{
			get
			{
				return false;
			}
		}

		public override bool CanWrite
		{
			get
			{
				return true;
			}
		}

		public override bool CanSeek
		{
			get
			{
				return false;
			}
		}

		public override void Flush()
		{

		}

		public override long Length
		{
			get
			{
				throw new NotSupportedException();
			}
		}

		public override long Position
		{
			get
			{
				throw new NotSupportedException();
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		public override int Read(byte[] buffer, int offset, int count)
		{
			throw new NotSupportedException();
		}

		public override long Seek(long offset, SeekOrigin origin)
		{
			throw new NotSupportedException();
		}

		public override void SetLength(long value)
		{
			throw new NotSupportedException();
		}

		public override void Write(byte[] buffer, int offset, int count)
		{
			Write(buffer, offset, count, true);
		}

		public void Write(byte[] buffer, int offset, int count, bool flush)
		{
			outputStream.Write(buffer, offset, count);
			bytesSent += count;
		}
	}
}
