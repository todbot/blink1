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
using MiniHttpd;
using Newtonsoft.Json;
//using Newtonsoft.Json.Linq;
using Blink1Lib;

namespace Blink1Control
{

    class blink1HttpInterface
    {
        HttpWebServer bhI = new HttpWebServer(8934);
        //JTokenWriter jtw = new JTokenWriter();
        Blink1 blink1 = new Blink1();

        //J bhIJW = new JsonTextWriter(new System.IO.TextWriter());
        public blink1HttpInterface()
        {
            blink1.hostId = (string) Properties.Settings.Default["hostId"];
            blink1.regenerateBlink1Id();
            blink1.open();

            try
            {
                VirtualDirectory root = new VirtualDirectory();
                DriveDirectory homeDir = new DriveDirectory(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
                foreach (DriveDirectory d in homeDir.GetDirectories())
                {
                    root.AddDirectory(d.Path);
                }
                VirtualDirectory blink1dir = new VirtualDirectory("blink1",root);

                // FIXME: the below is completely gross, how to do HTTP routing in .NET?
                Blink1JSONFile id        = new Blink1JSONFile("id", blink1dir, blink1);
                id.GetStringResponse = blink1Id;
                blink1dir.AddFile(id);   //add a virtual file for each json method

                Blink1JSONFile enumerate = new Blink1JSONFile("enumerate", blink1dir, blink1);
                enumerate.GetStringResponse = blink1Enumerate;
                blink1dir.AddFile(enumerate);   //add a virtual file for each json method

                Blink1JSONFile regen     = new Blink1JSONFile("regenerateblink1id", blink1dir, blink1);
                regen.GetStringResponse  = blink1RegenerateBlink1Id;
                blink1dir.AddFile(regen);

                Blink1JSONFile fadeToRGB = new Blink1JSONFile("fadeToRGB", blink1dir, blink1);
                fadeToRGB.GetStringResponse = blink1FadeToRGB;
                blink1dir.AddFile(fadeToRGB);

                Blink1JSONFile id2       = new Blink1JSONFile("id2", blink1dir, blink1);
                id2.GetStringResponse = blink1Id2;
                blink1dir.AddFile(id2);   //add a virtual file for each json method

                root.AddDirectory(blink1dir);
                bhI.Root = root;
                bhI.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public delegate string GetJSONStringResponse(HttpRequest request, Blink1 aBlink1);

        //    /blink1/id -- Display blink1_id and blink1 serial numbers (if any)
        static string blink1Id(HttpRequest request, Blink1 blink1)
        {
            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("blink1_id", blink1.getBlink1Id());
            List<string> serialnums = new List<string>();
            for( int i=0; i < blink1.getCachedCount(); i++ ) {  // FIXME: surely a smarter way to do this
                serialnums.Add( blink1.getCachedSerial(i) );
            }
            result.Add("blink1_serialnums", serialnums);
            result.Add("status", "blink1 id");
            string resultstr = JsonConvert.SerializeObject(result, Formatting.Indented);
            return resultstr;
        }

        //    /blink1/enumerate -- Re-enumerate and List available blink(1) devices
        static string blink1Enumerate(HttpRequest request, Blink1 blink1)
        {
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
            string resultstr = JsonConvert.SerializeObject(result, Formatting.Indented);
            return resultstr;
        }

        //    /blink1/regenerateblinkid -- Generate, save, and return new blink1_id
        static string blink1RegenerateBlink1Id(HttpRequest request, Blink1 blink1)
        {
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
            string resultstr = JsonConvert.SerializeObject(result, Formatting.Indented);
            return resultstr;
        }

        //    /blink1/fadeToRGB -- Send fadeToRGB command to blink(1) with hex color & fade time
        static string blink1FadeToRGB(HttpRequest request, Blink1 blink1)
        {
            // FIXME: stop pattern player
            NameValueCollection query = request.Query;
            string rgbstr  = query.Get("rgb");
            string timestr = query.Get("time");
            Color colr = System.Drawing.ColorTranslator.FromHtml(rgbstr);
            float secs = float.Parse(timestr, CultureInfo.InvariantCulture);

            blink1.fadeToRGB((int)(secs*1000), colr.R, colr.G, colr.B);

            Dictionary<string, object> result = new Dictionary<string, object>();
            result.Add("status", "fadeToRGB");
            result.Add("rgb", System.Drawing.ColorTranslator.ToHtml(colr));
            result.Add("time", secs.ToString("F3", CultureInfo.InvariantCulture));
            string resultstr = JsonConvert.SerializeObject(result, Formatting.Indented);
            return resultstr;
 
        }


//    /blink1/on -- Stop pattern playback and send fadeToRGB command to blink(1) with #FFFFFF & 0.1 sec fade time

//    /blink1/off -- Stop pattern playback and send fadeToRGB command to blink(1) with #000000 & 0.1 sec fade time

//    /blink1/lastColor -- Return the last color command sent to blink(1)

//Input Selection  //interface methods to patterns and watchers

//    /blink1/input/ -- List configured inputs, enable or disable input watching

//    /blink1/input/del -- Remove a configured input

//    /blink1/input/delall -- Remove all configured inputs

//    /blink1/input/ifttt -- Add and Start watching messages from IFTTT webservice

//    /blink1/input/file -- Add and Start file watcher on given filepath

//    /blink1/input/url -- Add and Start URL watcher on given URL

//    /blink1/input/script -- Add and Start command-line script executer

//    /blink1/input/scriptlist -- List available scripts to run

//    /blink1/input/cpuload -- Add and Start CPU load watching input

//    /blink1/input/netload -- Start network load watching input

//Color Patterns //patterns

//    /blink1/pattern/ -- List saved color patterns

//    /blink1/pattern/add -- Add color pattern to color pattern list

//    /blink1/pattern/del -- Remove color pattern from color pattern list

//    /blink1/pattern/delall -- Remove all color patterns from color pattern list

//    /blink1/pattern/play -- Play/test a specific color pattern

//    /blink1/pattern/stop -- Stop a pattern playback, for a given pattern or all patterns


        static string blink1Id2(HttpRequest request, Blink1 blink1)//example
        {
            Properties.Settings.Default["blink1Id"] = blink1.getBlink1Id();
            Properties.Settings.Default.Save();
            //Settings.Default["blink1Id"] = blink1.getBlink1Id();
            //Settings.Default.Save(); // Saves settings in application configuration file
            return @"\n\n{ suck it }\n\n";
        }


        public class Blink1JSONFile : IFile
        {
            public Blink1 blink1;
            public Blink1JSONFile(string name, IDirectory parent, Blink1 aBlink1)
            {
                this.name = name;
                this.parent = parent;
                this.blink1 = aBlink1;
                GetStringResponse = delegate(HttpRequest input, Blink1 ab)
                {
                    return input.ToString();
                };
            }

            string name;
            IDirectory parent;

            public GetJSONStringResponse GetStringResponse;

            public void OnFileRequested(HttpRequest request,
                                        IDirectory directory)
            {
                // Assign a MemoryStream to hold the response content.
                request.Response.ResponseContent = new MemoryStream();
                // Create a StreamWriter to which we
                // can write some text, and write to it.
                StreamWriter writer = new StreamWriter(request.Response.ResponseContent);
                writer.WriteLine(GetStringResponse(request, blink1));
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
