using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Blink1Lib
{
    public class Blink1
    {
        private readonly object lock_ = new object();

        /// <summary>
        /// Returns the blink(1) uid (aka IFTTT key) 
        /// </summary>
        /// <returns>blink1_uid</returns>
        public string blink1Id { get; private set; }
        /// <summary>
        /// hostid is a component of the blink1Id (aka IFTT key) 
        /// </summary>
        public string hostId { get; set; }
        /// <summary>
        /// 
        /// </summary>
        public Color lastColor { get; private set; }

        public string lastColorString { get { return Blink1.colorToHexCode(lastColor); } }

        static void Main(string[] args)
        {
        }

        /// <summary>
        /// Library for communicating with blink(1) USB RGB LEDs.
        /// On instantiation, enumerates the USB and records any plugged in blink(1) devices. 
        /// </summary>
        public Blink1()
        {
            enumerate();
            lastColor = Color.Black;
        }

        /// <summary>
        // Create a blink1_id (aka "IFTTT Key" and other names)
        // If no blink1 device is present, create a fake blink1_id with zerod serial
        /// </summary>
        /// <returns>blink1_id</returns>
        public string regenerateBlink1Id()
        {
            if (hostId == null || hostId.Equals("00000000") )
            {
                Random rand = new Random();
                int rval = rand.Next();
                hostId = rval.ToString("X8");
            }
            string blink1_serial = "00000000"; // 8-digit hexnum
            if (getCachedCount() > 0 )
            {
                blink1_serial = getCachedSerial(0);
            }
            blink1Id = hostId + blink1_serial;
            return blink1Id;
        }

        /// <summary>
        /// Open blink(1) device for use.  If multiple blink(1)s, opens the first device. use OpenById for a specific device
        /// </summary>
        /// <returns>true if device was opened, false if no device available.</returns>
        public Boolean open()
        {
            lock (lock_) {
                dev = blink1_open();
            }
            if (dev == null) return false;
            return true;
        }

        /// <summary>
        /// Open a particular blink(1) device, if there are several
        /// </summary>
        /// <param name="i">integer id (0,1,2...) of device to open</param>
        /// <returns>true if device was opened, false otherwise</returns>
        public Boolean openById(int i)
        {
            lock (lock_) {
                dev = blink1_openById(i);
            }
            if (dev == null) return false;
            return true;
        }

        /// <summary>
        /// Close an open blink(1) device.
        /// </summary>
        public void close()
        {
            lock (lock_) {
                blink1_close(dev);
            }
        }

        /// <summary>
        /// Set blink(1) to RGB color immediately.
        /// </summary>
        /// <param name="r">red component</param>
        /// <param name="g">green component</param>
        /// <param name="b">blue component</param>
        public void setRGB(int r, int g, int b)
        {
            lastColor = Color.FromArgb(r, g, b);
            lock (lock_) {
                blink1_setRGB(dev, r, g, b);
            }
        }

        /// <summary>
        /// Fade to an RGB color over a period of milliseconds.
        /// </summary>
        /// <param name="millis">milliseconds fade time</param>
        /// <param name="r">red component</param>
        /// <param name="g">green component</param>
        /// <param name="b">blue component</param>
        public void fadeToRGB(int millis, int r, int g, int b)
        {
            lastColor = Color.FromArgb(r, g, b);
            lock (lock_) {
                blink1_fadeToRGB(dev, millis, r, g, b);
            }
        }

        /// <summary>
        /// Convert a Color to a HTML hex code (e.g."#FFCC33")
        ///
        /// </summary>
        /// <param name="c">color to convert</param>
        /// <returns>hex code string</returns>
        public static String colorToHexCode(Color c)
        {
            // stolen from: http://stackoverflow.com/questions/2395438/convert-system-drawing-color-to-rgb-and-hex-value
            // because ColorTranslator.ToHtml() can return color names (e.g. "Blue")
            return "#" + c.R.ToString("X2") + c.G.ToString("X2") + c.B.ToString("X2");
        }

        /// <summary>
        /// FIXME: make more like Blink1.m enumerate
        /// </summary>
        /// <returns>number of blink1 devices</returns>
        public int enumerate()
        {
            int n = 0;
            lock (lock_) {
                n = blink1_enumerate();
            }
            return n;
        }

        /// <summary>
        /// get count of cached serial numbers of currently connected blink(1) devices
        /// </summary>
        /// <returns>number of blink(1) devices</returns>
        public int getCachedCount()
        {
           return blink1_getCachedCount();
        }

        /// <summary>
        /// get serial number of blink1 device at index
        /// </summary>
        /// <param name="i"></param>
        /// <returns></returns>
        public string getCachedSerial(int i)
        {
            IntPtr wcstr = blink1_getCachedSerial(i);
            string result = Marshal.PtrToStringAuto(wcstr);
            return result;
        }

        public bool setPatternAtPosition(int r, int g, int b, int millis, int pos)
        {
            int ret = -1;
            lock (lock_) {
                blink1_writePatternLine(dev, millis, r, g, b, pos);
            }
            if (ret > -1) {
                return true;
            }
            return false;
        }

        // see: http://msdn.microsoft.com/en-us/magazine/cc164123.aspx#S7
        // and: http://msdn.microsoft.com/en-us/library/aa446532.aspx
        // and: http://www.codeguru.com/csharp/csharp/cs_data/article.php/c4217/Calling-Unmanaged-Code-Part-1--simple-DLLImport.htm

        private const string DllName = "blink1-lib.dll";
        /// <summary>
        /// pointer for last device opened
        /// </summary>
        private System.IntPtr dev;

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern System.IntPtr blink1_open();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern System.IntPtr blink1_openById(int i);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void blink1_close(System.IntPtr dev);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern System.IntPtr blink1_setRGB(System.IntPtr dev, int r, int g, int b);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern System.IntPtr blink1_fadeToRGB(System.IntPtr dev, int millis, int r, int g, int b);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int blink1_enumerate();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int blink1_getCachedCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr blink1_getCachedSerial(int i);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int blink1_play(System.IntPtr dev, int play, int pos);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int blink1_writePatternLine(System.IntPtr dev, int fadeMillis,
                                    int r, int g, int b,
                                    int pos);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int blink1_readPatternLine(System.IntPtr dev, ref int fadeMillis,
                                   ref int r, ref int g, ref int b,
                                   int pos);

    }
}
