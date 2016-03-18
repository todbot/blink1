#!/bin/sh
#
# Get Skype presence indicator, return based on it
#
username=$1

if [ -z "$username" ] ; then 
  echo "usage: $0 skypeusername";  exit 1;
fi

skype_url="http://mystatus.skype.com/${username}.txt"
val=`curl -s "${skype_url}"`

if [ "$val" = "Offline" ] ; then
    echo "#000000";
elif [ "$val" = "Online" ] ; then
    echo "#00FF00";
elif [ "$val" = "Away" ] ; then
    echo "#FFFF00";
elif [ "$val" = "Do Not Disturb" ] ; then
    echo "#FF0000";
else
    echo "#000000";
fi

