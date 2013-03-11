#!/bin/bash
# Makes your Blink(1) blink when a commit happens to a repository 
#
# Note: Requires curl

BLINK=`which blink1-tool`

COLOR_PASS="0,200,50" # Mostly green

SLEEPTIME=300  # Time to wait between checking git
let COUNT=$SLEEPTIME/2  # Num of times to blink

# Inspired by the gmail example
cleanup()
{
    # Turn the Blink(1) off
    $BLINK --off > /dev/null 2>&1
    exit $?
}
trap cleanup SIGINT SIGTERM

# Load the repo from args
REPO=$1
if [[ -z $REPO ]]
then
    echo "Error Repo can not be empty." >&2
    echo "Usage:"
    echo "  ./github-to-blink <repo> "
    echo ""
    echo "For example ./github-to-blink todbot/blink1"
    echo ""
    exit 2
fi

while true; do
    cTime=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
    echo Will check $REPO every $SLEEPTIME seconds.
    sleep $SLEEPTIME
    resp=$(curl -s https://api.github.com/repos/$REPO/commits?since="$cTime")
    if [[ "$resp" == *commit* ]]
    then
        $BLINK --rgb $COLOR_PASS --blink $COUNT > /dev/null 2>&1
    else
        $BLINK --off > /dev/null 2>&1
    fi
done

