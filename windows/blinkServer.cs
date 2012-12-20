using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;
using System.Drawing;
using System.Reflection;
using System.IO;
using System.Diagnostics;
using MiniHttpd;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Blink1Lib;

namespace blink1
{

    class blinkHttpInterface
    {

        HttpWebServer bhI = new HttpWebServer(8080);
        JTokenWriter jtw = new JTokenWriter();
        blinkServer bs = new blinkServer();
        //J bhIJW = new JsonTextWriter(new System.IO.TextWriter());
        public blinkHttpInterface()
        {
            try
            {
                VirtualDirectory root = new VirtualDirectory();
                DriveDirectory homeDir = new DriveDirectory(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
                foreach (DriveDirectory d in homeDir.GetDirectories())
                {
                    root.AddDirectory(d.Path);
                }
                VirtualDirectory blink1 = new VirtualDirectory("blink1",root);
                Blink1JSONFile id = new Blink1JSONFile("id", blink1, bs);
                id.GetStringResponse = blink1Id;
                blink1.AddFile(id);   //add a virtual file for each json method
                
                root.AddDirectory(blink1);
                bhI.Root = root;
                bhI.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public delegate string GetJSONStringResponse(HttpRequest request, blinkServer bs);

//    /blink1/id -- Display blink1_id and blink1 serial numbers (if any)

        static string blink1Id(HttpRequest request, blinkServer bs)//example
        {
            string serial = bs.getId();
            return @"{
  ""blink1_id"": ""44288083" + serial + @""",
  ""blink1_serialnums"": [
    """ + serial + @"""
  ],
  ""status"": ""blink1 id""
}";
        }

//    /blink1/regenerateblinkid -- Generate, save, and return new blink1_id

//    /blink1/enumerate -- Re-enumerate and List available blink(1) devices

//    /blink1/fadeToRGB -- Send fadeToRGB command to blink(1) with hex color & fade time

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
        

        public class Blink1JSONFile : IFile
        {
            public blinkServer bs;
            public Blink1JSONFile(string name, IDirectory parent, blinkServer bsp)
            {
                this.name = name;
                this.parent = parent;
                this.bs = bsp;
                GetStringResponse = delegate(HttpRequest input, blinkServer bsa)
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
                StreamWriter writer =
                  new StreamWriter(request.Response.ResponseContent);

                writer.WriteLine(GetStringResponse(request, bs));

                // Don't forget to flush!
                writer.Flush();
            }

            public string ContentType
            {
                get { return ContentTypes.GetExtensionType(".txt"); }
            }
            public string Name
            {
                get { return name; }
            }

            public IDirectory Parent
            {
                get { return parent; }
            }

            public void Dispose()
            {
            }
        }

    }

    public class blinkServer
    {
        Blink1 blink1;
        public blinkServer()
        {
            blink1 = new Blink1();            
        }
//        "Usage: \n"
//"  %s <cmd> [options]\n"
//"where <cmd> is one of:\n"
//"  --blink <numtimes>          Blink on/off (specify --rgb before to blink a color)\n"
//"  --random <numtimes>         Flash a number of random colors \n"
//"  --rgb <red>,<green>,<blue>  Fade to RGB value\n"
//"  --on                        Turn blink(1) full-on white \n"
//"  --red                       Turn blink(1) off \n"
//"  --green                     Turn blink(1) red \n"
//"  --blue                      Turn blink(1) green \n"
//"  --off                       Turn blink(1) blue \n"
//"  --savergb <r>,<g>,<b>,<pos> Write pattern RGB value at pos\n" 
//"  --readrgb <pos>             Read pattern RGB value at pos\n" 
//"  --play <1/0,pos>            Start playing color sequence (at pos)\n"
//"  --servertickle <1/0>        Turn on/off servertickle (uses -t msec) \n"
//"  --list                      List connected blink(1) devices \n"
//" Nerd functions: (not used normally) \n"
//"  --hidread                   Read a blink(1) USB HID GetFeature report \n"
//"  --hidwrite <listofbytes>    Write a blink(1) USB HID SetFeature report \n"
//"  --eeread <addr>             Read an EEPROM byte from blink(1)\n"
//"  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) \n"
//"  --version                   Display blink(1) firmware version \n"
//"and [options] are: \n"
//"  -g -nogamma                 Disable autogamma correction\n"
//"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
//"  -m ms,   --millis=millis    Set millisecs for color fading (default 300)\n"
//"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
//"  --vid=vid --pid=pid         Specifcy alternate USB VID & PID\n"
//"  -v, --verbose               verbose debugging msgs\n"
        /// <summary>
        /// DEPRECATED: use blink1Lib object instead where possible
        /// </summary>
        /// <param name="args"></param>
        /// <returns></returns>
        string runBlinkProcess(string args)
        {
            try
            {
                Process serverSideProcess = new Process();
                serverSideProcess.StartInfo = new ProcessStartInfo("blink1-tool.exe");
                serverSideProcess.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                serverSideProcess.StartInfo.Arguments = args;
                serverSideProcess.StartInfo.UseShellExecute = false;
                serverSideProcess.StartInfo.RedirectStandardOutput = true;
                serverSideProcess.Start();
                string output = serverSideProcess.StandardOutput.ReadToEnd();
                serverSideProcess.WaitForExit();
                if (output != null && output != "")
                {
                    return output;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            return "";
        }

        public string getId(){
            if (blink1.enumerate() > 0)
            {
                string re = blink1.getCachedSerial(0);
                return re;
            }
            else return "";
        }


        public void sendPatternToBlink1(Color[] param)
        {
            for (int i = 0; i < param.Length; i++)
            {
                Color c = param[i];
                //runBlinkProcess(string.Format("-- savergb {0} {1} {2} {3}", c.R, c.G, c.B, i));
                blink1.setPatternAtPosition(c.R, c.G, c.B, 500, i);
            }
        }

        Timer delayTrigger = new Timer();
        Timer blink1Poll = new Timer();



        public enum IFTTT
        {
        }

        public void parseIFTTT()
        {
        }



        interface json
        {
        }

        public void sendSingleColor(Color c)
        {
            blink1.setRGB(c.R, c.G, c.B);
        }




    }
}
