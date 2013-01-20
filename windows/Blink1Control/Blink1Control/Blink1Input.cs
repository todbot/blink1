using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using System.Net;
using System.IO;
using Newtonsoft.Json;

namespace Blink1Control
{
    /// <summary>
    /// A representation of a data input mechanism/technique (e.g. IFTTT, URL, script,etc.)
    /// This could be implemented as many subclasses ("Blink1InputUrl") but instead
    /// all inputs are the same and have a "type".
    /// In addition to a type, inputs can have up to 3 arguments (arg1,arg2,arg3)
    /// 
    /// </summary>
    public class Blink1Input
    {
        public static string iftttEventUrl = "http://api.thingm.com/blink1/events";
        //public static float iftttUpdateInterval = 15.0F;
        //public static float urlUpdateInterval = 15.0F;
        // FIXME: hacks
        public static DateTime iftttLastTime;
        public static int iftttUpdateInterval = 15;
        public static string iftttLastContent;

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
        private DateTime lastDateTime;
        public string lastTime
        {
            get { return ConvertToUnixTimestamp(lastDateTime).ToString();  }
            set {
                double v=0;
                Double.TryParse(value, out v);
                lastDateTime = ConvertFromUnixTimestamp(v);
            }
        }

        // holder of last valid response/content
        private string lastContent;

        [JsonIgnore]
        public Blink1Server blink1Server { private get; set; }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="type"></param>
        /// <param name="arg1"></param>
        /// <param name="arg2"></param>
        /// <param name="arg3"></param>
        public Blink1Input(string name, string type, string arg1, string arg2, string arg3)
            : this(null, name, null, type, arg1, arg2, arg3)
        {
        }

        public Blink1Input(Blink1Server b1s, string iname, string pname, string type, string arg1)
            : this(b1s, iname, pname, type, arg1, null, null)
        {
        }

        public Blink1Input(Blink1Server b1s, string iname, string pname, string type, string arg1, string arg2, string arg3)
        {
            this.blink1Server = b1s;
            this.iname = iname; 
            this.pname = pname;
            this.type = type;
            this.arg1 = arg1; this.arg2 = arg2; this.arg3 = arg3;
            this.lastDateTime = DateTime.Now;  // FIXME: what about UTC?
        }

