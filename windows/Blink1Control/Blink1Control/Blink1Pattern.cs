using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using System.Globalization;
using Blink1Lib;


namespace Blink1Control
{
    class Blink1Pattern
    {
        /// <summary>
        /// Name of the pattern 
        /// </summary>
        public string name { get; set; }

        List<Color> colors;
        List<float> times;
        Timer timer;

        /// <summary>
        /// number of repeats for this pattern (0=play forever) 
        /// </summary>
        public int repeats { get; set; }
        public int playcount { get; set; }
        /// <summary>
        /// Current playing position 
        /// </summary>
        public int playpos { get; set; }
        /// <summary>
        /// Is this pattern playing 
        /// </summary>
        public Boolean playing { get; set; } // FIXME: how to make this read-only except in this class?
        /// <summary>
        /// String representation of pattern repeats, colors & times
        /// </summary>
        public string pattern
        {
            get
            {
                List<string> sa = new List<string>();
                for (int i = 0; i < colors.Count; i++) {
                    sa.Add(ColorTranslator.ToHtml(colors[i]));
                    sa.Add(times[i].ToString("F2", CultureInfo.InvariantCulture));
                }
                return String.Join(",", sa);
            }
        }
        
        public Blink1Server blink1Server { get; set; } //


        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="aName"></param>
        /// <param name="patternstr"></param>
        public Blink1Pattern(string aName, string patternstr)
        {
            name = aName;
            playpos = 0;
            playcount = 0;
            playing = false;
            parsePatternStr(patternstr);  // FIXME: check for bad patternstr vals
        }

        /// <summary>
        /// Parse a pattern string, setting internal vars as needed
        /// </summary>
        /// <param name="patternstr">patternstr format "repeats,color1,color1time,color2,c2time,..."</param>
        /// <returns>true if parsing worked, false otherwise</returns>
        public Boolean parsePatternStr(string patternstr)
        {
            colors = new List<Color>();
            times = new List<float>();
            string[] values = patternstr.Split(',');
            repeats = 0;
            try { repeats = int.Parse(values[0]); }
            catch (Exception e) { Console.WriteLine(e.ToString()); }
            int len2x = values.Length - 1;
            if ((len2x % 2) == 0)
            {  // even number, so good
                for (int i = 0; i < len2x; i += 2)
                {
                    Color colr = Color.Black;
                    float secs = 0.1F;
                    try {
                        string rgbstr = values[1 + i + 0];
                        string secstr = values[1 + i + 1];
                        colr = ColorTranslator.FromHtml(rgbstr);
                        secs = float.Parse(secstr, CultureInfo.InvariantCulture);
                    }
                    catch (Exception e) { Console.WriteLine(e.ToString()); }
                    colors.Add(colr);
                    times.Add(secs);
                }
            }
            else { return false; }
            return true;
        }

        /// <summary>
        /// Start a pattern playing
        /// </summary>
        public void play()
        {
            playpos = 0;
            playcount = 0;
            playing = true;

            Color colr = colors[playpos];
            float nextTime = times[playpos];

            float fadeTime = nextTime / 2;
            blink1Server.fadeToRGB(fadeTime, colr);

            timer = new Timer( update, null, (int)(nextTime*1000), 0);
        }

        /// <summary>
        /// Called whenever a new color needs to be played, via timer
        /// </summary>
        public void update(Object stateInfo)
        {
            if (!playing) return;
            Console.WriteLine("update! "+name);

            Boolean scheduleNext = true;
            playpos++;
            if( playpos == times.Count() ) {
                playpos = 0;
                if( repeats != 0 ) {  // infinite
                    playcount++;
                    if( playcount == repeats ) {
                        scheduleNext = false;
                    }
                }
            }
    
            if( scheduleNext ) {
                float nextTime = times[playpos];
                Color color = colors[playpos];
                Console.WriteLine("update: scheduleNext: "+nextTime);
                //DLog(@"%@ updt p:%d c:%d %@ nextTime:%f",name,playpos,playcount,[Blink1 hexStringFromColor:color],nextTime);
                blink1Server.fadeToRGB(nextTime/2, color);
                timer = new Timer( update, null, (int)(nextTime*1000), 0);
            } else {
                playing = false;
            }
        }

        /// <summary>
        /// Stop a pattern playing
        /// </summary>
        public void stop()
        {
            playing = false;
        }


        public override string ToString()
        {
            return "{name:"+name+",pattern:"+pattern+",repeats:"+repeats+",playing:"+playing+"}";
        }

    }
}
