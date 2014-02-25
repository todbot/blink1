#!/bin/sh

while [ 1 ] ; do
blink1-tool -l 2 --red && blink1-tool -l 1 --blue
sleep 1
blink1-tool -l 1 --red && blink1-tool -l 2 --rgb 255,255,40
sleep 1
done
