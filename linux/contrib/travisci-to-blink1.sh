#!/bin/bash
#
# Monitors a project hosted at travis-ci.org and changes the color of a
# Blink(1) to reflect the build status (green for passing, red for failing,
# yellow for build in progress). If there is an error retrieving the status,
# the script will cause the Blink(1) to blink white until the next poll.
#
# DEPENDENCIES:
# This script requires that curl, sed, and awk be installed and on the
# PATH, which should be the case for most Linux and OS X installs.
# blink1-tool is required; if it is on the PATH, the script will find
# it; you can also specify its location on the command line.
#
# CREDITS:
# This script is loosely based on gmail-to-blink1.sh, which is found in the
# linux/contrib directory of the official Blink(1) repository.
# The JSON "parsing" code is adapted from this Gist by Carlos Justiniano:
# https://gist.github.com/1047794
#
# AUTHOR: 
# Matt Kimmel <matt@infinite-jest.net>
# Copyright (c) 2012, Matt Kimmel.
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


### VARIABLES ###

# Frequency, in seconds, to poll for project status.
# Probably best not to set this to too small an interval.
#
# If specifying two repositories then specify a number
# that is half of what you want each one to poll at.
POLL_FREQUENCY=60

# Path to blink-tool(1)
BLINK1_TOOL="`which blink1-tool`"

# URL of travis-ci's API
TRAVISCI_API_URL="https://api.travis-ci.org"

# which LED to light up (e.g. "1")
LED=""

# Project to monitor (e.g. "todbot/blink1")
# No default.
PROJECT1=""
PROJECT2=""

# Pass color
COLOR_PASS="0,255,0" # Green

# Fail color
COLOR_FAIL="255,0,0" # Red

# Building
COLOR_BUILDING="255,208,0" # Yellow. This looks much more yellow than 255,255,0 on my Blink(1)

# Error
COLOR_ERROR="255,255,255" # White


### FUNCTIONS ###

# Return the led parameter
function led_number
{
    if [ -n "${1}" ]; then
        echo "${1}"
    elif [ -n "$LED" ]; then
        echo "$LED"
    else
        echo "0"
    fi
}


# Set the Blink(1) to a color in "R,G,B" format, which must be supplied
# as an argument. arg2 is the led color
function set_blink1_color
{
    $BLINK1_TOOL  --led=$(led_number ${2}) --rgb ${1}> /dev/null 2>&1
}

# Blink the specified color for the specified number of seconds. Do so
# without running blink1-tool the entire time, so we can still turn
# off the Blink(1) on exit if necessary.
# arg1 is color, arg2 is number of seconds, arg3 is the led number
function blink_color_for_seconds
{
    for i in {1..${2}}
    do
        $BLINK1_TOOL --led=$(led_number ${3}) --rgb ${1} > /dev/null 2>&1
        sleep 0.5
        $BLINK1_TOOL --led=$(led_number ${3}) --off > /dev/null 2>&1
        sleep 0.5
    done
}

# Check the status of the url (arg1) and update the led (arg2)
function update_status
{
    # Query the travis-ci API for the status of the project's repo
    RESPONSE=$(curl -f -s "${TRAVISCI_API_URL}/repos/${1}")
    
    # If the query succeeded, try to parse it; otherwise, blink the error
    # color.
    if [ $? -eq 0 ]; then
        # Pull the last build status from the API's JSON response.
        # TODO: This is pretty complicated, but was the only way I could get
        # this working without introducing dependencies on more than sed and awk.
        # Improvements welcome.
        STATUS=$(echo ${RESPONSE} | sed 's/\\\\\//\//g' | sed 's/[{}]//g' | awk -v k="text" '{n=split($0,a,","); for (i=1; i<=n; i++) print a[i]}' | sed 's/\"\:\"/\|/g' | sed 's/[\,]/ /g' | sed 's/\"//g' | grep -w last_build_status | cut -d: -f2)

        # Set Blink(1) color according to status. On unparseable status,
        # blink the error color.
        if [ "${STATUS}" == "0" ]; then
            set_blink1_color ${COLOR_PASS} ${2}
            sleep ${POLL_FREQUENCY}
        elif [ "${STATUS}" == "1" ]; then
            set_blink1_color ${COLOR_FAIL} ${2}
            sleep ${POLL_FREQUENCY}
        elif [ "${STATUS}" == "null" ]; then
            set_blink1_color ${COLOR_BUILDING} ${2}
            sleep ${POLL_FREQUENCY}
        else
            blink_color_for_seconds ${COLOR_ERROR} ${POLL_FREQUENCY} ${2}
        fi
    else
        # Curl returned an error, so blink the error color. We do this in
        # place of sleeping before the next poll.
        blink_color_for_seconds ${COLOR_ERROR} ${POLL_FREQUENCY} ${2}
    fi
}

# Used with trap to shut off the Blink(1) when we get a SIGINT or SIGTERM.
function cleanup
{
    # Turn the Blink(1) off
    if [ -n ${PROJECT2} ]; then
       $BLINK1_TOOL --off > /dev/null 2>&1
    else
       $BLINK1_TOOL --led=$(led_number) --off > /dev/null 2>&1
    fi
    exit $?
}

# Show usage instructions
function show_usage
{
    echo " Usage: `basename ${0}` [OPTIONS] <project>"
    echo ""
    echo " Options:"
    echo "    -h              Displays this help"
    echo "    -t <seconds>    Polling interval in seconds (default: ${POLL_FREQUENCY} s.)"
    echo "    -l <led>        Which LED to light (e.g. 1 or 2) (default: both)"
    echo "    -b <path>       Path to blink1-tool, if it is not on the PATH"
    echo ""
    echo " <project> should be in the form ownername/projectname"
    echo ""
}


### SETUP ###

# Get command line options
while getopts "ht:b:l:" OPTION
do
    case $OPTION in
        h)
            show_usage
            exit 0
            ;;
        t)
            POLL_FREQUENCY=$OPTARG
            ;;
        b)
            BLINK1_TOOL="$OPTARG"
            ;;
        l)
            LED="$OPTARG"
            ;;
        ?)
            show_usage
            exit 1
            ;;
    esac
done

# Get project, and show usage and exit if none exists
shift $(($OPTIND - 1))
PROJECT1="$1"
if [ -z "${PROJECT1}" ]; then
    echo "${0}: no project specified."
    show_usage
    exit 1
fi
PROJECT2="$2"
# two projects specfied means not to use global led setting
if [ -n "${PROJECT2}" ]; then
    LED=""
fi

# Turn off the Blink(1) on SIGINT or SIGTERM
trap cleanup SIGINT SIGTERM


### MAIN ###

# In an infinite loop, poll the project and update the Blink(1) color.
if [ -z "${PROJECT2}" ]; then
    echo "Monitoring ${PROJECT1}. CTRL-C to exit."
else
    echo "Monitoring ${PROJECT1} and ${PROJECT2}. CTRL-C to exit."
fi

while true; do
    if [ -z "${PROJECT2}" ]; then
        update_status "${PROJECT1}" 
    else
        update_status "${PROJECT1}" 1
        update_status "${PROJECT2}" 2

    fi
done
