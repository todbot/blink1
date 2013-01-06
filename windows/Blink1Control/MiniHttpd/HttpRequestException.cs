using System;

namespace MiniHttpd
{
	/// <summary>
	/// Excpetion raised to stop processing a request and return an error to the client.
	/// </summary>
	/// <remarks>
	/// This exception is always handled by HttpClient. If you catch this exception earlier for any reason, you must rethrow it for the response to be sent.
	/// </remarks>
	public class HttpRequestException : Exception
	{
		/// <summary>
		/// Creates a new <see cref="HttpRequestException"/> with the given response code.
		/// </summary>
		/// <param name="responseCode">A response code to return to the client.</param>
		public HttpRequestException(string responseCode) : this(responseCode, StatusCodes.GetDescription(responseCode))
		{
		}

		/// <summary>
		/// Creates a new <see cref="HttpRequestException"/> with the given response code and message.
		/// </summary>
		/// <param name="responseCode">A response code to return to the client.</param>
		/// <param name="message">An accompanying message to return to the client.</param>
		public HttpRequestException(string responseCode, string message) : this(responseCode, message, false)
		{
		}

		/// <summary>
		/// Creates a new <see cref="HttpRequestException"/> with the given response code and disconnect behavior.
		/// </summary>
		/// <param name="responseCode">A response code to return to the client.</param>
		/// <param name="disconnect">A value indicating whether to disconnect the client immediately after sending the response.</param>
		public HttpRequestException(string responseCode, bool disconnect) : this(responseCode, StatusCodes.GetDescription(responseCode), disconnect)
		{
		}
		
		/// <summary>
		/// Creates a new <see cref="HttpRequestException"/> with the given response code, message and disconnect behavior.
		/// </summary>
		/// <param name="responseCode">A response code to return to the client.</param>
		/// <param name="message">An accompanying message to return to the client.</param>
		/// <param name="disconnect">A value indicating whether to disconnect the client immediately after sending the response.</param>
		public HttpRequestException(string responseCode, string message, bool disconnect) : base(message)
		{
			this.responseCode = responseCode;
			this.disconnect = disconnect;
		}

		string responseCode;
		bool disconnect;

		/// <summary>
		/// Gets the HTTP response code of the error.
		/// </summary>
		public string ResponseCode
		{
			get
			{
				return responseCode;
			}
		}

		/// <summary>
		/// Gets a value indicating whether or not the client should be disconnected immediately.
		/// </summary>
		public bool Disconnect
		{
			get
			{
				return disconnect;
			}
		}
	}
}
