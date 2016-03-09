// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Blink(1) console demonstration program.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ConsoleDemo
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading;

    using ThingM.Blink1.ColorProcessor;

    /// <summary>
    ///     Blink(1) console demonstration program.
    /// </summary>
    public class Program
    {
        #region Public Methods and Operators

        /// <summary>
        /// The main program.
        /// </summary>
        /// <param name="args">
        /// The arguments.
        /// </param>
        public static void Main(string[] args)
        {
            TestCase1();
            TestCase2();
            TestCase3();
            TestCase4();
            TestCase5();
            TestCase6();
            TestCase7();
            TestCase8();

            Console.WriteLine("We're done with the demo, press any key to stop the program.");
            Console.ReadKey();
        }

        /// <summary>
        ///     Pass 1: work on the first Blink(1) device found, synchronously
        /// </summary>
        private static void TestCase1()
        {
            Console.WriteLine("####################################################################");
            Console.WriteLine("## Pass 1: work on the first Blink(1) device found, synchronously ##");
            Console.WriteLine("####################################################################");

            // Note, the first past goes so fast, Blink(1) does not have the time to, well, blink.
            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 1: Opening the first Blink(1) found.");
            blink1.Open();

            int versionNumber = blink1.GetVersion();
            Console.WriteLine("Pass 1: Blink(1) device is at version: {0}.", versionNumber.ToString());

            Console.WriteLine("Pass 1: Set Blink(1) to be RED.");
            blink1.SetColor(255, 0, 0);

            Console.WriteLine("Pass 1: Set Blink(1) to fade to BLUE over 10 seconds.");
            blink1.FadeToColor(10000, 0, 0, 255, false);
            Thread.Sleep(10000);

            Console.WriteLine("Pass 1: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 2: work on the last Blink(1) device found, asynchronously
        /// </summary>
        private static void TestCase2()
        {
            Console.WriteLine("####################################################################");
            Console.WriteLine("## Pass 2: work on the last Blink(1) device found, asynchronously ##");
            Console.WriteLine("####################################################################");

            List<string> devicePaths = Blink1Info.GetDevicePath();

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 2: Opening the last Blink(1) found via its HID path.");
            blink1.Open(devicePaths.Last());

            Console.WriteLine("Pass 2: Set Blink(1) to blink 8 times PURPLE.");
            blink1.Blink(8, 500, 200, 128, 0, 128);

            Console.WriteLine("Pass 2: Set Blink(1) to be RED, using RGB(255, 0, 0).");
            blink1.SetColor(new Rgb(255, 0, 0));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be GREEN, using CYMK(100, 0, 100, 0).");
            blink1.SetColor(new Cmyk(100, 0, 100, 0));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be BLUE, using HSB(240, 100, 100).");
            blink1.SetColor(new Hsb(240, 100, 100));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be YELLOW, using HSL(60, 100, 50).");
            blink1.SetColor(new Hsl(60, 100, 50));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be CYAN, using HtmlHexadecimal(\"#00FFFF\").");
            blink1.SetColor(new HtmlHexadecimal("#00FFFF"));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be MAGENTA, using HtmlColorName and HtmlHexadecimal.");
            blink1.SetColor(new HtmlHexadecimal(HtmlColorName.Magenta));

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to be BLACK.");
            blink1.SetColor(0, 0, 0);

            Thread.Sleep(1000);

            Console.WriteLine("Pass 2: Set Blink(1) to fade to ORANGE over 10 seconds.");
            blink1.FadeToColor(10000, 251, 61, 4, true);

            Console.WriteLine("Pass 2: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 3: reading the current presets
        /// </summary>
        private static void TestCase3()
        {
            Console.WriteLine("#########################################");
            Console.WriteLine("## Pass 3: reading the current presets ##");
            Console.WriteLine("#########################################");

            // Note, any dealing with presets is done asynchronously.
            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 3: Opening the first Blink(1) found.");
            blink1.Open();

            for (ushort position = 0; position < Blink1Constant.NumberOfPreset; position++)
            {
                Blink1Preset blink1Preset = blink1.ReadPreset(position);

                Console.WriteLine(
                    "Pass 3: Position:{0,3}, Millisecond:{1,5}, Red:{2,4}, Green:{3,4}, Blue:{4,4}, Hex: #{2:X02}{3:X02}{4:X02}", position, blink1Preset.Millisecond, blink1Preset.Rgb.Red, blink1Preset.Rgb.Green, blink1Preset.Rgb.Blue);
            }

            Console.WriteLine("Pass 3: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 4: playing the current presets
        /// </summary>
        private static void TestCase4()
        {
            Console.WriteLine("#########################################");
            Console.WriteLine("## Pass 4: playing the current presets ##");
            Console.WriteLine("#########################################");

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 4: Opening the first Blink(1) found.");
            blink1.Open();

            Console.WriteLine("Pass 4: Playing the presets for 20 seconds...");
            blink1.PlayPreset(0);
            Thread.Sleep(20000);

            Console.WriteLine("Pass 4: Stopping playing presets.");
            blink1.StopPlayingPreset();

            Console.WriteLine("Pass 4: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 5: saving new random presets
        /// </summary>
        private static void TestCase5()
        {
            Console.WriteLine("#######################################");
            Console.WriteLine("## Pass 5: saving new random presets ##");
            Console.WriteLine("#######################################");

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 5: Opening the first Blink(1) found.");
            blink1.Open();

            for (ushort position = 0; position < Blink1Constant.NumberOfPreset; position++)
            {
                Blink1Preset blink1Preset = new Blink1Preset(Convert.ToUInt16(1000 + (position * 10)), GetRandomRgbColor());

                Console.WriteLine(
                    "Pass 5: Position:{0,3}, Millisecond:{1,5}, Red:{2,4}, Green:{3,4}, Blue:{4,4}, Hex: #{2:X02}{3:X02}{4:X02}", position, blink1Preset.Millisecond, blink1Preset.Rgb.Red, blink1Preset.Rgb.Green, blink1Preset.Rgb.Blue);
                blink1.SavePreset(blink1Preset, position);
            }

            Console.WriteLine("Pass 5: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 6: reading the new random presets
        /// </summary>
        private static void TestCase6()
        {
            Console.WriteLine("############################################");
            Console.WriteLine("## Pass 6: reading the new random presets ##");
            Console.WriteLine("############################################");

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 6: Opening the first Blink(1) found.");
            blink1.Open();

            for (ushort position = 0; position < Blink1Constant.NumberOfPreset; position++)
            {
                Blink1Preset blink1Preset = blink1.ReadPreset(position);

                Console.WriteLine(
                    "Pass 6: Position:{0,3}, Millisecond:{1,5}, Red:{2,4}, Green:{3,4}, Blue:{4,4}, Hex: #{2:X02}{3:X02}{4:X02}", position, blink1Preset.Millisecond, blink1Preset.Rgb.Red, blink1Preset.Rgb.Green, blink1Preset.Rgb.Blue);
            }

            Console.WriteLine("Pass 6: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 7: playing the new random presets
        /// </summary>
        private static void TestCase7()
        {
            Console.WriteLine("############################################");
            Console.WriteLine("## Pass 7: playing the new random presets ##");
            Console.WriteLine("############################################");

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 7: Opening the first Blink(1) found.");
            blink1.Open();

            Console.WriteLine("Pass 7: Playing the presets for 20 seconds...");
            blink1.PlayPreset(0);
            Thread.Sleep(20000);

            Console.WriteLine("Pass 7: Stopping playing presets.");
            blink1.StopPlayingPreset();

            Console.WriteLine("Pass 7: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        /// <summary>
        ///     Pass 8: work with the inactivity mode
        /// </summary>
        private static void TestCase8()
        {
            Console.WriteLine("###########################################");
            Console.WriteLine("## Pass 8: work with the inactivity mode ##");
            Console.WriteLine("###########################################");

            Blink1 blink1 = new Blink1();

            Console.WriteLine("Pass 8: Opening the first Blink(1) found.");
            blink1.Open();

            Console.WriteLine("Pass 8: Writing inactivity mode presets");
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.Red)), 0);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 1);
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.White)), 2);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 3);
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.Green)), 4);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 5);
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.Blue)), 6);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 7);
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.Magenta)), 8);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 9);
            blink1.SavePreset(new Blink1Preset(500, new HtmlHexadecimal(HtmlColorName.Cyan)), 10);
            blink1.SavePreset(new Blink1Preset(250, new HtmlHexadecimal(HtmlColorName.Black)), 11);

            Console.WriteLine("Pass 8: Activating the inactivity mode for 10 seconds of inactivity.");
            blink1.ActivateInactivityMode(5000);

            Console.WriteLine("Pass 8: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine("Pass 8: After the inactivity period, Blink(1) will play its presets.");
            Thread.Sleep(15000);

            Console.WriteLine("Pass 8: Opening the first Blink(1) found.");
            blink1.Open();

            Console.WriteLine("Pass 8: Deactivating the inactivity.");
            blink1.DeactivateInactivityMode();

            Console.WriteLine("Pass 8: Closing Blink(1) connection.");
            blink1.Close();

            Console.WriteLine(Environment.NewLine);
        }

        #endregion

        #region Methods

        /// <summary>
        ///     Get a truly different seed number.
        /// </summary>
        /// <returns>
        ///     A seed number based on the DateTime.Ticks.
        /// </returns>
        private static int GetDifferentSeed()
        {
            long tickNow = DateTime.Now.Ticks;

            while (tickNow == DateTime.Now.Ticks)
            {
            }

            return (int)DateTime.Now.Ticks;
        }

        /// <summary>
        ///     Generate a random RGB color object.
        /// </summary>
        /// <returns>
        ///     A RGB color object.
        /// </returns>
        private static Rgb GetRandomRgbColor()
        {
            Random rand;

            rand = new Random(GetDifferentSeed());
            ushort red = (ushort)rand.Next(0, 255);

            rand = new Random(GetDifferentSeed());
            ushort green = (ushort)rand.Next(0, 255);

            rand = new Random(GetDifferentSeed());
            ushort blue = (ushort)rand.Next(0, 255);

            return new Rgb(red, green, blue);
        }

        #endregion
    }
}