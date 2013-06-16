

Blink1Control Application Requirements
======================================


1. Introduction 
---------------

### 1.1. Background

Blink1Control is an application used to mediate between events on the Net and the 
user's computer and the blink(1) USB RGB LED notification device.  When an event
important to the user occur, the blink(1) plays a color pattern of the user's choosing.

The original release of Blink1Control had a uniform UI across Mac & Windows platforms.
This UI was written in HTML5/Javascript and the underlying platform-specific local
HTTP server and application logic was written in Cocoa and .NET, respectively.
Managing three separate codebases (HTML,Cocoa,.NET) is problematic, and it's hoped that
a unified codebase in Qt will make future updates easier and faster to accomplish.

### 1.2. General Description

The application provides an interface for the user to create and manage different
data "inputs" and color "patterns".  Inputs can be any data source that is periodically 
evaluated and when an evaluation criteria is met, a corresponding color pattern is
played on a blink(1) device.

### 1.3. Previous implementations

There are two previous implementations of Blink1Control: one written in Cocoa, 
one written in .NET.  The .NET one came later and is slightly better organized.  
Either can be used to crib from.  But note that while the Qt version of Blink1Control
will support a local HTTP server, the GUI is not in HTML5/Javascript and is instead 
created with normal Qt widgets. 


2. Cross-platform Targets
-------------------------

### 2.1. Mac OS X 
Minimum OS version requirement is Mac OS X 10.7.  
It is strongly preferred that it also work on 10.6.

### 2.2. Windows
Minimum OS version requirement is Windows Vista.
It is strongly preferred that it also work on Windows XP.

### 2.3. Linux 
The overal design should not preclude the ability to be ported to Linux.


3. Functional Requirements
--------------------------

### 3.1. Application settings
The application maintains a collection of settings of the current state of the app.

3.1.1. Load application settings from disk on app start.

3.1.2. Save application settings to disk on any settings change.


### 3.2. Color Patterns
A color pattern (or just "pattern") is a list of RGB color, millisecond time tuples.  
A pattern has additional meta data such as: pattern name, numbers of repeats, currently playing, number of times played.  

Example color patterns are shown in [app-url-api.md](app-url-api.md)

The pattern subsystem must:

3.2.1. Maintain a user-editable map of named color patterns.

3.2.2. Save color pattern map as application setting.

3.2.3. Load color pattern map from application setting.

3.2.4. A color pattern may be started playing at any time, by name.
Update resolution for color patterns is 50 milliseconds.

3.2.5. Multiple color patterns may be played concurrently.

3.2.6. A color pattern may be stopped at any time, by name.

3.2.7. *** Special color patterns for real-time mapping?


### 3.3. Data Inputs
A data input (or just "input") is a data source that is periodically queried.
When an input's matching criteria is met, it triggers a color pattern to play.

Example input types are: 

* IFTT JSON data source 
* arbitrary URL JSON data source
* local text file data source, 
* local script/executable output.

A pattern has additional meta data such as: 

* input name
* pattern name to trigger on match
* list of input-specific user-supplied arguments (IFTTT tag, JSON URL, etc.)
* list of input-specific parameters (e.g. input-specific update rate, IFTTT URL)
 

When the user-specified /

The input subsystem must:

3.3.1. Maintain a user-editable map of named inputs.
3.3.2. Save input map as application setting.
3.3.3. Load color pattern map from application setting.
3.3.4. Periodically, an input is evaluated (e.g. URL fetched & parsed) and if 


### 3.4. blink(1) library interface
Use the C-based "blink1-lib" for all communication with blink(1) devices.

3.4.1. Query for available blink(1) devices.
3.4.2. Connect to first available blink(1) device.
3.4.3. Query blink(1) serial number to be displayed in GUI.
3.4.4. Send color commands to blink(1) device.

3.4.5. As the blink1-lib library doesn't do mutex, proper mutex of access to blink1-lib will be enforced.




3.4.1.  Maintain a named Map of trigger Inputs
- Patterns are
- Inputs are



4. Interface Requirements
-------------------------

4.1. "Virtual blink(1)" display widget shows current color state of real blink(1) device
 


5. Performance Requirements

6. Other Non-Functional Requirements

7. Example Use Case Scenarios




(format stolen from: http://www.radford.edu/waconley/Requirements.pdf)

------------



4. Application logic

5. GUI and Design



Features
--------
- blink(1) library interface
- App logic
- GUI & Design
- Notification icon Menubar (Mac OS X) / Tray Icon (Windows)
- Local HTTP server 


blink(1) library interface
---------------------------
- Query for available blink(1) devices
- Connect to first available blink(1) device
- Query blink(1) serial number
- Send color commands to blink(1) device


App logic
----------
- Load application settings from disk on app start
- Save application settings to disk on any settings change
- Maintain a named Map of color Patterns 
- Maintain a named Map of trigger Inputs
- Patterns are
- Inputs are


Trigger Inputs
--------------


GUI & Design
------------
- Primary application font is Helvetica Neue 
- uniform UI
- custom color picker


Menu / Tray Icon
----------------
- Open main gui
- Shut down app



HTTP Server
-----------
- Local HTTP server running on port 8934
- Responds to URIs as described in url-api.md

