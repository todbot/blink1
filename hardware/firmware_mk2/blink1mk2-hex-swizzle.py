#!/usr/bin/env python

import sys
import string
import re

data = sys.stdin.readlines()

# line is in form
# ;083FF000CD34AB34343412343B
# need to keep checksum (last two chars)
# need to keep first 9 chars (semi-colon + 8 chars)
# need to swap "1234" with "CD34",
# expanded it is:
# ;083FF000 CD34 AB34 3434 1234 3B
#
p = re.compile(r'([;:])(\w{8})(\w{4})(\w{4})(\w{4})(\w{4})(\w{2})')

for line in data:
    #line = re.sub( r'\\','', line)
    m = p.search( line )
    if m :
        print ''.join( m.group(1,2, 6,5,4,3, 7 ))
    else:
        print line


        
