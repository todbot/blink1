Windows application for blink(1) 
================================


To build, open solution file 'Blink1Control/Blink1Control.sln' in Visual Studio Desktop Express 2012

All dependencies are included in this directory.

Local webserver URL API:

See the document 'blink1/app-url-api.md'


Open Source Software used in this application
---------------------------------------------

In general, the software packages are copied wholesale, complete with their respective licensing.

- blink1-lib - C-library to talking to blink(1), wraps HIDAPI
-- http://github.com/todbot/blink1

- MiniHttpd - stand-alone HTTP web server library
-- http://www.codeproject.com/Articles/11342/MiniHttpd-an-HTTP-web-server-library

- Json.NET - JSON framework for .NET
-- http://json.codeplex.com/





old notes from justin 
---------------------

references a little squirrely, I need to go through this tomorrow and clean up. Using awesomium for now, until openwebkitsharp can be fixed. I'd stay with awesonium but I'm suspicious of their licensing and business model, I'd hate to get locked into something. gitignore file didn't want to play so build and debug files included for now. httplistener seems like an ok server, but I need to build out the event interaction. 


at the moment this project contains far too much, I need to go through here and cut down the references. 

we need a real .ico file, something that is small enough to fit but can be recognized. 

not 100% sure about compatibility, I've been testing on windows 7 and very updated .NET references. everything should be internal to the produced binary, but I need to dig out my old windows images and start running this on some virtual machines. 

better context menus for sure. 

fix the references to the basic library of blink1 interaction, we are missing some moves that look like they should work. 

I used WPF because it's the road ahead, but the style system requires some time cranking, it's UGLY depending on your desktop theme.

no idea about windows 8 at all. (compatibility mode should work fine, need to find an image of it somewhere)