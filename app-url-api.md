
URL API for blink(1) Applications
=================================
version 0.3 -- 20121001 -- Tod E. Kurt

The application that controls a blink(1) device is comprised of an HTML5 GUI 
running on an embedded webserver.  The GUI communicates to the application
backend via the URL API described below.

In this document:

* Architecture
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

### Inputs ###
There are multiple kinds of data input sources: text files, URL responses, etc.
Each configured input is given a unique human-readable input name.
Some inputs have a configurable update rate.
When an input is evaluated, it produces as a color pattern name as output.
Depending on the input, generation of color pattern names may happen external to the application (e.g. URL fetch) or internal (e.g. CPU load setpoint).
The color pattern name does not need to previously exist the color pattern list.

### Color Patterns ###
A color pattern is a named list of RGB colors and times between colors.
A color pattern can also specify an optional number of repeats to perform.
These colors are scheduled and sent to blink(1) based on the given timing.
There are multiple color patterns that can play on a blink(1) simultaneously.

### Examples ###
- input: "fetch a URL every 30 seconds"
- pattern name: "blink3times_red" or "ladygagatweet"
- pattern: "blink3times_red,3,#FF0000,1.0,#000000,1.0"

### File Formats ###
In general, files emitted or parsed are text files with JSON data structures.  
The keys in the JSON struct depends on the application.
For command responses, there is typically the "status" key with a value describing how an operation performed. 
One exception to the JSON rule, is the simple "RGB hex string" for files and URLs fetching.  These files are usually single-line and the first occurrence of `/#XXXXXX/` is parsed as a hex RGB color.

### File Format Examples ###

__File with color pattern name:__

    { 
      "color_pattern":"blink3_red"
    }

__Standard JSON response for `/blink1/fadeToRGB?rgb=#0000FF&time=5.0`:__

    {
      "rgb": "#0000ff",
      "status": "fadeToRGB: no blink(1) detected",
      "time": "5.000"
    }

__IFTTT response:__

    {
      "blink1_id":"323abcd3f",
      "name":"default",
      "source":"default",
      "date":"1348604132"
    }

__IFTTT response:__

    {
      "blink1_id":"7890abcd12",
      "name":"New Mail",
      "source":"gmail",
      "date":"1348604177"
    }

__File with RGB hex string:__

    {
      "rgb":"#FF0023"
    }

__File with RGB hex string:__

    color: #FF2233

__File with RGB hex string:__

    #CCFF00


## URL Summary ##############################################################

Base URL: `http://localhost:8080/blink1`


### Direct blink(1) control ###

* `/blink1/enumerate` 
-- Re-enumerate and List available blink(1) devices

* `/blink1/fadeToRGB` 
-- Send fadeToRGB command to blink(1) with hex color & fade time

### Input Selection ###

* `/blink1/input/` 
-- List configured inputs

* `/blink1/input/del` 
-- Remove a configured input

* `/blink1/input/file` 
-- Start file watcher on given filepath

* `/blink1/input/url` 
-- Start URL watcher on given URL

* `/blink1/input/script` -- Start command-line script executer

* `/blink1/input/ifttt` -- Start watching messages from IFTTT webservice

* `/blink1/input/cpuload` -- Start CPU load watching input

* `/blink1/input/netload`
-- Start network load watching input


### Color Patterns ###

* `/blink1/pattern/`
-- List saved color patterns

* `/blink1/pattern/add`
-- Add color pattern to color pattern list

* `/blink1/pattern/del`
-- Remove color pattern from color pattern list

* `/blink1/pattern/play`
-- Play/test a specific color pattern

* `/blink1/pattern/stop`
-- Stop a pattern playback, for a given pattern or all patterns




## URL Command Reference  ###################################################

/blink1/enumerate
-----------------
__Description:__ Re-enumerate and List available blink(1) devices

__Query args:__ -none-

__Example:__
`/blink1/enumerate`

__Response:__ 
    { 
      "status": "blink(1) devices detected",
      "blink1_serialnums" : [ "01A12345", "01A12346", "01A12347" ]
    } 


/blink1/fadeToRGB
-----------------
__Description:__ Send fadeToRGB command to blink(1) with hex color & fade time

__Query args:__

* `rgb` : hex RGB color (e.g. "#ff00ff")
* `time` : time in seconds to complete fade (e.g. "0.8")

__Example:__
`/blink1/fadeToRGB?rgb=%23FF00FF&time=2.7`

__Response:__ 
Standard JSON 'status' response


/blink1/input/
--------------
__Description:__ List configured inputs

__Query args:__ -none-

