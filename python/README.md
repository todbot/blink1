Using Python to control blink(1)
================================

There are three ways to get Python to access blink(1): 

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
- Usable on many more platforms like Raspberry Pi

Cons:
- Ineffecient; Spawns a process for each blink(1) call
- Requires user to have blink1-tool downloaded and in path


Python libraries
----------------

Thanks to members of the blink(1) community, we have working examples of all three techniques:

- "blink1hid-demo.py" - Aaron Blondeau's script uses PyUSB to show how to construct raw HID reports 
that send commands to blink(1)

- "blink1-ctypes.py"  - Stephen Youndt's script shows how to wrap the C library

- "bliPi.py" - DPontes wrapper of blink1-tool, available at : https://github.com/DPontes/bliPy/blob/master/bliPy.py


