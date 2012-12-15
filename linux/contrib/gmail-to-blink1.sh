#!/bin/bash
# Blinks your Blink(1) when you have unread Gmail
#
# Dependencies:
#  wget (to retrieve gmail atom feed file) - most have this by default
#
# Return values:
#  0  exits successfully
#  1  error in options
#  2  blank username
#  3  blank password
#  4  wget/atom file issue
#
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
# Color to flash when there is gmail
COLOR_MAIL=0,204,255 #bright blue (similar to the old default gmail theme)

# Color to quickly flash before COLOR_MAIL when there is
# too much unread gmail
COLOR_TOO_MUCH_MAIL=255,0,0 #red


###########################
#<--- OTHER VARIABLES --->#
###########################
# How slow to blink
DELAY_BLINK="1"         # seconds

# Less than 60 seconds will probably upset Google and could
# result in them locking you out of your account.
DELAY_MAIL_CHECK="60"   # seconds

# Duh
GMAIL_USERNAME=""
GMAIL_PASSWORD=""

# How much mail is too much unread mail?
# Change this to 0 to disable the distinction
TOO_MUCH_MAIL=10

# Using which for convenience at the moment
# blink1-tool needs to be in your PATH
TOOL=`which blink1-tool`

####
# You probably don't want to change these
####
# Where to save the atom file from Gmail
GMAILOUTDIR="/tmp/"
GMAILOUT=${GMAILOUTDIR}"atom"

# A flag so we know what state we are in
LED=0

#####################
#<--- FUNCTIONS ---># No more configuration, hack below to have some fun
#####################

## Turn on the Blink(1)
function led_on
{
        LED=1
        # set the Blink(1) to COLOR_MAIL
        $TOOL --rgb $COLOR_MAIL > /dev/null 2>&1
}

## Turns off the Blink(1)
function led_off
{
        LED=0
        # set the Blink(1) to --off
        $TOOL --off > /dev/null 2>&1
}

## Makes a led to blink
function led_blink
##  ${1} time in seconds between blinks
{
    led_on
    sleep ${1}
    led_off
    sleep ${1}
}

## Checks mail with a given password
function check_mail
##  ${1} Gmail username
##  ${2} Gmail password
{
    GMAILHDR="https://"
    GMAILURL="mail.google.com/mail/feed/atom"
    WGETOPTS="--no-check-certificate --quiet -O $GMAILOUT"

    wget ${GMAILHDR}${GMAILURL} --user ${1} --password ${2} ${WGETOPTS}

    # We could probably be more specific regarding the wget return value
    # but I'm feeling lazy right now
    WGET_RETURN=${?}
    if [[ $WGET_RETURN -ne 0 ]]
    then
        exit 4
    fi

    # did we get the file from the atom feed?
    if [[ ! -f $GMAILOUT ]]
    then
        echo "$(basename ${0}): Could not accesss acount. May be the password you've typed is incorrect or there are many instances of this script runing at the same time." >&2
        exit 4
    fi

    COUNT=$(grep "fullcount" $GMAILOUT | sed 's/^<.*>\([^<].*\)<.*>$/\1/' )

    rm --force $GMAILOUT

    return $COUNT
}


## Show usage
function show_help
{
    echo " Usage: $(basename ${0}) -u <username> [OPTIONS]"
    echo ""
    echo " Options:"
    echo "    -h              Displays this help"
    echo "    -u <username>   Gmail username"
    echo "    -p <password>   Gmail password"
    echo "    -t <seconds>    Time between checks (default: ${DELAY_MAIL_CHECK} s.)"
    echo ""
    echo " If no password is given, a prompt will ask for it (not echoed)."
    echo ""
}


##signal trapping
CTRL_c()
## run if user hits CTRL-c
{
    # Turn the Blink(1) off
    led_off
    rm --force $GMAILOUT
    exit $?
}
# trap keyboard interrupt (CTRL-c)
trap CTRL_c SIGINT


################
#<--- MAIN --->#
################

# Get options
while getopts "hu:t:p" OPTION
do
    case $OPTION in
        h)
            show_help
            exit 0
            ;;
        u)
            GMAIL_USERNAME="$OPTARG"
            ;;
        p)
            GMAIL_PASSWORD="$OPTARG"
            ;;
        t)
            DELAY_MAIL_CHECK=$OPTARG
            ;;
        ?)
            show_help
            exit 1
            ;;
    esac
done


# Username or password cannot be empty
if [[ -z $GMAIL_USERNAME ]]
then
    echo "$(basename ${0}): Username cannot be blank" >&2
    echo ""
    show_help
    exit 2
fi
if [[ -z $GMAIL_PASSWORD ]]
then
    echo -n "Gmail Password: "
    stty -echo
    read GMAIL_PASSWORD
    stty echo
    echo ""
fi
if [[ -z $GMAIL_PASSWORD ]]
then
    echo "$(basename ${0}): Password cannot be blank" >&2
    exit 3
fi


MAIL_NUM=0
#infinite loop, stop with CTRL-c
while true; do
    echo Checking mail
    check_mail $GMAIL_USERNAME $GMAIL_PASSWORD
    MAIL_NUM=${?}

    if [[ $MAIL_NUM -eq 0 ]]
    then #there's no new mail
        if [[ $LED -eq 1 ]] #turn off the LED if it's already on
        then
            led_off
        fi
        sleep $DELAY_MAIL_CHECK
    else #there is new mail

        # We want to keep blinking until the next check but this loop
        # is blocking, so let's all pull out our abaci...
        # We only need to blink for half the duration of DELAY_MAIL_CHECK
        # (since the Blink(1) will be --off half of this time as it blinks). 
        # Then we account for the DELAY_BLINK duration will put us fairly
        # close to the time to do the next check that is defined by
        # DELAY_MAIL_CHECK once we complete the loop
        #
        # We could try to get fancy or go with some date/time math, but
        # we're going to just stick with a horseshoes and hand grenades
        # method here... close will do the job!
        N=$((${DELAY_MAIL_CHECK}/2/${DELAY_BLINK}))

        # We could use $TOOL --blink and skip all this, but it seems that
        # it only blinks white at the moment. (Perhaps I didn't read the
        # docs closely enough)
        for i in `seq 1 $N`
        do
            # If TOO_MUCH_MAIL isn't 0
            if [[ $TOO_MUCH_MAIL -ne 0 ]]; then
                # If there is TOO_MUCH_MAIL
                if [[ $MAIL_NUM -ge $TOO_MUCH_MAIL ]]; then
                    # Quickly flash COLOR_TOO_MUCH_MAIL
                    # This slightly increases the time until the next
                    # check, but whatever... it's cool looking
                    $TOOL --rgb $COLOR_TOO_MUCH_MAIL > /dev/null 2>&1
                fi
            fi

            led_blink $DELAY_BLINK # blink once
        done
    fi

done

