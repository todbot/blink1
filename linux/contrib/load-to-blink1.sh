#!/bin/bash
# Automatically change a Blink(1)'s color based on system load
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

######################
#<--- THRESHOLDS ---># In order of precedence
######################
# Load analysis is a dark art, and varies by machine so you
# probably want to change these to your own preferred levels.
#
# If you don't understand what I just said, this script probably
# won't be much use to you ;^)
#
# Dec 2012 - WCWaggoner (ctgreybeard)
#
# Changed the philosophy for the colors.
#
# Threshold 1 MUST be passed to alert at all. The theory is that T1
# is the most immediate measurement and if that is below the threshold
# then the others will be coming down also.
#
# T5 is then examined to see the extent of the load duration and,
# likewise, T15. T15 takes precendence over T5. T5 or T15 take
# precendence over T1.
#
# Finally, the color is pushed every time (currenty 30 seconds) in
# that some other activity may have changed the color while we were
# sleeping.
#
# Some thought was given to reading the current color and only sending
# a change if there was a difference but that seemed to be 
# counterproductive.

# The threshold for load15
THRESHOLD_15="1.10"

# The threshold for load5
THRESHOLD_5="1.20"

# The threshold for load1
THRESHOLD_1="1.50"


##################
#<--- COLORS ---># In order of precedence
##################

# Color when the loads are normal
#
# Change this to 0,0,0 if you want the Blink(1) to only be lit
# when the loads exceed the threshold values above, or if you'd
# prefer to just make it dimmer, change the 255 to a lesser value
# like 150
COLOR_OK=0,0,255 # blue

# Color when the load15 exceeds THRESHOLD_15
COLOR_15=255,0,0 # red

# Color when the load5 exceeds THRESHOLD_5
COLOR_5=255,153,0 # orange

# Color when the load1 exceeds THRESHOLD_1
# I needed to change this from 255,255,0 because that was too green
COLOR_1=255,200,0 # yellow


###########################
#<--- OTHER VARIABLES --->#
###########################

# Using which for convenience at the moment
# blink1-tool needs to be in your PATH
TOOL=`which blink1-tool`

# How long to sleep between polling
# If this is too low and the system is borderline it would swap
# back and forth. Also, since we are watching the load, there is
# no point in contributing more to it than necessary ;^)
#
# Accurate to a half minute should be fine
SLEEP=30


################
#<--- MAIN ---># No more configuration, hack below to have some fun
################

# A flag so we know what state we are in so we don't constantly
# send redundant --rgb commands to set the LED to the same color
# with every poll
ALERT=0

# Start out as being available/free
$TOOL --rgb $COLOR_OK > /dev/null 2>&1

##signal trapping
cleanup()
{
    # Turn the Blink(1) off
    $TOOL --off > /dev/null 2>&1
    exit $?
}
# trap keyboard interrupt (CTRL-c) or a SIGTERM (kill)
trap cleanup SIGINT SIGTERM


#infinite loop, stop with CTRL-c
while true; do
    # Gather the load information
    #
    # I could combine this with the 'res' variables below
    # but this makes it handy if you want to echo or do something
    # with the actual load values later, either here or via
    # copy-paste elsewhere ;)
    load1=$(uptime | awk '{ print $10 }' | cut -c1-4)
    load5=$(uptime | awk '{ print $11 }' | cut -c1-4)
    load15=$(uptime | awk '{ print $12 }' | cut -c1-4)

    res1=$(echo "$load1 > $THRESHOLD_1" | bc)
    res5=$(echo "$load5 > $THRESHOLD_5" | bc)
    res15=$(echo "$load15 > $THRESHOLD_15" | bc)

    # We start clean
    ALERT=9
    SETCOLOR=$COLOR_OK

    # if we've crossed THRESHOLD_1...
    if [[ $res1 -eq 1 ]]; then
	ALERT=1 # we are alerting, so...
	# set the Blink(1) to COLOR_1
	SETCOLOR=$COLOR_1
    fi

    # if we are already alerting
    if [[ $ALERT -eq 1 ]]; then
	# and if we've crossed THRESHOLD_5...
	if [[ $res5 -eq 1 ]]; then
            # set the Blink(1) to COLOR_5
            SETCOLOR=$COLOR_5
        fi
    fi

    # if we are already alerting
    if [[ $ALERT -eq 1 ]]; then
	# and we've crossed THRESHOLD_15...
	if [[ $res15 -eq 1 ]]; then
            # set the Blink(1) to COLOR_15
            SETCOLOR=$COLOR_15
        fi
    fi

# Set the color
# We set it each time, somebody else might have messed with it
    $TOOL --rgb $SETCOLOR > /dev/null 2>&1

#take a nap
    sleep $SLEEP
done

