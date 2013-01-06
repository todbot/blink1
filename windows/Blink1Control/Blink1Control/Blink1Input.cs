using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using System.Runtime.Serialization;
using System.Text.RegularExpressions;
using System.Net;
using System.IO;
using Newtonsoft.Json;

namespace Blink1Control
{
//    [Serializable]
    public class Blink1Input
    {

        public Blink1Server blink1Server { private get; set; }
        /// <summary>
        /// Name of the input 
        /// </summary>
        public string iname { get; set; }
        /// <summary>
        /// name of the color pattern to play if this input is triggered
        /// </summary>
        public string pname { get; set; }
        /// <summary>
        ///  type of input, can be: "ifttt", "url", "file", "script"
        /// </summary>
        public string type { get; set; }
        public string arg1 { get; set; }
        public string arg2 { get; set; }
        public string arg3 { get; set; }
        public string lastVal { get; set; }
        public string possibleVals { get; set; }

        // each input can have an independent update interval that's greater than master interval
        public DateTime lastTime;
        //private int updateInterval;
        // holder of last valid response/content
        private string lastContent;

        // FIXME: hacks
        public static DateTime iftttLastTime;
        public static int iftttUpdateInterval;
        public static string iftttLastContent;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="type"></param>
        /// <param name="arg1"></param>
        /// <param name="arg2"></param>
        /// <param name="arg3"></param>
        public Blink1Input(string name, string type, string arg1, string arg2, string arg3)
        {
            this.iname = name; this.type = type;
            this.arg1 = arg1; this.arg2 = arg2; this.arg3 = arg3;
        }

        /// <summary>
        /// 
        /// </summary>
        public void stop()
        {
        }

        /// <summary>
        /// Called periodically by blink1Server
        /// </summary>
        public void update()
        {
            if (type.Equals("ifttt")) { // FIXME: there's totally a better way of doing this
                updateIftttInput();
            }
            else if (type.Equals("url")) {
                updateUrlInput();
            }
            else if (type.Equals("file")) {
                updateFileInput();
            }
            else if (type.Equals("script")) {
                updateScriptInput();
            }

        }

        // periodically fetch events from IFTTT gateway
        public void updateIftttInput()
        {
            if (iftttLastContent==null) return;
            
            // add interval checking

            string rulename = arg1;

            IftttResponse iftttResponse = JsonConvert.DeserializeObject<IftttResponse>( iftttLastContent );
            if (iftttResponse.event_count > 0) {
                foreach (IftttEvent ev in iftttResponse.events) {
                    possibleVals += ev.name;
                    lastVal = ev.source;
                }
            }
        }

        /// <summary>
        /// Periodically fetch URL, looking for color patern name or color code 
        /// </summary>
        public void updateUrlInput()
        {
            string url = arg1;
            string resp = getContentsOfUrl(url);
            lastContent = resp;
            if (resp == null) {
                lastVal = "bad url";
                return;
            }
            string patternstr = parsePatternOrColorString(resp);

            if (patternstr != null && !patternstr.Equals(lastVal)) {
                lastVal = patternstr;
                blink1Server.playPattern(patternstr);
            }
        }

        /// <summary>
        /// Periodically check file, looking for color pattern name or color code
        ///
        /// </summary>
        public void updateFileInput()
        {
            string filepath = arg1;
            string resp = getContentsOfFile(filepath);
            lastContent = resp;
            if (resp == null) {
                lastVal = "bad filename";
                return;
            }
            string patternstr = parsePatternOrColorString(resp);
            if (patternstr != null && !patternstr.Equals(lastVal)) {
                lastVal = patternstr;
                blink1Server.playPattern(patternstr);
            }
        }

        /// <summary>
        /// Periodically excecute script, looking for color pattern name or color cod
        /// </summary>
        public void updateScriptInput()
        {
            string scriptname = arg1;
        }

        // ---------------------------------------------------------------------------------------
        // static utility methods 


