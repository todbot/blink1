using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Timers;
using System.Drawing;
using System.Reflection;
using System.IO;
using System.Diagnostics;
using System.Configuration;
using System.Globalization;
using System.Net;
using MiniHttpd;
using Newtonsoft.Json;
using Blink1Lib;
using System.Collections.ObjectModel;
using System.Text.RegularExpressions;


namespace Blink1Control
{
    class Blink1Server
    {
        static int httpPortDefault = 8934;
        static string iftttEventUrl = "http://api.thingm.com/blink1/events";
        static float iftttUpdateInterval = 15.0F;
        static float urlUpdateInterval = 15.0F;

        HttpWebServer bhI = new HttpWebServer( httpPortDefault );
        //JTokenWriter jtw = new JTokenWriter();
        Blink1 blink1 = new Blink1();

        Dictionary<string, Blink1Input> inputs   = new Dictionary<string, Blink1Input>();
        Dictionary<string, Blink1Pattern> patterns = new Dictionary<string, Blink1Pattern>();

        // constructor
        public Blink1Server()
        {
            inputs = new Dictionary<string, Blink1Input>();
            patterns = new Dictionary<string, Blink1Pattern>();

            blink1.hostId = (string)Properties.Settings.Default["hostId"];
            
            blink1.regenerateBlink1Id();

            //MySettings.Instance.Parameters["foo"] = "bar";
            //MySettings.Instance.Save();
            //MySettings.Instance.Reload();

            //MySettings mysettings = new MySettings();
            //Blink1Input ainput = new Blink1Input("foobee", "url", "http://shut.up/now", null, null);
            //ainput.pname = "gosh!";
            //inputs["golly"] = ainput; // NOTE: this replaces input if already exists
            //mysettings.Parameters["todgod"] = "jorby";
            //MySettings.saveSettings(mysettings);

            //string todid = (string) Properties.Settings.Default["todId"];
            //if (todid != null) {
            //}
            /*
            if (Properties.Settings.Default.TheInputs == null) {
                Console.WriteLine("*** New Settings! ****\n");
                Properties.Settings.Default.TheInputs = new ObservableCollection<Blink1Input>
                {
                    new Blink1Input("bob","url",null,null,null),
                    new Blink1Input("sue","file",null,null,null),
                    new Blink1Input("joe","script",null,null,null)
                };
                Properties.Settings.Default.Save();
            }
            else {
                Console.WriteLine("****\n\n found TheInputs Settings!\n\n **********");
                Console.WriteLine(Properties.Settings.Default.TheInputs.Count.ToString());
            }
*/
            Console.WriteLine("Blink1Server!");

            try {
                VirtualDirectory root = new VirtualDirectory();
                DriveDirectory homeDir = new DriveDirectory(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
                foreach (DriveDirectory d in homeDir.GetDirectories()) {
                    root.AddDirectory(d.Path);
                }

                VirtualDirectory blink1dir = new VirtualDirectory("blink1", root);

                // FIXME: the below is completely gross, how to do HTTP routing in .NET?
                Blink1JSONFile id = new Blink1JSONFile("id", blink1dir, this);
                id.GetStringResponse = blink1Id;
                blink1dir.AddFile(id);   //add a virtual file for each json method

                Blink1JSONFile enumerate = new Blink1JSONFile("enumerate", blink1dir, this);
                enumerate.GetStringResponse = blink1Enumerate;
                blink1dir.AddFile(enumerate);   //add a virtual file for each json method

                Blink1JSONFile regen = new Blink1JSONFile("regenerateblink1id", blink1dir, this);
                regen.GetStringResponse = blink1RegenerateBlink1Id;
                blink1dir.AddFile(regen);

                Blink1JSONFile fadeToRGB = new Blink1JSONFile("fadeToRGB", blink1dir, this);
                fadeToRGB.GetStringResponse = blink1FadeToRGB;
                blink1dir.AddFile(fadeToRGB);

                Blink1JSONFile on = new Blink1JSONFile("on", blink1dir, this);
                on.GetStringResponse = blink1On;
                blink1dir.AddFile(on);

                Blink1JSONFile off = new Blink1JSONFile("off", blink1dir, this);
                off.GetStringResponse = blink1Off;
                blink1dir.AddFile(off);

                Blink1JSONFile pattern = new Blink1JSONFile("pattern", blink1dir, this);
                pattern.GetStringResponse = blink1Pattern;
                blink1dir.AddFile(pattern);

                Blink1JSONFile pattadd = new Blink1JSONFile("patternadd", blink1dir, this);
                pattadd.GetStringResponse = blink1PatternAdd;
                blink1dir.AddFile(pattadd);

                Blink1JSONFile pattdel = new Blink1JSONFile("patterndel", blink1dir, this);
                pattdel.GetStringResponse = blink1PatternDel;
                blink1dir.AddFile(pattdel);

                Blink1JSONFile pattdelall = new Blink1JSONFile("patterndelall", blink1dir, this);
                pattdelall.GetStringResponse = blink1PatternDelAll;
                blink1dir.AddFile(pattdelall);

                Blink1JSONFile input = new Blink1JSONFile("input", blink1dir, this);
                input.GetStringResponse = blink1Input;
                blink1dir.AddFile(input);

                Blink1JSONFile inputdel = new Blink1JSONFile("inputdel", blink1dir, this);
                inputdel.GetStringResponse = blink1InputDel;
                blink1dir.AddFile(inputdel);

                Blink1JSONFile inputdelall = new Blink1JSONFile("inputdelall", blink1dir, this);
                inputdelall.GetStringResponse = blink1InputDelAll;
                blink1dir.AddFile(inputdelall);

                Blink1JSONFile inputurl = new Blink1JSONFile("inputurl", blink1dir, this);
                inputurl.GetStringResponse = blink1InputUrl;
                blink1dir.AddFile(inputurl);

                /*
                                Blink1JSONFile id2       = new Blink1JSONFile("id2", blink1dir, blink1);
                                id2.GetStringResponse = blink1Id2;
                                blink1dir.AddFile(id2);   //add a virtual file for each json method
                                */
                root.AddDirectory(blink1dir);

                bhI.Root = root;
                bhI.Start();
            }
            catch (Exception e) {
                Console.WriteLine(e.ToString());
            }
        }

        // -----------------------------------------------------------------------------------------------
        // url methods
        //

        // why do we need this?
        public delegate string GetJSONStringResponse(HttpRequest request, Blink1Server aBlink1Server);

        //
        // direct blink(1) control url handling
        //
        
        //    /blink1/id -- Display blink1_id and blink1 serial numbers (if any)
        static string blink1Id(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id", blink1.getBlink1Id());
            List<string> serialnums = new List<string>();
            for( int i=0; i < blink1.getCachedCount(); i++ ) {  // FIXME: surely a smarter way to do this
                serialnums.Add( blink1.getCachedSerial(i) );
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "blink1 id");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/enumerate -- Re-enumerate and List available blink(1) devices
        static string blink1Enumerate(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            string blink1Id_old = blink1.getBlink1Id();

            blink1.enumerate();
            blink1.regenerateBlink1Id();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id_old", blink1Id_old);
            result.Add("blink1_id", blink1.getBlink1Id());
            List<string> serialnums = new List<string>();
            for (int i = 0; i < blink1.getCachedCount(); i++)
            {  // FIXME: surely a smarter way to do this
                serialnums.Add(blink1.getCachedSerial(i));
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "enumerate");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/regenerateblinkid -- Generate, save, and return new blink1_id
        static string blink1RegenerateBlink1Id(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            string blink1Id_old = blink1.getBlink1Id();

            blink1.hostId = null;
            blink1.enumerate();
            blink1.regenerateBlink1Id();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id_old", blink1Id_old);
            result.Add("blink1_id", blink1.getBlink1Id());
            List<string> serialnums = new List<string>();
            for (int i = 0; i < blink1.getCachedCount(); i++)
            {  // FIXME: surely a smarter way to do this
                serialnums.Add(blink1.getCachedSerial(i));
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "regenrateblink1id");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/fadeToRGB -- Send fadeToRGB command to blink(1) with hex color & fade time
        static string blink1FadeToRGB(HttpRequest request, Blink1Server blink1Server)
        {
            // FIXME: stop pattern player
            //NameValueCollection query = request.Query;
            string rgbstr  = request.Query.Get("rgb");
            string timestr = request.Query.Get("time");
            if (rgbstr == null) rgbstr = "#000000";
            if (timestr == null) timestr = "0.1";   
            Color colr = ColorTranslator.FromHtml(rgbstr);
            float secs = float.Parse(timestr, CultureInfo.InvariantCulture);

            blink1Server.fadeToRGB(secs, colr);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "fadeToRGB");
            result.Add("rgb", ColorTranslator.ToHtml(colr));
            result.Add("time", secs.ToString("F2", CultureInfo.InvariantCulture));
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/on -- Stop pattern playback and send fadeToRGB command to blink(1) with #FFFFFF & 0.1 sec fade time
        static string blink1On(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.fadeToRGB(0.1, Color.White);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "on");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/off -- Stop pattern playback and send fadeToRGB command to blink(1) with #000000 & 0.1 sec fade time
        static string blink1Off(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.fadeToRGB(0.1, Color.Black);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "off");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        // -----------------------------------------------------------------------------------------------
        // color patterns url handling
        //

        //    /blink1/pattern/ -- List saved color patterns
        static string blink1Pattern(HttpRequest request, Blink1Server blink1Server)
        {
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "pattern results");
            result.Add("patterns", blink1Server.patterns);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/pattern/add -- Add color pattern to color pattern list
        static string blink1PatternAdd(HttpRequest request, Blink1Server blink1Server)
        {
            string pname      = request.Query.Get("pname");
            string patternstr = request.Query.Get("pattern");

            Blink1Pattern pattern = new Blink1Pattern(pname, patternstr);
            blink1Server.patterns[pname] = pattern; // NOTE: this replaces pattern if already exists

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "pattern add");
            result.Add("pattern", pattern.ToString());
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/pattern/del -- Remove color pattern from color pattern list
        static string blink1PatternDel(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            Blink1Pattern patt = null ;
            if (pname != null) {
                patt = blink1Server.patterns[pname];
                patt.stop();
                blink1Server.patterns.Remove(pname);
                statusstr = "pattern '" + pname + "' removed";
            }
   
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/pattern/delall -- Remove all color patterns from color pattern list
        static string blink1PatternDelAll(HttpRequest request, Blink1Server blink1Server)
        {
            foreach (KeyValuePair<string, Blink1Pattern> kvp in blink1Server.patterns) {
                kvp.Value.stop();
            }
            blink1Server.patterns.Clear();
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "all patterns removed");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/pattern/play -- Play/test a specific color pattern
        static string blink1PatternPlay(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            if (pname != null) {
                Blink1Pattern patt = null ;
                if( blink1Server.patterns.TryGetValue(pname, out patt)) {
                    patt.blink1Server = blink1Server; // justin case
                    patt.play();
                    statusstr = "pattern '" + pname + "' playing";
                }
            }
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/pattern/stop -- Stop a pattern playback, for a given pattern or all patterns
        static string blink1PatternStop(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            if (pname != null) {
                Blink1Pattern patt = null ;
                if( blink1Server.patterns.TryGetValue(pname, out patt) ) {
                    patt.stop();
                    statusstr = "pattern '" + pname + "' stopped";
                }
            }
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        // 
        // inputs
        //

        //    /blink1/input/ -- List configured inputs, enable or disable input watching
        static string blink1Input(HttpRequest request, Blink1Server blink1Server)
        {
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "input results");
            result.Add("inputs", blink1Server.inputs);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }
        //    /blink1/input/del -- Remove a configured input
        static string blink1InputDel(HttpRequest request, Blink1Server blink1Server)
        {
            string iname = request.Query.Get("iname");
            string statusstr = "no input by that name";
            Blink1Input input = null;
            if (iname != null) {
                input = blink1Server.inputs[iname];
                input.stop();
                blink1Server.inputs.Remove(iname);
                statusstr = "input '" + iname + "' removed";
            }

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/input/delall -- Remove all configured inputs
        static string blink1InputDelAll(HttpRequest request, Blink1Server blink1Server)
        {
            foreach (KeyValuePair<string, Blink1Input> kvp in blink1Server.inputs) {
                kvp.Value.stop();
            }
            blink1Server.patterns.Clear();
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "all patterns removed");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/input/url -- Add and Start URL watcher on given URL
        static string blink1InputUrl(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string url   = request.Query.Get("arg1");
            string test  = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test==null) ? false : (test.Equals("on") || test.Equals("true"));

            string statusstr = "must specifiy 'iname' and 'arg1' (url)";

            Blink1Input input = null;
            if( url != null && iname != null ) {
                statusstr = "input url";
                input = new Blink1Input(iname, "url", url, null, null);
                input.pname = pname;
                blink1Server.updateUrlInput(input);
                if (!testmode) {
                    blink1Server.inputs[iname] = input; // NOTE: this replaces input if already exists
                }
            }

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("input", input);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/input/ifttt -- Add and Start watching messages from IFTTT webservice
        static string blink1InputIfttt(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string url = request.Query.Get("url");
            string test = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test == null) ? false : (test.Equals("on") || test.Equals("true"));

            // FIXME: insert magic here

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "not implemented yet");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        // ----------------------------------------------------------------------------------------
        // input update url handling
        //

        /// <summary>
        /// Periodically update the inputs, triggering color patterns if needed
        /// Runs every 15(?) seconds
        /// </summary>
        public void updateInputs()
        {
            foreach (var pair in inputs) {
                Blink1Input input = pair.Value;
                if (input.type.Equals("ifttt")) { // FIXME: there's totally a better way of doing this
                    updateIftttInput(input);
                }
                else if (input.type.Equals("url")) {
                    updateUrlInput(input);
                }
                else if (input.type.Equals("file")) {
                }
                else if (input.type.Equals("script")) {
                }
            }
        }

        public void updateIftttInput(Blink1Input input)
        {

        }

        public void updateUrlInput(Blink1Input input)
        {
            string url = input.arg1;

            string resp = getContentOfUrl(url);
            if (resp == null) {
                input.lastVal = "bad url";
                return;
            }
            string patternstr = parsePatternOrColorString(resp);

            if (patternstr != null && !patternstr.Equals(input.lastVal)) {
                input.lastVal = patternstr;
                playPattern(patternstr);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="url"></param>
        /// <returns></returns>
        public string getContentOfUrl(string url)
        {
            string content = null;
            WebClient webclient = new WebClient();
            try {
                content = webclient.DownloadString(url);
            }
            catch (WebException we) {
                // WebException.Status holds useful information
                Console.WriteLine(we.Message + "\n" + we.Status.ToString());
            }
            catch (NotSupportedException ne) {
                Console.WriteLine(ne.Message);   // other errors
            } 
            return content;
        }

        // ----------------------------------------------------------------------------------------
        // pattern handling
        //

        /// <summary>
        // Play a color pattern.
        // Can restart if already playing, or just leave be an already playing pattern
        //
        /// </summary>
        /// <param name="pname">pattern name / color code to play</param>
        /// <returns> true if pattern was played, false if not</returns>
        public Boolean playPattern(string pname)
        {
            if (pname == null) return false;
            // if rgb hex code, just do it
            if (pname.StartsWith("#")) {
                Color c = ColorTranslator.FromHtml(pname);
                fadeToRGB(0.1, c);
                return true;
            }
            // otherwise treat it as a color pattern name
            Blink1Pattern patt = null;
            if (patterns.TryGetValue(pname, out patt)) {
                patt.blink1Server = this; // just in case
                patt.play();
                return true;
            }
            return false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public string parsePatternOrColorString(string str)
        {
            // read color pattern, can be of form: pattern: "my pattern"
            Regex patternregex = new Regex(@"\""*pattern\""*:\s*\""(.+?)\""");
            Match matchp = patternregex.Match(str);
            if( matchp.Success ) { 
                return matchp.Groups[1].Value;
            }
            //
            Regex hexcoderegex = new Regex(@"(#[A-Fa-f0-9]{6})");
            Match matchc = hexcoderegex.Match(str.ToUpper());
            if( matchc.Success ) {
                return matchc.Groups[1].Value;
            }

            //Color c = ColorTranslator.FromHtml(str);
            return null;
        }

        /// <summary>
        /// Same as Blink1.fadeToRGB, but does an open/close around it.
        /// Called by Blink1Patterns to send colors to blink(1)
        /// </summary>
        public void fadeToRGB(double secs, Color c)
        {
            Console.WriteLine("fadeToRGB: rgb:" + ColorTranslator.ToHtml(c) + " secs:" + secs);
            blink1.open();
            blink1.fadeToRGB((int)(secs * 1000), c.R, c.G, c.B);
            blink1.close();
        }


//    /blink1/lastColor -- Return the last color command sent to blink(1)


//    /blink1/input/file -- Add and Start file watcher on given filepath


//    /blink1/input/script -- Add and Start command-line script executer

//    /blink1/input/scriptlist -- List available scripts to run

//    /blink1/input/cpuload -- Add and Start CPU load watching input

//    /blink1/input/netload -- Start network load watching input



        // testing
        static string blink1Id2(HttpRequest request, Blink1 blink1)//example
        {
            Properties.Settings.Default["blink1Id"] = blink1.getBlink1Id();
            Properties.Settings.Default.Save();
            //Settings.Default["blink1Id"] = blink1.getBlink1Id();
            //Settings.Default.Save(); // Saves settings in application configuration file
            return @"\n\n{ suck it }\n\n";
        }

        /// <summary>
        /// Weirdass wrapper for binding urls to funcblocks, why do we need this?
        /// </summary>
        public class Blink1JSONFile : IFile
        {
            public Blink1Server blink1Server;
            string name;
            IDirectory parent;

            public Blink1JSONFile(string name, IDirectory parent, Blink1Server aBlink1Server)
            {
                this.name = name;
                this.parent = parent;
                this.blink1Server = aBlink1Server;
                GetStringResponse = delegate(HttpRequest input, Blink1Server bs)
                {
                    return input.ToString();
                };
            }

            public GetJSONStringResponse GetStringResponse;

            public void OnFileRequested(HttpRequest request,
                                        IDirectory directory)
            {
                // Assign a MemoryStream to hold the response content.
                request.Response.ResponseContent = new MemoryStream();
                // Create a StreamWriter to which we
                // can write some text, and write to it.
                StreamWriter writer = new StreamWriter(request.Response.ResponseContent);
                writer.WriteLine(GetStringResponse(request, blink1Server));
                // Don't forget to flush!
                writer.Flush();
            }

            public string ContentType { get { return ContentTypes.GetExtensionType(".txt"); } }
            public string Name { get { return name; } }
            public IDirectory Parent { get { return parent; } }

            public void Dispose()
            {
            }
        }

    }

}


        /*
        // this is for tod testing, trying to figure otu httpwebserver class
        public class Blink1IDir : IDirectory
        {
            string name;
            IDirectory parent;
            public Blink1IDir(string name, IDirectory parent)
            {
                this.name = name;
                this.parent = parent;
                Console.WriteLine("Blink1IDir Constructor");
            }
            public IDirectory GetDirectory(string dir)
            {
                Console.WriteLine("dir: " + dir);
                return null;
            }
            public IFile GetFile(string filename)
            {
                Console.WriteLine("filename: " + filename);
                return null;
            }
            public ICollection GetDirectories()
            {
                Console.WriteLine("getDirectories");
                return null;    
            }
            public ICollection GetFiles()
            {
                Console.WriteLine("getFiles");
                return null;
            }
            public IResource GetResource(string name)
            {
                Console.WriteLine("name: " + name);
                return null;
            }
            public string Name { get { return name; } }
            public IDirectory Parent { get { return parent; } }
            public void Dispose()
            {
            }
        }  // end testing class
        */

        //J bhIJW = new JsonTextWriter(new System.IO.TextWriter());
