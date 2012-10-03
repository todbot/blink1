
URL API for blink(1) Applications
=================================
20121001 - Tod E. Kurt

The application that controls a blink(1) device is comprised of an HTML5 GUI 
running on an embedded webserver.  The GUI communicates to the application
backend via the URL API described below.

In this document:

* Architecture
* URL Summary
* URL Command Reference


## Architecture ##############################################################

### Overview ###
The application maintains a list of data inputs from which it reads.
These inputs emit consume data and output color pattern names.
Color patterns are a list of color commands for blink(1).
The application maintains a these color patterns.
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
    - e.g. `/blink1/enumerate`

* `/blink1/fadeToRGB` 
-- Send fadeToRGB command to blink(1) with hex color & fade time
    - e.g. `/blink1/fadeToRGB?rgb=%23FF00FF&time=2.7`

### Input Selection ###

* `/blink1/input/` 
-- List configured inputs

* `/blink`/input/del` 
-- Remove a configured input
    - e.g. `/blink1/input/del?iname=mysqllog`

* `/blink1/input/file` 
-- Start file watcher on given filepath
    - e.g. `/blink1/input/file?iname=mysqllog&path=/usr/local/mysqlout.txt`

* `/blink1/input/url` 
-- Start URL watcher on given URL
    - e.g. `/blink1/input/url?iname=myarduino&url=http://todbot.com/tst/color.txt`

* `/blink1/input/script` -- Run command-line script
    - e.g. `/blink1/input/script?iname=checkService&cmd=CheckServ.bat`

* `/blink1/input/ifttt` -- Start watching messages from IFTTT webservice
    - e.g. `/blink1/input/ifttt

* `/blink1/input/cpuload` -- Start CPU load watching input
    - e.g. /blink1/input/cpuload?iname=cpu99p&level=99

* `/blink1/input/netload`
    - Start network load watching input
    - If no arguments, return network load as 0-100 percentage
    - e.g. `/blink1/input/netload?iname=net95&level=95`


### Color Pattern Handling ###

* `/blink1/pattern/`
    - List color patterns

* `/blink1/pattern/add`
    - Add color pattern to color pattern list
    e.g. `/blink1/pattern/add?pname=blink3_red&pattern=3,%23FF0000,1.0,%23000000,1.0`

* `/blink1/pattern/del`
    - Remove color pattern to color pattern list

* `/blink1/pattern/play`
    - play/test a specific color pattern
    - e.g. `/blink1/pattern/play?pname=blink_red`

* `/blink1/pattern/stop`
    - stop a pattern playback, for a given pattern or all patterns



### Trigger Mapping ###

* `/blink1/trigger/`
    - List trigger mappings

* `/blink1/trigger/add`
    - Add an input to color name trigger map
    - e.g. `/blink1/trigger/add/?iname=ifttt-gmail&pname=blink1_red`

* `/blink1/trigger/del`
    - Remove an input to color name trigger map
    - e.g. `/blink1/trigger/del?iname=ifttt-gmail`

    



## URL Command Reference  ###################################################

/blink1/fadeToRGB
-----------------
Description:

Query args:

* rgb : hex RGB color (e.g. "#ff00ff")
* time : time in seconds to complete fade (e.g. "0.8")

Response:

Example:


/blink1/enumerate
-----------------
Description: Re-enumerate and List available blink(1) devices

Query args:

Response:

Example:
`/blink1/enumerate`


/blink1/fadeToRGB
-----------------
Description: Send fadeToRGB command to blink(1) with hex color & fade time

Query args:

Response:

Example:
`/blink1/fadeToRGB?rgb=%23FF00FF&time=2.7`


/blink1/input/
--------------
Description: List configured inputs

Query args: -none-

Response: 
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
       } 
       ]
    }

Example:

/blink1/input/del
-----------------
Description: Remove a configured input

Query args: 

* `iname` -- input name

Response: standard JSON 'status' response

Example:

`/blink1/input/del?iname=mysqllog`


/blink1/input/file
------------------
Description: Start file watcher on given filepath; file contains color pattern names or hex rgb color

Query args:

* `iname` -- name for this input
* `file` -- fully-qualified filepath to text file to parse

Response: standard JSON 'status' response

Example:

`/blink1/input/file?iname=mysqllog&path=/usr/local/mysqlout.txt`


/blink1/input/url
-----------------
Description: Start URL watcher on given URL; URL contains color pattern names

Query args: 

* `iname` -- name for this input
* `url`  -- URL path to text file to watch, properly escaped

Response: standard JSON 'status' response

Example:

`/blink1/input/url?iname=myarduino&url=http://todbot.com/tst/color.txt`


/blink1/input/script
--------------------
Description: Run command-line script, get output as color name or rgb color code

Query args:

* `iname` -- name for this input
* `script` -- script name (must live in blink1 'scripts' directory)

Response:  standard JSON 'status' response

Example:

`/blink1/input/script?iname=checkService&script=CheckServ.bat`


/blink1/input/ifttt
-------------------
Description: Start/stop watching for messages from IFTTT webservice

Query args: 

`enable` -- "on" or "off"

Response:  standard JSON 'status' response

Example:
`/blink1/input/ifttt?enable=on`


/blink1/pattern/
----------------
Description:

Query args:

Response: 

Example:


/blink1/pattern/add
-------------------
Description:

Query args:

Response:

Example:


/blink1/pattern/del
-------------------
Description:

Query args:

Response:

Example:


/blink1/pattern/play
--------------------
Description:

Query args:

Response:

Example:


/blink1/pattern/stop
--------------------
Description:

Query args:

* `pname` -- pattern name string

Response: standard 'status' JSON response

Example:




