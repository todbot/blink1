Blink1Control Windows application for blink(1) 
==============================================


To build, open solution file 'Blink1Control/Blink1Control.sln' in Visual Studio Desktop Express 2012.

All dependencies are included in this directory.

Local webserver URL API:  

See the document 'blink1/app-url-api.md'


Open Source Software used in this application
=============================================

In general, the software packages are copied wholesale, complete with their respective licensing.

- blink1-lib - C-library to talking to blink(1), wraps HIDAPI
-- http://github.com/todbot/blink1

- CefSharp - .NET binding for Chromium Embedded Framework
-- https://github.com/chillitom/CefSharp

- MiniHttpd - stand-alone HTTP web server library
-- http://www.codeproject.com/Articles/11342/MiniHttpd-an-HTTP-web-server-library

- Json.NET - JSON framework for .NET
-- http://json.codeplex.com/


Compilation Notes
=================

Blink1Control currently targets x86 and .NET Framework 4.5.  
There may be a way of downgrading to .NET 4.0, but I don't know how to do it.
The x86 dependency is because of the CefSharp library, which was compiled x86.



Packaging Steps (notes to tod)
===============
% In VS2012: Build
% cd blink1/windows/Blink1Control/Blink1Control/bin/x86/Debug
% rm *xml *pdb *application *manifest
% cd .. 
% rm -rf Blink1Control # (in case already exists)
% mv Debug Blink1Control 
% zip -r Blink1Control-win-0.9.5.zip Blink1Control
