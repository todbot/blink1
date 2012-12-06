using System.Collections;

namespace MiniHttpd
{
	/// <summary>
	/// Provides a collection of response codes and their corresponding English descriptions.
	/// </summary>
	public class StatusCodes
	{
		private StatusCodes()
		{
		}

		static Hashtable InitDescriptions()
		{
			Hashtable descriptions = new Hashtable();
			descriptions.Add("100", "Continue");
			descriptions.Add("101", "Switching Protocols");
			descriptions.Add("200", "OK");
			descriptions.Add("201", "Created");
			descriptions.Add("202", "Accepted");
			descriptions.Add("203", "Non-Authoritative Information");
			descriptions.Add("204", "No Content");
			descriptions.Add("205", "Reset Content");
			descriptions.Add("206", "Partial Content");
			descriptions.Add("300", "Multiple Choices");
			descriptions.Add("301", "Moved Permanently");
			descriptions.Add("302", "Found");
			descriptions.Add("303", "See Other");
			descriptions.Add("304", "Not Modified");
			descriptions.Add("305", "Use Proxy");
			descriptions.Add("307", "Temporary Redirect");
			descriptions.Add("400", "Bad Request");
			descriptions.Add("401", "Unauthorized");
			descriptions.Add("402", "Payment Required");
			descriptions.Add("403", "Forbidden");
			descriptions.Add("404", "Not Found");
			descriptions.Add("405", "Method Not Allowed");
			descriptions.Add("406", "Not Acceptable");
			descriptions.Add("407", "Proxy Authentication Required");
			descriptions.Add("408", "Request Time-out");
			descriptions.Add("409", "Conflict");
			descriptions.Add("410", "Gone");
			descriptions.Add("411", "Length Required");
			descriptions.Add("412", "Precondition Failed");
			descriptions.Add("413", "Request Entity Too Large");
			descriptions.Add("414", "Request-URI Too Large");
			descriptions.Add("415", "Unsupported Media Type");
			descriptions.Add("416", "Requested range not satisfiable");
			descriptions.Add("417", "Expectation Failed");
			descriptions.Add("500", "Internal Server Error");
			descriptions.Add("501", "Not Implemented");
			descriptions.Add("502", "Bad Gateway");
			descriptions.Add("503", "Service Unavailable");
			descriptions.Add("504", "Gateway Time-out");
			descriptions.Add("505", "HTTP Version not supported");
			return descriptions;
		}

		static Hashtable descriptions = InitDescriptions();

		/// <summary>
		/// Retrieves an English description of the given response code.
		/// </summary>
		/// <param name="codeNum">An HTTP Response code.</param>
		/// <returns>An English description of the given response code, otherise <c>null</c> if the response code is invalid.</returns>
		static public string GetDescription(string codeNum)
		{
			return descriptions[codeNum] as string;
		}
	}
}

//  "100"  ; Section 10.1.1:  Continue
//| "101"  ; Section 10.1.2:  Switching Protocols
//| "200"  ; Section 10.2.1:  OK
//| "201"  ; Section 10.2.2:  Created
//| "202"  ; Section 10.2.3:  Accepted
//| "203"  ; Section 10.2.4:  Non-Authoritative Information
//| "204"  ; Section 10.2.5:  No Content
//| "205"  ; Section 10.2.6:  Reset Content
//| "206"  ; Section 10.2.7:  Partial Content
//| "300"  ; Section 10.3.1:  Multiple Choices
//| "301"  ; Section 10.3.2:  Moved Permanently
//| "302"  ; Section 10.3.3:  Found
//| "303"  ; Section 10.3.4:  See Other
//| "304"  ; Section 10.3.5:  Not Modified
//| "305"  ; Section 10.3.6:  Use Proxy
//| "307"  ; Section 10.3.8:  Temporary Redirect
//| "400"  ; Section 10.4.1:  Bad Request
//| "401"  ; Section 10.4.2:  Unauthorized
//| "402"  ; Section 10.4.3:  Payment Required
//| "403"  ; Section 10.4.4:  Forbidden
//| "404"  ; Section 10.4.5:  Not Found
//| "405"  ; Section 10.4.6:  Method Not Allowed
//| "406"  ; Section 10.4.7:  Not Acceptable
//| "407"  ; Section 10.4.8:  Proxy Authentication Required
//| "408"  ; Section 10.4.9:  Request Time-out
//| "409"  ; Section 10.4.10: Conflict
//| "410"  ; Section 10.4.11: Gone
//| "411"  ; Section 10.4.12: Length Required
//| "412"  ; Section 10.4.13: Precondition Failed
//| "413"  ; Section 10.4.14: Request Entity Too Large
//| "414"  ; Section 10.4.15: Request-URI Too Large
//| "415"  ; Section 10.4.16: Unsupported Media Type
//| "416"  ; Section 10.4.17: Requested range not satisfiable
//| "417"  ; Section 10.4.18: Expectation Failed
//| "500"  ; Section 10.5.1:  Internal Server Error
//| "501"  ; Section 10.5.2:  Not Implemented
//| "502"  ; Section 10.5.3:  Bad Gateway
//| "503"  ; Section 10.5.4:  Service Unavailable
//| "504"  ; Section 10.5.5:  Gateway Time-out
//| "505"  ; Section 10.5.6:  HTTP Version not supported