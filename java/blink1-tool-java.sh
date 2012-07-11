#!/bin/sh
#
# you must type "make jar" before this script will work
#

if [ -e libtargets/blink1.jar ]; then
    #java -d32 -Djava.library.path=libtargets -jar libtargets/blink1.jar $*
    java -Djava.library.path=libtargets -jar libtargets/blink1.jar $*
else 
    echo "cannot run. make the jar with 'make jar' please"
fi
