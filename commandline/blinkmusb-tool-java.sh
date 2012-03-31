#!/bin/sh
#
# you must type "make jar" before this script will work
#

if [ -e libtargets/blinkmusb.jar ]; then
    #java -d32 -Djava.library.path=libtargets -jar libtargets/blinkmusb.jar $*
    java -Djava.library.path=libtargets -jar libtargets/blinkmusb.jar $*
else 
    echo "cannot run. make the jar with 'make jar' please"
fi