        /// <summary>
        /// This is called peridically, but only once for all IFTTT inputs. 
        /// FIXE: This whole method is a super hack
        /// </summary>
        /// <param name="input"></param>
        /// <param name="normalmode"></param>
        public static void getIftttResponse(Boolean normalmode)
        {
            // only update URLs every 30 secs
            DateTime now = DateTime.Now; ; //[[NSDate date] timeIntervalSince1970];
            TimeSpan diff = now - iftttLastTime;            
            if( normalmode && (diff.Seconds < iftttUpdateInterval) ) { 
                return;
            }
            iftttLastTime = now;

            string eventUrl = Blink1Server.iftttEventUrl +"/"+ Blink1Server.blink1Id;  // FIXME: hack
            iftttLastContent = getContentsOfUrl(eventUrl);
        }

        /// <summary>
        /// Look for a color pattern designator or a HTML color hex code in a string
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static string parsePatternOrColorString(string str)
        {
            // read color pattern, can be of form: pattern: "my pattern"
            Regex patternregex = new Regex(@"\""*pattern\""*:\s*\""(.+?)\""");
            Match matchp = patternregex.Match(str);
            if (matchp.Success) {
                return matchp.Groups[1].Value;
            }
            //
            Regex hexcoderegex = new Regex(@"(#[A-Fa-f0-9]{6})");
            Match matchc = hexcoderegex.Match(str.ToUpper());
            if (matchc.Success) {
                return matchc.Groups[1].Value;
            }

            //Color c = ColorTranslator.FromHtml(str);
            return null;
        }

        /// <summary>
        /// Get the contents of a URL into string
        /// </summary>
        /// <param name="url">the url to fetch</param>
        /// <returns>contents of url or null</returns>
        public static string getContentsOfUrl(string url)
        {
            // stolen from: http://stackoverflow.com/questions/9961220/webclient-read-content-of-error-page
            Console.WriteLine("getContentsOfUrl:" + url);  // FIXME: how to do debug logging better?
            string content = null;
            WebClient webclient = new WebClient();
            try {
                content = webclient.DownloadString(url);
            }
            catch (WebException we) {
                // WebException.Status holds useful information
                Console.WriteLine(we.Message + "\n" + we.Status.ToString());  // FIXME:
                Stream receiveStream = we.Response.GetResponseStream();
                Encoding encode = System.Text.Encoding.GetEncoding("utf-8");
                StreamReader readStream = new StreamReader(receiveStream, encode);
                content = readStream.ReadToEnd();
            }
            catch (NotSupportedException ne) {
                Console.WriteLine(ne.Message);   // other errors
            }
            return content;
        }
        /// <summary>
        /// Get contents of file into string
        /// </summary>
        /// <param name="filepath">full filepath of file to read</param>
        /// <returns>contents of file or null</returns>
        public static string getContentsOfFile(string filepath)
        {
            try {
                using (StreamReader sr = new StreamReader(filepath)) {
                    String contents = sr.ReadToEnd();
                    Console.WriteLine("file contents:" + contents);
                    return contents;
                }
            }
            catch (Exception e) {
                Console.WriteLine("The file could not be read:");
                Console.WriteLine(e.Message);
            }
            return null;
        }

        // stolen from http://stackoverflow.com/questions/3354893/how-can-i-convert-a-datetime-to-the-number-of-seconds-since-1970
        public static DateTime ConvertFromUnixTimestamp(double timestamp)
        {
            DateTime origin = new DateTime(1970, 1, 1, 0, 0, 0, 0);
            return origin.AddSeconds(timestamp);
        }

        public static double ConvertToUnixTimestamp(DateTime date)
        {
            DateTime origin = new DateTime(1970, 1, 1, 0, 0, 0, 0);
            TimeSpan diff = date.ToUniversalTime() - origin;
            return Math.Floor(diff.TotalSeconds);
        }


    }
}