__Example:__ 
`/blink1/input`

__Response:__ 
(example with several inputs configured)

    {
      "inputs" : [
       { 
         "iname":"mysqllog",
         "type":"file",
         "path":"/usr/local/mysqlout.txt"
       },
       { 
         "iname":"myarduino",
         "type":"url",
         "url":"http://todbot.com/tst/color.txt",
       },
       { 
         "iname":"cpu99",
         "type":"cpuload",
         "level":"99"
       },
       { 
         "iname":"net95",
         "type":"netload",
         "level":"95"
       },
       {
         "iname":"ifttt",
         "type":"ifttt",
       } 
       ]
    }


/blink1/input/del
-----------------
__Description:__ Remove a configured input

__Query args:__ 

* `iname` -- input name

__Example:__
`/blink1/input/del?iname=mysqllog`

__Response:__ Standard JSON 'status' response



/blink1/input/file
------------------
__Description:__ Start file watcher on given filepath; file contains color pattern names or hex rgb color

__Query args:__

* `iname` -- name for this input
* `file` -- fully-qualified filepath to text file to parse
* `pname` -- (optional) color pattern name to trigger

__Example:__
`/blink1/input/file?iname=mysqllog&path=/usr/local/mysqlout.txt`

__Response:__ Standard JSON 'status' response



/blink1/input/url
-----------------
__Description:__ Start URL watcher on given URL; URL contains color pattern names

__Query args:__ 

* `iname` -- name for this input
* `url`  -- URL path to text file to watch, properly escaped
* `pname` -- (optional) color pattern name to trigger

__Example:__
`/blink1/input/url?iname=myarduino&url=http://todbot.com/tst/color.txt`

__Response:__ Standard JSON 'status' response


/blink1/input/script
--------------------
__Description:__ Run command-line script, get output as color name or rgb color code

__Query args:__

* `iname` -- name for this input
* `script` -- script name (must live in blink1 'scripts' directory)
* `pname` -- (optional) color pattern name to trigger

__Example:__
`/blink1/input/script?iname=checkService&script=CheckServ.bat`

__Response:__  Standard JSON 'status' response


/blink1/input/ifttt
-------------------
__Description:__ Start watching for messages from IFTTT webservice

__Query args:__ -none-

__Example:__
`/blink1/input/ifttt`

__Response:__  Standard JSON 'status' response


/blink1/input/cpuload
---------------------
__Description:__
Start CPU load watching input.  If no arguments, return network load as 0-100 percentage

____Query args:____ 

* `iname` -- name for this input
* `level` -- level from 1-100 on which to trigger this event
* `pname` -- (optional) color pattern name to trigger

__Example:__
`/blink1/input/cpuload?iname=cpu99&level=99`

__Response:__  Standard JSON 'status' response


/blink1/input/netload
---------------------
__Description:__
Start network load watching input.  If no arguments, return network load as 0-100 percentage.

____Query args:____ 

* `iname` -- name for this input
* `level` -- level from 1-100 on which to trigger this event
* `pname` -- (optional) color pattern name to trigger

__Example:__
`/blink1/input/cpuload?iname=net95&level=95`

__Response:__  Standard JSON 'status' response


/blink1/pattern/
----------------
__Description:__
List saved color patterns

__Query args:__ -none-

__Example:__
`/blink1/pattern/`

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



/blink1/pattern/add
-------------------
__Description:__
Add color pattern to color pattern list

__Query args:__

* `pname` -- name of color pattern to add
* `pattern` -- color pattern definition in string format
    
    format: "repeats,color1,color1time,color2,color2time,..."

__Example:__
`/blink1/pattern/add?pname=blink3_red&pattern=3,%23FF0000,1.0,%23000000,1.0`

__Response:__ Standard JSON 'status' response


/blink1/pattern/del
-------------------
__Description:__
Remove color pattern from color pattern list

__Query args:__

* `pname` -- name of color pattern to delete

__Example:__
`/blink1/pattern/del?pname=blink3_red`

__Response:__ Standard JSON 'status' response


/blink1/pattern/play
--------------------
__Description:__
Play/test a specific color pattern

__Query args:__

* `pname` -- name of color pattern to play

__Example:__
`/blink1/pattern/play?pname=blink3_red`

__Response:__ Standard JSON 'status' response


/blink1/pattern/stop
--------------------
__Description:__
Stop a pattern playback, for a given pattern or all patterns

__Query args:__

* `pname` -- pattern name string

__Example:__
`/blink1/pattern/stop?pname=blink3_red`

__Response:__ Standard 'status' JSON response




