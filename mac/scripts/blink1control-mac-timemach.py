#!/usr/bin/python
#
# Very simple and script for Blink1Control to check Time Machine status
#
# Change colors below to suit your uses
#
# 2014, Tod E. Kurt, http://todbot.com/blog/ 
#

colorRunning = 'pattern: red flashes'
colorOff     = ''

#colorRunning = '#FF0000'
#colorOff     = '#000000'

import subprocess
import re

p = subprocess.Popen(['tmutil', 'status'],
                     stdout=subprocess.PIPE, stderr=subprocess.PIPE)
out, err = p.communicate()
#print out

# super hacky
if re.search("Running = 1", out): print colorRunning;

if re.search("Running = 0", out): print colorOff;

