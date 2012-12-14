#!/bin/bash
# Automatically change a Blink(1)'s color based on a
# USB Webcam's camera or audio use
#
# AUTHOR: Matthew J. Brooks <matthew@sonomatechpartners.com>
# Copyright (C)2012  Matthew J. Brooks <matthew@sonomatechpartners.com>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

##################
#<--- COLORS --->#
##################
# Color when the camera or microphone is not in use
COLOR_FREE=0,255,0 #green

# Color when the camera is in use
COLOR_CAM=255,0,0 #red

# Color when the microphone is in use, but not the camera
COLOR_MIC=255,153,0 #orange

###################
#<--- DEVICES ---># Yours may vary
###################

# The default video camera device
CAMERA=/dev/video0

# This covers all of the input sound devices
MICROPHONE=/dev/snd/pcmC*D*c

###########################
#<--- OTHER VARIABLES --->#
###########################

# Using which for convenience at the moment
# blink1-tool needs to be in your PATH
TOOL=`which blink1-tool`

# How long to sleep between polling
SLEEP=5

################
#<--- MAIN ---># No more configuration, hack below to have some fun
################

# A flag so we know what state we are in so we don't constantly
# send redundant --rgb commands to set the LED to the same color
# with every poll
BUSY=0

# Start out as being available/free
$TOOL --rgb $COLOR_FREE > /dev/null 2>&1

#signal trapping
CTRL_c()
# run if user hits CTRL-c
{
    # Turn the Blink(1) off
    $TOOL --off > /dev/null 2>&1
    exit $?
}
# trap keyboard interrupt (CTRL-c)
trap CTRL_c SIGINT

#infinite loop, stop with CTRL-c
while true; do
    CAM=`lsof $CAMERA`;

    # If the camera is in use and we're not already "busy"
    if [[ $CAM && $BUSY -eq 0 ]]; then
        BUSY=1 # We're busy now, so...
        # set the Blink(1) to COLOR_CAM
        $TOOL --rgb $COLOR_CAM > /dev/null 2>&1

    # Otherwise if the camera is not on,
    # check if the microphone is being used
    elif [[ ! $CAM ]]; then
        MIC=`lsof $MICROPHONE`

        # If the microphone is being used and we're not already "busy"
        if [[ $MIC && $BUSY -eq 0 ]]; then
            BUSY=1 # We're busy now, so...
            # set the Blink(1) to COLOR_MIC
            $TOOL --rgb $COLOR_MIC > /dev/null 2>&1

        # If the microphone is not in use, but we are flagged as busy
        elif [[ ! $MIC && $BUSY -eq 1 ]]; then
            BUSY=0 # We're not really busy afer all, so...
            # set the Blink(1) to COLOR_FREE
            $TOOL --rgb $COLOR_FREE > /dev/null 2>&1
        fi
    fi

#take a nap
sleep $SLEEP
done

