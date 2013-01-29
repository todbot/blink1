
Using Python to control blink(1)
================================

There are two ways to get Python to access blink(1): 

- Use the PyUSB library to construct USB HID requests
- Wrap the "blink1-lib" C-library located in blink1/commandline

The PyUSB is very easy to use, but creates an additional dependency for shipped code.  
Wrapping the C-library means you need to deal with platform-specific issues.

Thanks to members of the blink(1) community, both of these techniques have working examples:

- Aaron Blondeau's script "blink1hid-demo.py" shows how to construct raw HID reports 
that send commands to blink(1)

- Stephen Youndt's script "blink1-ctypes.py" shows how to wrap the C library