        public Blink1Input()
        {
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
        public Boolean update()
        {
            Boolean wasTriggered = false;
            if (type.Equals("ifttt")) { // FIXME: there's totally a better way of doing this
                wasTriggered = updateIftttInput();
            }
            else if (type.Equals("url")) {
                wasTriggered = updateUrlInput();
            }
            else if (type.Equals("file")) {
                wasTriggered = updateFileInput();
            }
            else if (type.Equals("script")) {
                wasTriggered = updateScriptInput();
            }
            return wasTriggered;
        }

        /// <summary>
        /// periodically fetch events from IFTTT gateway
        /// </summary>
        /// <returns>true if pattern was played from input match</returns>
        public Boolean updateIftttInput()
        {
            if (iftttLastContent==null ) {
                lastVal = "could not connect";
                return false;
            }

            string rulename = arg1;
            
            IftttResponse iftttResponse = JsonConvert.DeserializeObject<IftttResponse>( iftttLastContent );
            if (iftttResponse.event_count > 0) {
                long lastsecs = (long)ConvertToUnixTimestamp(lastDateTime);
                foreach (IftttEvent ev in iftttResponse.events) {
                    long evdate = long.Parse(ev.date);
                    string evname = ev.name;
                    lastVal = ev.source;
                    possibleVals = evname; // FIXME: should be array
                    //Blink1Server.Log("--ifttt ev.name:" + evname);
                    if (rulename.Equals(evname)) {
                        Blink1Server.Log("---ifttt match: evdate:" + evdate + ", lastsecs:" + lastsecs + ", dt:" + (evdate - lastsecs));
                        lastDateTime = ConvertFromUnixTimestamp(evdate);
                        if (evdate > lastsecs) {
                            blink1Server.playPattern(pname);
                            return true;
                        }
                    }
                }
            }
            else {
                lastVal = "no IFTTT data";
            }
            return false;
        }

        /// <summary>
        /// Periodically fetch URL, looking for color patern name or color code 
        /// </summary>
        /// <returns>true if pattern was played from input match</returns>
        public Boolean updateUrlInput()
        {
            lastDateTime = DateTime.Now.ToUniversalTime();
            string url = arg1;
            string resp = getContentsOfUrl(url);
            lastContent = resp;
            if (resp == null) {
                lastVal = "bad url";
                return false;
            }
            string patternstr = parsePatternOrColorString(resp);
            if (patternstr != null && !patternstr.Equals(lastVal)) {
                lastVal = patternstr;
                blink1Server.playPattern(patternstr);
                return true;
            }
            else {
                lastVal = "no color or pattern in output";
            }
            return false;
        }

        /// <summary>
        /// Periodically check file, looking for color pattern name or color code
        ///
        /// </summary>
        /// <returns>true if pattern was played from input match</returns>
        public Boolean updateFileInput()
        {
            lastDateTime = DateTime.Now.ToUniversalTime();
            string filepath = arg1;
            string resp = getContentsOfFile(filepath);
            lastContent = resp;
            if (resp == null) {
                lastVal = "bad filename";
                return false;
            }
            string patternstr = parsePatternOrColorString(resp);
            if (patternstr != null && !patternstr.Equals(lastVal)) {
                lastVal = patternstr;
                blink1Server.playPattern(patternstr);
                return true;
            }
            else {
                lastVal = "no color or pattern in output";
            }
            return false;
        }

        /// <summary>
        /// Periodically excecute script, looking for color pattern name or color code
        /// </summary>
        /// <returns>true if pattern was played from input match</returns>
        public Boolean updateScriptInput()
        {
            lastDateTime = DateTime.Now.ToUniversalTime();
            string scriptname = arg1;
            string resp = execScript(scriptname);
            lastContent = resp;
            if (resp == null) {
                lastVal = "bad scriptname";
                return false;
            }
            string patternstr = parsePatternOrColorString(resp);
            if (patternstr != null && !patternstr.Equals(lastVal)) {
                lastVal = patternstr;
                blink1Server.playPattern(patternstr);
                return true;
            }
            else {
              lastVal = "no color or pattern in output";
            }
            return false;
        }

        // ---------------------------------------------------------------------------------------
        // static utility methods 

        /// <summary>
        /// Static metho callled peridically, but only once for all IFTTT inputs. 
        /// FIXE: This whole method is a super hack
        /// </summary>
        /// <param name="input"></param>
        /// <param name="normalmode"></param>
        public static void getIftttResponse(Boolean normalmode)
        {
            // only update URLs every 30 secs
            DateTime now = DateTime.Now; //.ToUniversalTime(); 
            TimeSpan diff = now - iftttLastTime;
            //Console.WriteLine("getIftttResponse now:"+now+", diff:" + diff);
            if( normalmode && (diff.Seconds < iftttUpdateInterval) ) {
                return;
            }
            iftttLastTime = now;

            string eventUrl = iftttEventUrl +"/"+ Blink1Server.blink1Id;  // FIXME: hack
            iftttLastContent = getContentsOfUrl(eventUrl);
            //Blink1Server.Log("iftttLastContent:" + iftttLastContent);
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
            if (url == null) return null;
            WebClient webclient = new WebClient();
            try {
                content = webclient.DownloadString(url);
            }
            catch (WebException we) {
                // WebException.Status holds useful information
                Console.WriteLine(we.Message); // + "\n" + we.Status.ToString());  // FIXME:
                /*
                Stream receiveStream = we.Response.GetResponseStream();
                Encoding encode = System.Text.Encoding.GetEncoding("utf-8");
                StreamReader readStream = new StreamReader(receiveStream, encode);
                content = readStream.ReadToEnd();
                Console.WriteLine("content: " + content);
                 */
                content = null;
            }
            catch (System.ArgumentException sae) {
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

        /// <summary>
        /// Execute a script 
        /// </summary>
        /// <param name="scriptpath"></param>
        /// <returns></returns>
        public static string execScript(string scriptpath)
        {
            //stolen from: http://stackoverflow.com/questions/878632/best-way-to-call-external-program-in-c-sharp-and-parse-output
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.FileName = "cmd.exe";
            p.StartInfo.Arguments = "/c " + scriptpath; // dir *.cs";
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.RedirectStandardOutput = true;
            p.Start();

            string output = p.StandardOutput.ReadToEnd();
            p.WaitForExit();
            Console.WriteLine("Output:"+output);

            return output;
        }

        // from: http://www.epochconverter.com/
        public static long epochSecs(DateTime date)
        {
            //epoch = (DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000000;
            long epochsecs = (date.ToUniversalTime().Ticks - 621355968000000000) / 10000000;
            return epochsecs;
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
            //TimeSpan diff = date.ToUniversalTime() - origin;
            TimeSpan diff = date - origin; // this works, but what about IFTTT's data?
            return Math.Floor(diff.TotalSeconds);
        }


    }
}
