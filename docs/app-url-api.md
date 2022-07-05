 
URL API for blink(1) Applications
=================================
version 1.0 -- 20220105 -- Tod Kurt

The application that controls a blink(1) device is comprised of a GUI 
containing an embedded webserver.  The embedded webserver has this API
to let you control blink(1) devices via HTTP REST API.

The ~~strikeout text~~ are API endpoints that existed in the original app
but not in the current app, mostly for security reasons.

In this document:

* Architecture
* Internal Architecture
* URL Summary
* URL Command Reference


## Architecture ##############################################################

### Overview ###
The application maintains a list of inputs from which it reads.
An input emits a color pattern name when its input condition is met.
A color pattern is a list of color commands for blink(1).
The application maintains a list of color patterns.
The application runs one or more color patterns when triggered by inputs.
There is a user-defined mapping of input source to color pattern.

### Color Patterns ###
A color pattern is a named list of RGB colors and times between colors.
A color pattern can also specify an optional number of repeats to perform.
These colors are scheduled and sent to blink(1) based on the given timing.
There are multiple color patterns that can play on a blink(1) simultaneously.

<s>
 
### Inputs ###
There are multiple kinds of data input sources: text files, URL responses, etc.
Each configured input is given a unique human-readable input name.
Some inputs have a configurable update rate.
When an input is evaluated, it produces as a color pattern name as output.
Depending on the input, generation of color pattern names may happen external to the application (e.g. URL fetch) or internal (e.g. CPU load setpoint).
The color pattern name does not need to previously exist the color pattern list.

</s>


### File Formats ###
In general, files emitted or parsed are text files with JSON data structures.  
The keys in the JSON struct depends on the application.
For command responses, there is typically the "status" key with a value describing how an operation performed. 
One exception to the JSON rule, is the simple "RGB hex string" for files and URLs fetching.  These files are usually single-line and the first occurrence of `/#XXXXXX/` is parsed as a hex RGB color.

### File Format Examples ###

__Standard JSON response for `/blink1/fadeToRGB?rgb=$230000FF&time=5.0`:__

    {
      "rgb": "#0000ff",
      "time": "5.000"
      "status": "fadeToRGB: no blink(1) detected",
    }

__File with color pattern name:__

    { 
      "pattern":"blink3_red"
    }

__IFTTT response:__

    {
      "events":[
        {
          "blink1_id":"4428808301AA1A23",
          "name":"todpoliceflash",
          "source":"gmail",
          "date":"1348631098"
        },
      ],
      "event_count":1,
      "status" : "ok"
    }

__File with RGB hex string:__

    {
      "rgb":"#FF0023"
    }

__File with RGB hex string:__

    color: #FF2233

__File with RGB hex string:__

    #CCFF00


## Internal Architecture ####################################################

### Mapping Inputs to Color Patterns ###

A color pattern is identified by its "pname", or pattern name.  
This is its unique key into the color pattern dictionary.
Similarly, an input is uniquely identified by its "iname", or input name.
These names are used purely as identifies and can be arbitrary strings.

Normally (and expected by the current HTML front-end) an "iname" and "pname" must match,
for those inputs that work by triggering a color pattern.  
When configuring an input, if a "pname" is not specified for an input, 
it is assumed to be the same as the "iname".


### IFTTT Watcher ###

The IFTTT Watcher pulls queued IFTTT events for a particular "blink1_id" from api.thingm.com/blink1. 
The watcher runs every 15 seconds, and keeps track of the timestamp of the last seen event.
It uses the timestamp to determine if a new event of the same name has occurred.

IFTTT events are identified by a "name" (IFTTT rule name) and an optional "source" (IFTTT trigger source).
The expected operation of the IFTTT functionality is:

* On IFTTT.com, user configures the blink(1) channel with their "blink_id".

* On IFTTT.com, user creates a new recipe with blink(1) as the action. 

* The name of the user's recipe or user-chosen text string becomes the "name" of the event seen by the IFTTT Watcher.

* In blink(1) control app, user creates a new IFTTT trigger with "rule name" set to the same as on IFTTT.com.

* When the action is triggered, IFTTT.com sends event to IFTTT reflector at api.thingm.com, with "name" and "source".

* User's blink(1) control application's IFTTT Watcher queries api.thingm.com every 15 seconds 
for new events on user's blink1_id.

* When new event is seen, IFTTT Watcher plays color pattern "pname" bound to the configured input.


### URL Watcher ###

