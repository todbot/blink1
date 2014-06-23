Using Python with blink(1)
==========================

Python libraries
----------------

The current official Python library for blink(1) is `blink1.py` below. It's actually a thin wrapper around two different approaches, depending on your installation.  The `blink1.py` library will pick the correct one automatically.

- `blink1.py` - blink(1) library
This is a ThingM-supported library. It wraps the two libraries below automatically.
Run it by itself to get a command-line tool similar to blink1-tool.

- `blink1_pyusb.py` - blink(1) library using PyUSB.
This is a ThingM-supported Python library for blink(1).

- `blink1_ctypes.py` - blink(1) library using ctypes (which in turn wraps HIDAPI).
This is a ThingM-supported Python library for blink(1).


Thanks to members of the blink(1) community who helped create these.


## Things to Note ##

There are at least three ways to get Python to access blink(1): 

- Use the PyUSB library to construct USB HID requests
- Wrap the "blink1-lib" C-library located in blink1/commandline
- Wrap the "blink1-tool" command line executable

There are pros and cons ot each technique.

### PyUSB method ###

Pros:
- 100% Python, no native C-code dependencies; if you have PyUSB, this will work
- Effeciently uses system resources (assuming PyUSB does)

Cons:
- Requires user to install PyUSB (which usually requires admin access)
- PyUSB not available on all platforms 
- Requires re-implementing functions in blink1-lib or blink1-tool 

### blink1-lib wrapper method ###

Pros:
- No extra libraries to install
- Functionality matches blink1-lib by definition

Cons:
- Native C-code code makes it harder to port to other systems

### blink1-tool commandline wrapper ###

Pros:
- 100% Python
- Easy to understand

Cons:
- Ineffecient; Spawns a process for each blink(1) call
- Requires user to have blink1-tool downloaded and in path



Other code
----------
- `blink1_pyusb_demoblondeau.py` - Aaron Blondeau's script uses PyUSB to show how to construct raw HID reports that send commands to blink(1)

- `blink1_ctypes_demoyoundt.py`  - Stephen Youndt's script shows how to wrap the C library

- `bliPi.py` - DPontes wrapper of blink1-tool, available at : https://github.com/DPontes/bliPy/blob/master/bliPy.py


