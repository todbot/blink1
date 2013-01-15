using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Drawing;
using System.Reflection;
using System.IO;
using System.Diagnostics;
using System.Configuration;
using System.Globalization;
using System.Collections.ObjectModel;
using MiniHttpd;
using Newtonsoft.Json;
using Blink1Lib;


namespace Blink1Control
{
    public class Blink1Server
    {
        static int httpPortDefault = 8934;
        public static string iftttEventUrl = "http://api.thingm.com/blink1/events";
        public static float iftttUpdateInterval = 15.0F;
        public static float urlUpdateInterval = 15.0F;
        
        HttpWebServer httpServer = new HttpWebServer(httpPortDefault);

        Dictionary<string, Blink1Input> inputs = new Dictionary<string, Blink1Input>();
        Dictionary<string, Blink1Pattern> patterns = new Dictionary<string, Blink1Pattern>();

        Timer inputsTimer;
        Boolean inputsEnable = true;
        float inputUpdateInterval = 5.0F;

        // stolen from: http://stackoverflow.com/questions/7427909/how-to-tell-json-net-globally-to-apply-the-stringenumconverter-to-all-enums
        static JsonSerializerSettings jsonSerializerSettings = new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore };

        static Blink1 Sblink1 = new Blink1();

        public Blink1 blink1 { get { return Blink1Server.Sblink1; } private set { } }
        public static string blink1Id { get { return Blink1Server.Sblink1.blink1Id; } }

        /*
        public static Blink1Server staticBlink1Server;
        public static void setBlink1Server(Blink1Server b1s)
        {
            staticBlink1Server = b1s;
        }
        public static Blink1Server getBlink1Server(Blink1Server b1s)
        {
            return staticBlink1Server;
        }
        */

        public static bool logToScreen;

        /// <summary>
        /// Extremely simplistic logging system. one method only!
        /// FIXME: need to investigate System.Diagnostics.Debug.WriteLine()
        /// </summary>
        /// <param name="s"></param>
        public static void Log(string s)
        {
            if (logToScreen) Console.WriteLine(s);
        }

        /// <summary>
        /// 
        /// </summary>
        public void loadSettings()
        {
            // some notes on settings:
            // http://stackoverflow.com/questions/1804302/where-is-the-data-for-properties-settings-default-saved
            // http://stackoverflow.com/questions/4647796/c-how-to-make-sure-a-settings-variable-exists-before-attempting-to-use-it-from
            // http://stackoverflow.com/questions/469742/where-are-user-mode-net-settings-stored
            // On my Win7 box, the settings are stored at:
            // /c/Users/biff/AppData/Local/ThingM/Blink1Control.vshost.exe_Url_0icskbpuzcclllscwtnuhj23qahf4mn1/1.0.0.0/

            // we assume hostId is always set, it defaults to "00000000" on fresh installation
            // (or we could not set it in Properties and check for KeyNotFoundException)
            blink1.hostId = (string)Properties.Settings.Default["hostId"];

            logToScreen = (bool) Properties.Settings.Default["logToScreen"];

            String patternsstr = (string)Properties.Settings.Default["patterns"];
            String inputsstr = (string)Properties.Settings.Default["inputs"];
            Log("patterns: " + patternsstr);
            Log("inputs: " + inputsstr);
            patterns = JsonConvert.DeserializeObject<Dictionary<string, Blink1Pattern>>(patternsstr);
            inputs = JsonConvert.DeserializeObject<Dictionary<string, Blink1Input>>(inputsstr);
            if (patterns == null) {
                patterns = new Dictionary<string, Blink1Pattern>();
            }
            if (inputs == null) {
                inputs = new Dictionary<string, Blink1Input>();
            }
            foreach (KeyValuePair<string, Blink1Input> kvp in inputs) {
                kvp.Value.blink1Server = this;
            }

            Log("inputs:"+   JsonConvert.SerializeObject(inputs, Formatting.Indented, jsonSerializerSettings));
            Log("patterns:"+   JsonConvert.SerializeObject(patterns, Formatting.Indented, jsonSerializerSettings));

            blink1.regenerateBlink1Id();

            Log("blink1.hostId:" + blink1.hostId);
            Log("blink1.blink1Id:" + blink1.blink1Id);
        }

