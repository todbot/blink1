Blink1Control File/Script/URL response format
============================================

Blink1Control can fetch color or pattern triggers
from several classes of sources.  Thoses sources are:

  - text files  (e.g. "somefile.txt")
  - executed scripts / programs (e.g. "myscript.sh", "serverquery.py")
  - URLs (e.g. "http://todbot.com/tst/pattern.txt")
  
### Allowed values ###

The contents of these text files can either be a hex color code 
or the name of an existing color pattern configred in Blink1Control.


### Format ###

The format that Blink1Control understands can be one of the following:

#### Hex color code ####
Grabs the first 6-digit hex colorcode (starts with '#' followed by 6 hexadecimal digits)

Example 1:
```
set the color
#008899
```

Example 2:
```
This is a text file with some text.
And here's another line.
Here is a color code #ff3399 inside a block of text.
And here is another "#123999" in quotes.
```

#### JSON hex color code ####
Should be valid JSON, try http://jsonlint.com/

Example 1:
```
{
  "color": "#FF00FF"
}
```

#### Simple pattern line ####
Just the string "pattern:" followed by a pattern name.  
If pattern name contains spaces, use double-quotes.  Any other content is ignored.

Example 1:
```
pattern: groovy
```

Example 2:
```
pattern: "red flashes"
# ddd
```


#### JSON pattern line ####
Should be valid JSON, try http://jsonlint.com/

Example 1:
```
{ "pattern": "red flashes" }
```

Example 2:
```
{
  "pattern": "policecar"
  "comment": "should trigger policecar pattern"
}
```
 
 
