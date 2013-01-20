Blink1Lib -- .NET C# library for blink(1) USB RGB LED
=====================================================
(part of the blink(1) project: https://github.com/todbot/blink1/ )


This is a very simple wrapper of the blink1-lib C library.
Thus, it requires a blink1-lib.dll (included in the Blink1Lib project).

The solution in this directory contains two projects:

- Blink1Lib -- project for the actual C# .NET library
- Blink1LibDemo -- project for small commandline C# app showing use of Blink1Lib

To use this library in your own code, copy the Blink1Lib directory to your project
and add it as a reference in your application's solution.


Blink1Lib Example Usage:
------------------------
            Console.WriteLine("------------Blink1LibDemo Begin-------------");
            Blink1 blink1 = new Blink1();

            int count = blink1.enumerate(); 
            Console.WriteLine("detected " + count + " blink(1 devices");

            if (count != 0)
            {
                string serialnum = blink1.getCachedSerial(0);
                Console.WriteLine("blink(1) serial number: " + serialnum);
            }

            blink1.open();

            Console.WriteLine("setting white");
            blink1.setRGB(255, 255, 255);
            Thread.Sleep(1000);

            Console.WriteLine("fading cyan");
            blink1.fadeToRGB(500, 0, 255, 255);
            Thread.Sleep(1000);

            Console.WriteLine("fading red");
            blink1.fadeToRGB(500, 255, 0, 0);
            Thread.Sleep(1000);

            Console.WriteLine("fading black");
            blink1.fadeToRGB(1000, 0, 0, 0);

            Console.WriteLine("------------Blink1LibDemo End --------------");
            Thread.Sleep(3000);


    