        /// <summary>
        /// 
        /// </summary>
        public void saveSettings()
        {
            Properties.Settings.Default["hostId"] = blink1.hostId;
            Properties.Settings.Default["inputs"] =  JsonConvert.SerializeObject(inputs, Formatting.Indented, jsonSerializerSettings);
            Properties.Settings.Default["patterns"] = JsonConvert.SerializeObject(patterns, Formatting.Indented, jsonSerializerSettings);
            Properties.Settings.Default.Save();
        }

        // constructor
        public Blink1Server()
        {
            Log("Blink1Server!");
            blink1.open();

            loadSettings();

            Log("Running on port " + httpPortDefault);
            Log("blink1Id:" + blink1Id);
            long updateMillis = (long)(inputUpdateInterval * 1000);
            inputsTimer = new Timer( updateInputs, null, updateMillis, updateMillis);

            saveSettings();

            try {
                VirtualDirectory root = new VirtualDirectory();
                // note: this works because there is an xcopy as a post build-build event
                string baseDir = System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                baseDir += "\\html\\blink_1";
                DriveDirectory htmlDir = new DriveDirectory(baseDir);
                Log("htmlDir: " + htmlDir.Path);
                root.AddDirectory(htmlDir.Path);

                VirtualDirectory blink1dir = new VirtualDirectory("blink1", root);
                VirtualDirectory inputdir  = new VirtualDirectory("input", blink1dir);
                VirtualDirectory patterndir = new VirtualDirectory("pattern", blink1dir);

                // FIXME: the below is completely gross, how to do good HTTP routing with MiniHttpd?
                Blink1JSONFile id = new Blink1JSONFile("id", blink1dir, this);
                id.GetStringResponse = Ublink1Id;
                blink1dir.AddFile(id);   //add a virtual file for each json method

                Blink1JSONFile enumerate = new Blink1JSONFile("enumerate", blink1dir, this);
                enumerate.GetStringResponse = Ublink1Enumerate;
                blink1dir.AddFile(enumerate);   //add a virtual file for each json method

                Blink1JSONFile regen = new Blink1JSONFile("regenerateblink1id", blink1dir, this);
                regen.GetStringResponse = Ublink1RegenerateBlink1Id;
                blink1dir.AddFile(regen);

                Blink1JSONFile fadeToRGB = new Blink1JSONFile("fadeToRGB", blink1dir, this);
                fadeToRGB.GetStringResponse = Ublink1FadeToRGB;
                blink1dir.AddFile(fadeToRGB);

                Blink1JSONFile on = new Blink1JSONFile("on", blink1dir, this);
                on.GetStringResponse = Ublink1On;
                blink1dir.AddFile(on);

                Blink1JSONFile off = new Blink1JSONFile("off", blink1dir, this);
                off.GetStringResponse = Ublink1Off;
                blink1dir.AddFile(off);

                Blink1JSONFile lastColor = new Blink1JSONFile("lastColor", blink1dir, this);
                lastColor.GetStringResponse = Ublink1LastColor;
                blink1dir.AddFile(lastColor);


                Blink1JSONFile pattern = new Blink1JSONFile("patterns", blink1dir, this);
                pattern.GetStringResponse = Ublink1Pattern;
                blink1dir.AddFile(pattern);

                Blink1JSONFile pattadd = new Blink1JSONFile("add", patterndir, this);
                pattadd.GetStringResponse = Ublink1PatternAdd;
                patterndir.AddFile(pattadd);

                Blink1JSONFile pattdel = new Blink1JSONFile("del", patterndir, this);
                pattdel.GetStringResponse = Ublink1PatternDel;
                patterndir.AddFile(pattdel);

                Blink1JSONFile pattdelall = new Blink1JSONFile("delall", patterndir, this);
                pattdelall.GetStringResponse = Ublink1PatternDelAll;
                patterndir.AddFile(pattdelall);

                Blink1JSONFile pattplay = new Blink1JSONFile("play", patterndir, this);
                pattplay.GetStringResponse = Ublink1PatternPlay;
                patterndir.AddFile(pattplay);

                Blink1JSONFile pattstop = new Blink1JSONFile("stop", patterndir, this);
                pattstop.GetStringResponse = Ublink1PatternStop;
                patterndir.AddFile(pattstop);

                Blink1JSONFile pattstopall = new Blink1JSONFile("stopall", patterndir, this);
                pattstopall.GetStringResponse = Ublink1PatternStopAll;
                patterndir.AddFile(pattstopall);


                Blink1JSONFile input = new Blink1JSONFile("inputs", blink1dir, this);
                input.GetStringResponse = Ublink1Input;
                blink1dir.AddFile(input);

                Blink1JSONFile inputdel = new Blink1JSONFile("del", inputdir, this);
                inputdel.GetStringResponse = Ublink1InputDel;
                inputdir.AddFile(inputdel);

                Blink1JSONFile inputdelall = new Blink1JSONFile("delall", inputdir, this);
                inputdelall.GetStringResponse = Ublink1InputDelAll;
                inputdir.AddFile(inputdelall);

                Blink1JSONFile inputurl = new Blink1JSONFile("url", inputdir, this);
                inputurl.GetStringResponse = Ublink1InputUrl;
                inputdir.AddFile(inputurl);

                Blink1JSONFile inputfile = new Blink1JSONFile("file", inputdir, this);
                inputfile.GetStringResponse = Ublink1InputFile;
                inputdir.AddFile(inputfile);

                Blink1JSONFile inputscript = new Blink1JSONFile("script", inputdir, this);
                inputscript.GetStringResponse = Ublink1InputScript;
                inputdir.AddFile(inputscript);

                Blink1JSONFile inputifttt = new Blink1JSONFile("ifttt", inputdir, this);
                inputifttt.GetStringResponse = Ublink1InputIfttt;
                inputdir.AddFile(inputifttt);

                // TESTING
                // embedding slashes in path name does not work
                //Blink1JSONFile foobar = new Blink1JSONFile("foo/bar", blink1dir, this);
                Blink1JSONFile foobar = new Blink1JSONFile("foo/bar", blink1dir, this);
                foobar.GetStringResponse = Ufoobar;
                blink1dir.AddFile(foobar);   //add a virtual file for each json method
                VirtualDirectory blarg = new VirtualDirectory();
                Blink1Directory bd = new Blink1Directory("floop", root);
                // TESTING END

                blink1dir.AddDirectory(inputdir);
                blink1dir.AddDirectory(patterndir);
                root.AddDirectory(bd);

                root.AddDirectory(blink1dir);
                httpServer.Root = root;

                ConsoleWriter writer = new ConsoleWriter();
                httpServer.Log = writer;
                httpServer.LogRequests = true;
                writer.OnWrite += new ConsoleWriter.WriteEventHandler(writer_OnWrite);

                httpServer.Start();
            }
            catch (Exception e) {
                Log(e.ToString());
            }
        }

