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

# The threshold for load15
THRESHOLD_15="2.00"

# The threshold for load5
THRESHOLD_5="3.00"

# The threshold for load1
THRESHOLD_1="4.00"


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
COLOR_1=255,255,0 # yellow


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

    # If we've crossed THRESHOLD_15...
    if [[ $res15 -eq 1 ]]; then
        # ...and we are not already alerting
        if [[ $ALERT -eq 0 ]]; then
            ALERT=1 # we are alerting, so...
            # set the Blink(1) to COLOR_15
            $TOOL --rgb $COLOR_15 > /dev/null 2>&1
        fi

    # otherwise, if we've crossed THRESHOLD_5...
    elif [[ $res5 -eq 1 ]]; then
        # ...and we are not already alerting
        if [[ $ALERT -eq 0 ]]; then
            ALERT=1 # we are alerting, so...
            # set the Blink(1) to COLOR_5
            $TOOL --rgb $COLOR_5 > /dev/null 2>&1
        fi

    # otherwise, if we've crossed THRESHOLD_1...
    elif [[ $res1 -eq 1 ]]; then
        # ...and we are not already alerting
        if [[ $ALERT -eq 0 ]]; then
            ALERT=1 # we are alerting, so...
            # set the Blink(1) to COLOR_1
            $TOOL --rgb $COLOR_1 > /dev/null 2>&1
        fi

    # otherwise, if we were alerting...
    elif [[ $ALERT -eq 1 ]]; then
        ALERT=0 # ...we don't need to any longer, so...
        # set the Blink(1) to COLOR_OK
        $TOOL --rgb $COLOR_OK > /dev/null 2>&1
    fi

#take a nap
sleep $SLEEP
done

