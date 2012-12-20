using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Blink1Lib
{
    public class Blink1
    {
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
        }

        /// <summary>
        /// Open blink(1) device for use.  If multiple blink(1)s, opens the first device. use OpenById for a specific device
        /// </summary>
        /// <returns>true if device was opened, false if no device available.</returns>
        public Boolean open()
        {
            dev = blink1_open();
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
            dev = blink1_openById(i);
            if (dev == null) return false;
            return true;
        }

        /// <summary>
        /// Close an open blink(1) device.
        /// </summary>
        public void close()
        {
            blink1_close(dev);
        }

        /// <summary>
        /// Set blink(1) to RGB color immediately.
        /// </summary>
        /// <param name="r">red component</param>
        /// <param name="g">green component</param>
        /// <param name="b">blue component</param>
        public void setRGB(int r, int g, int b)
        {
            blink1_setRGB(dev, r, g, b);
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
            blink1_fadeToRGB(dev, millis, r, g, b);
        }

        /// <summary>
        /// FIXME: make more like Blink1.m enumerate
        /// </summary>
        /// <returns>number of blink1 devices</returns>
        public int enumerate()
        {
            return blink1_enumerate();
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
            int ret = blink1_writePatternLine(dev, millis, r, g, b, pos);
            if (ret > -1)
            {
                return true;
            }
            else
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