These URLs contain RGB hex strings or color pattern names in simple text files or JSON files.
URLs are scanned for changes every 30 seconds, or a configurable duration.


### File Watcher ###

There can be at most 5 tracked files.  
These files contain RGB hex strings or color pattern names in simple text files or JSON files.
Files are scanned every 15 seconds for changes, or a configurable duration.


### Script Executer ###

All scripts must live in the "blink-scripts" folder in the user's "Documents" folder.
Scripts are executed every 15 seconds, or configurable duration.  Output is parsed like files.


### Color Pattern Player ###

Color patterns are updated on the blink(1) device at a maximum rate of 100msec.
Multiple color patterns may be active simultaneously.
The color pattern player does not need to reconcile collisions between
color patterns, but instead just sends the color commands.


## URL Summary ##############################################################

Base URL: `http://localhost:8934/blink1`


### Direct blink(1) control ###

* `/blink1/id`
-- Display blink1_id and blink1 serial numbers (if any)

* `/blink1/regenerateblinkid`
-- Generate, save, and return new blink1_id

* `/blink1/enumerate` 
-- Re-enumerate and List available blink(1) devices

* `/blink1/fadeToRGB` 
-- Send fadeToRGB command to blink(1) with hex color & fade time

* `/blink1/on` 
-- Stop pattern playback and send fadeToRGB command to blink(1) with #FFFFFF & 0.1 sec fade time

* `/blink1/off` 
-- Stop pattern playback and send fadeToRGB command to blink(1) with #000000 & 0.1 sec fade time

* `/blink1/lastColor`
-- Return the last color command sent to blink(1)

<s>
 
* `/blink1/logging`
-- Enable or disable logging (Windows only currently)

</s>
 
### Color Patterns ###

* `/blink1/patterns`
-- List saved color patterns

* `/blink1/pattern/add`
-- Add color pattern to color pattern list

* `/blink1/pattern/del`
-- Remove color pattern from color pattern list

* `/blink1/pattern/delall`
-- Remove all color patterns from color pattern list

* `/blink1/pattern/play`
-- Play/test a specific color pattern

* `/blink1/pattern/stop`
-- Stop a pattern playback, for a given pattern or all patterns

<s>

### Input Selection ###

* `/blink1/inputs` 
-- List configured inputs, enable or disable input watching

* `/blink1/input/del` 
-- Remove a configured input

* `/blink1/input/delall` 
-- Remove all configured inputs

* `/blink1/input/ifttt` 
-- Add and Start watching messages from IFTTT webservice

* `/blink1/input/file` 
-- Add and Start file watcher on given filepath

* `/blink1/input/url` 
-- Add and Start URL watcher on given URL

* `/blink1/input/script` 
-- Add and Start command-line script executer

* `/blink1/input/scriptlist` 
-- List available scripts to run

* `/blink1/input/cpuload` 
-- Add and Start CPU load watching input

* `/blink1/input/netload`
-- Start network load watching input

</s>



## URL Command Reference  ###################################################

`/blink1/id`
------------
__Description:__ List blink(1) devices.
Also show "blink1_id" used as identifier with IFTTT web service.

__Query args:__ -none-

__Example:__
`http://localhost:8934/blink1/id`

__Response:__ 

With no blink(1):

    {
      "blink1_id": "3D52B69200000000",
      "blink1_serialnums": [
      ],
      "status": "blink1 id"
    }

With blink(1) plugged in:

    {
      "blink1_id": "3D52B69201AA1A23",
      "blink1_serialnums": [
        "01AA1A23"
      ],
      "status": "blink1 id"
    }

<s>
 
`/blink1/regenerateblink1id`
----------
__Description:__ Regenerate the blink1_id based on current blink(1) device.

__Example:__

`http://localhost:8934/blink1/regenerateblink1id`

__Response:__ 

    {
      "blink1_id": "3D52B69201AA1A23",
      "blink1_id_old": "3D52B69200000000",
      "blink1_serialnums": [
        "01AA1A23"
      ],
      "status": "regenerateid"
    }
</s>

`/blink1/enumerate`
-----------------
__Description:__ Re-enumerate and List available blink(1) devices.

__Query args:__ -none-

__Example:__

`http://localhost:8934/blink1/enumerate`

__Response:__ 

    {
      "blink1_id": "3D52B69201AA1A23",
      "blink1_id_old": "3D52B69200000000",
      "blink1_serialnums": [
        "01AA1A23"
      ],
      "status": "enumerate"
    }