        #region URL Methods

        // -----------------------------------------------------------------------------------------------
        // url methods
        //

        // why do we need this?
        public delegate string GetJSONStringResponse(HttpRequest request, Blink1Server aBlink1Server);

        // TESTING
        static string Ufoobar(HttpRequest request, Blink1Server blink1Server)//example
        {
            //Properties.Settings.Default["blink1Id"] = blink1Server.blink1Id;
            //Properties.Settings.Default.Save();
            //Settings.Default["blink1Id"] = blink1.getBlink1Id();
            //Settings.Default.Save(); // Saves settings in application configuration file
            return "\n\n{ suck it whitey }\n\n";
        }

        //
        // direct blink(1) control url handling
        //

        //    /blink1/id -- Display blink1_id and blink1 serial numbers (if any)
        static string Ublink1Id(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id", blink1.blink1Id);
            List<string> serialnums = new List<string>();
            for (int i = 0; i < blink1.getCachedCount(); i++) {  // FIXME: surely a smarter way to do this
                serialnums.Add(blink1.getCachedSerial(i));
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "blink1 id");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/enumerate -- Re-enumerate and List available blink(1) devices
        static string Ublink1Enumerate(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            string blink1Id_old = blink1.blink1Id;

            blink1.enumerate();
            blink1.regenerateBlink1Id();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id_old", blink1Id_old);
            result.Add("blink1_id", blink1.blink1Id);
            List<string> serialnums = new List<string>();
            for (int i = 0; i < blink1.getCachedCount(); i++) {  // FIXME: surely a smarter way to do this
                serialnums.Add(blink1.getCachedSerial(i));
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "enumerate");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/regenerateblinkid -- Generate, save, and return new blink1_id
        static string Ublink1RegenerateBlink1Id(HttpRequest request, Blink1Server blink1Server)
        {
            Blink1 blink1 = blink1Server.blink1;
            string blink1Id_old = blink1.blink1Id;

            blink1.hostId = null;
            blink1.enumerate();
            blink1.regenerateBlink1Id();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id_old", blink1Id_old);
            result.Add("blink1_id", blink1.blink1Id);
            List<string> serialnums = new List<string>();
            for (int i = 0; i < blink1.getCachedCount(); i++) {  // FIXME: surely a smarter way to do this
                serialnums.Add(blink1.getCachedSerial(i));
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "regenrateblink1id");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/fadeToRGB -- Send fadeToRGB command to blink(1) with hex color & fade time
        static string Ublink1FadeToRGB(HttpRequest request, Blink1Server blink1Server)
        {
            // FIXME: stop pattern player
            //NameValueCollection query = request.Query;
            string rgbstr = request.Query.Get("rgb");
            string timestr = request.Query.Get("time");
            Log("rgb: " + rgbstr);
            if (rgbstr == null) rgbstr = "#000000";
            if (timestr == null) timestr = "0.1";
            Color colr = ColorTranslator.FromHtml(rgbstr);
            float secs = float.Parse(timestr, CultureInfo.InvariantCulture);

            blink1Server.fadeToRGB(secs, colr);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "fadeToRGB");
            result.Add("rgb", Blink1.colorToHexCode(colr));
            result.Add("time", secs.ToString("F2", CultureInfo.InvariantCulture));
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/on -- Stop pattern playback and send fadeToRGB command to blink(1) with #FFFFFF & 0.1 sec fade time
        static string Ublink1On(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.fadeToRGB(0.1, Color.White);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "on");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/off -- Stop pattern playback and send fadeToRGB command to blink(1) with #000000 & 0.1 sec fade time
        static string Ublink1Off(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.fadeToRGB(0.1, Color.Black);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "off");
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

        //    /blink1/lastColor -- Return the last color command sent to blink(1)
        static string Ublink1LastColor(HttpRequest request, Blink1Server blink1Server)
        {
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "lastColor");
            result.Add("lastColor", blink1Server.blink1.lastColorString);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }


        // -----------------------------------------------------------------------------------------------
        // color patterns url handling
        //

        //    /blink1/pattern/ -- List saved color patterns
        static string Ublink1Pattern(HttpRequest request, Blink1Server blink1Server)
        {
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "pattern results");
            result.Add("patterns", blink1Server.patterns.Values);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/add -- Add color pattern to color pattern list
        static string Ublink1PatternAdd(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string patternstr = request.Query.Get("pattern");
            string statusstr = "pattern add";

            Blink1Pattern pattern = null;
            if (pname != null && patternstr != null) {
                pattern = new Blink1Pattern(pname);
                Boolean goodpattern = pattern.parsePatternStr(patternstr);
                if (goodpattern) {
                    blink1Server.patterns[pname] = pattern; // NOTE: this replaces pattern if already exists
                }
                else {
                    statusstr = "error: pattern badly formatted";
                }
            }
            else {
                statusstr = "error: need 'pname' and 'pattern' (e.g. '2,#ff00ff,0.5,#00ff00,0.5') argument";
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("pattern", pattern);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/del -- Remove color pattern from color pattern list
        static string Ublink1PatternDel(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            Blink1Pattern patt = null;
            if (pname != null) {
                patt = blink1Server.patterns[pname];
                patt.stop();
                blink1Server.patterns.Remove(pname);
                statusstr = "pattern '" + pname + "' removed";
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/delall -- Remove all color patterns from color pattern list
        static string Ublink1PatternDelAll(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.stopAllPatterns();
            blink1Server.patterns.Clear();
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "all patterns removed");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/play -- Play/test a specific color pattern
        static string Ublink1PatternPlay(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            if (pname != null) {
                Blink1Pattern patt = null;
                if (blink1Server.patterns.TryGetValue(pname, out patt)) {
                    patt.blink1Server = blink1Server; // justin case
                    patt.play();
                    statusstr = "pattern '" + pname + "' playing";
                }
            }
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/stop -- Stop a pattern playback for a given pattern
        static string Ublink1PatternStop(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string statusstr = "no pattern by that name";
            if (pname != null) {
                Blink1Pattern patt = null;
                if (blink1Server.patterns.TryGetValue(pname, out patt)) {
                    patt.stop();
                    statusstr = "pattern '" + pname + "' stopped";
                }
            }
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/pattern/stopall -- Stop all pattern playback
        static string Ublink1PatternStopAll(HttpRequest request, Blink1Server blink1Server)
        {
            blink1Server.stopAllPatterns();
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "all patterns stopped");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        // 
        // inputs
        //

        //    /blink1/input/ -- List configured inputs, enable or disable input watching
        static string Ublink1Input(HttpRequest request, Blink1Server blink1Server)
        {
            string enabled = request.Query.Get("enabled");
            if (enabled != null) {
                blink1Server.inputsEnable = (enabled.Equals("on") || enabled.Equals("true"));
            }
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "input results");
            result.Add("enabled", blink1Server.inputsEnable);
            result.Add("inputs", blink1Server.inputs.Values);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }
        //    /blink1/input/del -- Remove a configured input
        static string Ublink1InputDel(HttpRequest request, Blink1Server blink1Server)
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

            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/input/delall -- Remove all configured inputs
        static string Ublink1InputDelAll(HttpRequest request, Blink1Server blink1Server)
        {
            foreach (KeyValuePair<string, Blink1Input> kvp in blink1Server.inputs) {
                kvp.Value.stop();
            }
            blink1Server.inputs.Clear();
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "all patterns removed");
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/input/url -- Add and Start URL watcher on given URL
        static string Ublink1InputUrl(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string url = request.Query.Get("arg1").Trim();
            string test = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test == null) ? false : (test.Equals("on") || test.Equals("true"));

            string statusstr = "must specifiy 'iname' and 'arg1' (url)";

            Blink1Input input = null;
            if (url != null && iname != null) {
                statusstr = "input url";
                input = new Blink1Input(blink1Server, iname, pname, "url", url);

                input.updateUrlInput();
                if (!testmode) {
                    blink1Server.inputs[iname] = input; // NOTE: this replaces input if already exists
                }
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("input", input);
            return JsonConvert.SerializeObject(result, Formatting.Indented,jsonSerializerSettings);
        }

        //    /blink1/input/file -- Add and Start File watcher on given filepath
        static string Ublink1InputFile(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string fpath = request.Query.Get("arg1").Trim();
            string test = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test == null) ? false : (test.Equals("on") || test.Equals("true"));

            string statusstr = "must specifiy 'iname' and 'arg1' (filepath)";

            Blink1Input input = null;
            if (fpath != null && iname != null) {
                statusstr = "input file";
                input = new Blink1Input(blink1Server, iname, pname, "file", fpath);

                input.updateFileInput();
                if (!testmode) {
                    blink1Server.inputs[iname] = input; // NOTE: this replaces input if already exists
                }
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("input", input);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/input/script -- Add and Start command-line script executer
        static string Ublink1InputScript(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string fpath = request.Query.Get("arg1").Trim();
            string test = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test == null) ? false : (test.Equals("on") || test.Equals("true"));

            string statusstr = "must specifiy 'iname' and 'arg1' (script filepath)";

            Blink1Input input = null;
            if (fpath != null && iname != null) {
                statusstr = "input script";
                input = new Blink1Input(blink1Server, iname, pname, "script", fpath);

                input.updateScriptInput();
                if (!testmode) {
                    blink1Server.inputs[iname] = input; // NOTE: this replaces input if already exists
                }
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("input", input);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        //    /blink1/input/ifttt -- Add and Start watching messages from IFTTT webservice
        static string Ublink1InputIfttt(HttpRequest request, Blink1Server blink1Server)
        {
            string pname = request.Query.Get("pname");
            string iname = request.Query.Get("iname");
            string rulename = request.Query.Get("arg1").Trim();
            string test = request.Query.Get("test");
            if (pname == null) pname = iname;
            Boolean testmode = (test == null) ? false : (test.Equals("on") || test.Equals("true"));

            string statusstr = "must specifiy 'iname' and 'arg1' (rulename)";

            Blink1Input input = null;
            if (rulename != null && iname != null) {
                statusstr = "input ifttt";
                input = new Blink1Input(blink1Server, iname, pname, "ifttt", rulename);

                if (testmode) { // override periodic fetch for immediate fetch
                    Blink1Input.getIftttResponse(false);
                }
                input.updateIftttInput();
                if (!testmode) {
                    blink1Server.inputs[iname] = input; // NOTE: this replaces input if already exists
                }
            }
            blink1Server.saveSettings();

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", statusstr);
            result.Add("input", input);
            return JsonConvert.SerializeObject(result, Formatting.Indented, jsonSerializerSettings);
        }

        #endregion

        // ----------------------------------------------------------------------------------------
        // input update url handling
        //


        /// <summary>
        /// Periodically update the inputs, triggering color patterns if needed
        /// Runs every 15(?) seconds
        /// </summary>
        public void updateInputs(Object stateInfo)
        {
            if (!inputsEnable) return;
            Log("updateInputs");

            Blink1Input.getIftttResponse(true);

            foreach (var pair in inputs) {
                Blink1Input input = pair.Value;
                input.update();

                // FIXME: maybe do a logging of inputs that cause triggers like:
                // if( input.update() ) {
                //   alertLog.Add( Now(), input.name );
                // }
                // and then have a "/blink1/alertlog" URL
            }
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
            Log("playPattern:"+pname);
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

        public Boolean stopPattern(string pname)
        {
            if (pname == null) return false;
            Blink1Pattern patt = null;
            if (patterns.TryGetValue(pname, out patt)) {
                patt.stop();
                return true;
            }
            return false;
        }

        public void stopAllPatterns()
        {
            foreach (KeyValuePair<string, Blink1Pattern> kvp in patterns) {
                kvp.Value.stop();
            }
        }

        /// <summary>
        /// Same as Blink1.fadeToRGB, but does an open/close around it.
        /// Called by Blink1Patterns to send colors to blink(1)
        /// </summary>
        public void fadeToRGB(double secs, Color c)
        {
            Log("fadeToRGB: rgb:" + ColorTranslator.ToHtml(c) + " secs:" + secs.ToString("F2", CultureInfo.InvariantCulture));
            //blink1.open();
            blink1.fadeToRGB((int)(secs * 1000), c.R, c.G, c.B);
            //blink1.close(); 
            // FIXME: this method crashes when called successively.
            // FIXME: I think we can't close because I'm doing hid_exit() in blink1_close()
        }

        /// <summary>
        /// Shut down EVERYTHING.  Called right before app exit.
        /// </summary>
        public void shutdown()
        {
            stopAllPatterns();
            inputsEnable = false;
            inputsTimer.Change(Timeout.Infinite, Timeout.Infinite);
            inputsTimer.Dispose();
        }


        // currently unimplemented URL API calls

        //    /blink1/input/scriptlist -- List available scripts to run

        //    /blink1/input/cpuload -- Add and Start CPU load watching input

        //    /blink1/input/netload -- Start network load watching input


        //
        // ---------------------------------------------------------------------------------------
        //

        // used to log http requests to console
        void writer_OnWrite(char[] buffer, int index, int count)
        {
            if (logToScreen) {
                String text = new string(buffer, index, count);
                //Log(text);
            }
        }
        /// <summary>
        /// Summary description for ConsoleWriter.
        /// </summary>
        public class ConsoleWriter : TextWriter
        {
            public override System.Text.Encoding Encoding
            {
                get { return System.Text.Encoding.Default; }
            }
            public override void Write(char[] buffer, int index, int count)
            {
                if (OnWrite != null) OnWrite(buffer, index, count);
            }
            public override void Write(char value)
            {
                Write(new char[] { value }, 0, 1);
            }
            public delegate void WriteEventHandler(char[] buffer, int index, int count);
            public event WriteEventHandler OnWrite;
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
                    return input.ToString(); //placeholder
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

            public string ContentType { get { return "application/json"; } }
            public string Name { get { return name; } }
            public IDirectory Parent { get { return parent; } }

            public void Dispose()
            {
            }
        }


        // TESTING: this is for tod testing, trying to figure out httpwebserver class
        public class Blink1Directory : IDirectory
        {
            string name;
            IDirectory parent;
            public Blink1Directory(string name, IDirectory parent)
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
        }  //TESTING: end testing class

    }
    
}
