#!/bin/sh
# Act like a police light
# for mk2 devices
#
while [ 1 ] ; do
  blink1-tool -l 2 --red && blink1-tool -l 1 --blue
  sleep 0.5
  blink1-tool -l 1 --red && blink1-tool -l 2 --blue
  sleep 0.5
done