`/blink1/fadeToRGB`
-----------------
__Description:__ Send fadeToRGB command to blink(1) with hex color & fade time

__Query args:__

* `rgb` : hex RGB color (e.g. "#ff00ff")
* `time` : time in seconds to complete fade (e.g. "0.8")
* `id`: blink1 serial number (aka "blink1_id") of blink(1) device to control
* `ledn`: LED number of a blink(1) to control. 0=all, 1=LED A, 2=LED B

__Example:__

`http://localhost:8934/blink1/fadeToRGB?rgb=%23FF00FF&time=2.7`

__Response:__ 

    {
      "rgb": "#ff00ff",
      "status": "fadeToRGB: #FF00FF t:2.70",
      "time": "2.700"
    }


`/blink1/lastColor`
-----------------
__Description:__ Return last fadeToRGB color sent to blink(1).

__Query args:__ -none-

__Example:__

`http://localhost:8934/blink1/lastColor`

__Response:__

    {
      "lastColor": "#FF00FF",
      "status": "lastColor"
    }


`/blink1/patterns`
----------------
__Description:__
List saved color patterns

__Query args:__ -none-

__Example:__

`http://localhost:8934/blink1/pattern/`

__Response:__ 

    { 
      "patterns": [ 
        { 
           "name":"blink3_red",
           "pattern":"3,%23FF0000,1.0,%23000000,1.0"
        },
        { 
           "name":"Springtime",
           "pattern":"0,%2333FF000,2.1,%255ff00,3.0"
        }
      ]
    }



`/blink1/pattern/add`
---------------------
__Description:__
Add color pattern to color pattern list

__Query args:__

* `pname` -- name of color pattern to add
* `pattern` -- color pattern definition in string format
    
    format: "repeats,color1,color1time,color2,color2time,..."

__Example:__

`http://localhost:8934/blink1/pattern/add?pname=blink3_red&pattern=3,%23FF0000,1.0,%23000000,1.0`

__Response:__ Standard JSON 'status' response


`/blink1/pattern/del`
---------------------
__Description:__
Remove color pattern from color pattern list

__Query args:__

* `pname` -- name of color pattern to delete

__Example:__

`http://localhost:8934/blink1/pattern/del?pname=blink3_red`

__Response:__ Standard JSON 'status' response


`/blink1/pattern/play`
----------------------
__Description:__
Play/test a specific color pattern

__Query args:__

* `pname` -- name of color pattern to play

__Example:__

`http://localhost:8934/blink1/pattern/play?pname=blink3_red`

__Response:__ Standard JSON 'status' response


`/blink1/pattern/stop`
----------------------
__Description:__
Stop a pattern playback, for a given pattern or all patterns

__Query args:__

* `pname` -- pattern name string

__Example:__

`http://localhost:8934/blink1/pattern/stop?pname=blink3_red`

__Response:__ Standard 'status' JSON response





<s>
 
`/blink1/logging`
-----------------
__Description:__ Enable or disable logfile writing

__Query args:__ loglevel  (can be 0 or 1)

__Example:__

`http://localhost:8934/blink1/logging?loglevel=0`

__Response:__

    {
      "loglevel": 0,
      "status": "logging"
    }
 
</s>

<s>

`/blink1/inputs`
--------------
__Description:__ List configured inputs

__Query args:__ 'enable' : "on" == enable all configured inputs, "off" == off

__Example:__ 

`http://localhost:8934/blink1/inputs`

__Response:__ 
(example with several inputs configured)

    {
      "inputs" : [
       { 
         "iname":"mysqllog",
         "type":"file",
         "arg1":"/usr/local/mysqlout.txt"
       },
       { 
         "iname":"myarduino",
         "type":"url",
         "arg1":"http://todbot.com/tst/color.txt",
       },
       { 
         "iname":"cpu99",
         "type":"cpuload",
         "arg1":"99"
       },
       { 
         "iname":"net95",
         "type":"netload",
         "arg1":"95"
       },
       {
         "iname":"ifttt",
         "type":"ifttt",
       } 
       ]
    }


`/blink1/input/del`
-----------------
__Description:__ Remove a configured input

__Query args:__ 

* `iname` -- input name

__Example:__

`http://localhost:8934/blink1/input/del?iname=mysqllog`

__Response:__ Standard JSON 'status' response


`/blink1/input/delall`
-----------------
__Description:__ Remove all configured inputs

__Query args:__  -none-

__Example:__

`http://localhost:8934/blink1/input/delall`

__Response:__ Standard JSON 'status' response


`/blink1/input/ifttt`
-------------------
__Description:__ Start watching for messages from IFTTT webservice
Multiple IFTTT inputs can be added, each with its own name.  
The IFTTT webservice is queried once every 15 seconds, no matter how many IFTTT inputs are configured.
The arg1 parameter of each configured IFTTT input is compared against the rule name attribute from IFTTT.
When a match occurs, the color pattern specified in "pname" is triggered.

__Query args:__ 

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger, otherwise is set to contents of 'iname'
* `arg1`  -- ifttt rule name (defined by user on ifttt.com)
* `test`  -- testmode boolean, immediately run input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__

`http://localhost:8934/blink1/input/ifttt?iname=My+Favorite+Trigger&arg1=New+Gmail`

`http://localhost:8934/blink1/input/ifttt?iname=My+Favorite+Trigger&arg1=New+Gmail&test=true`

__Response:__  standard JSON response with keys: "input" and "status".  
The "input" keys contain the parsed input, with an additional array "possibleVals" that may exist if the IFTTT processor has run 
and seen iftt rule names (arg1 values) from the IFTTT reflector.

    {
      "input": {
        "arg1": "New Gmail",
        "iname": "My Favorite Trigger",
        "pname": "My Favorite Trigger",
        "possibleVals": [
          "some rss feed", "New Gmail"
        ],
        "type": "ifttt"
      },
      "status": "input ifttt"
    }


`/blink1/input/url`
-----------------
__Description:__ Start URL watcher on given URL; URL contains color pattern names or explicit hex color values.

__Query args:__ 

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger
* `arg1`  -- URL path to text file to watch, properly escaped
* `test`  -- testmode boolean, immediately run input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__

`http://localhost:8934/blink1/input/url?iname=myarduino&url=http://todbot.com/tst/color.txt`

`http://localhost:8934/blink1/input/url?iname=tod+test&arg1=http://todbot.com/tst/color.txt&test=true`

__Response:__ 

    {
      "input": {
        "arg1": "http://todbot.com/tst/color.txt",
        "iname": "tod test",
        "lastTime": 1353463912,
        "lastVal": "#9970FF",
        "pname": "tod test",
        "type": "url"
      },
      "status": "input url"
    }


`/blink1/input/file`
------------------
__Description:__ Start file watcher on given filepath; file contains color pattern names or hex rgb color

__Query args:__

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger, otherwise is set to contents of 'iname'
* `arg1`  -- filename, fully-qualified filepath to text file to parse
* `test`  -- testmode boolean, immediately run input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__

`http://localhost:8934/blink1/input/file?iname=mysqllog&path=/usr/local/mysqlout.txt`

__Response:__ Standard JSON 'input' & 'status' response


`/blink1/input/script`
--------------------
__Description:__ Run command-line script, get output as color name or rgb color code

__Query args:__

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger, otherwise is set to contents of 'iname'
* `arg1`  -- script name (must live in "~/Documents/blink1-scripts" folder)
* `test`  -- testmode boolean, immediately run input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__

`http://localhost:8934/blink1/input/script?iname=checkService&arg1=CheckServ.bat`

__Response:__  Standard JSON 'input' & 'status' response


`/blink1/input/cpuload`
---------------------
__Description:__
Start CPU load watching input.  If no arguments, returns load

____Query args:____ 

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger, otherwise is set to contents of 'iname'
* `arg1`  -- min, minimum level on which to trigger this event
* `arg2`  -- max, (optional) maximum level on which to trigger this event
* `test`  -- testmode boolean, immediately run input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__

`http://localhost:8934/blink1/input/cpuload?iname=cpu99&arg1=90`

__Response:__  Standard JSON 'input' & 'status' response


`/blink1/input/netload`
---------------------
__Description:__
Start network load watching input.  If no arguments, return network load as 0-100 percentage.

____Query args:____ 

* `iname` -- name for this input
* `pname` -- (optional) color pattern name to trigger
* `arg1`  -- min, minimum level on which to trigger this event
* `arg2`  -- max, (optional) maximum level on which to trigger this event
* `test`  -- testmode boolean, immediately input processor, but don't add to input list. Set to "on" or "true" to enable, otherwise false.

__Example:__
`http://localhost:8934/blink1/input/cpuload?iname=net95&arg1=95&arg2=100`

__Response:__  Standard JSON 'status' response

</s>


