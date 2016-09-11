#!/bin/sh

### BEGIN INIT INFO
# Provides:		blink1-tiny-server
# Required-Start:	$remote_fs $syslog
# Required-Stop:	$remote_fs $syslog
# Default-Start:	4 5
# Default-Stop:		
# Short-Description:	Simple API server for blink(1) USB light
### END INIT INFO

#
# Here is a simple example of an init script for blink1-tiny-server
#
# Stick this file in /etc/init.d
# Use 'update-rc.d' and 'service' to install.
# e.g.
#  $ sudo cp blink1-tiny-server-init.sh /etc/init.d/blink1-tiny-server
#  $ sudo update-rc.d blink1-tiny-server defaults
# then:
#  $ sudo service blink1-tiny-server status
#  $ sudo service blink1-tiny-server start
#  $ sudo service blink1-tiny-server stop
#

# port you want blink1-tiny-server to live on
PORT=8888

# path to your copy of blink1-tiny-server, tries to find automatically
SERVER_PATH=`which blink1-tiny-server`
if [ -z "$SERVER_PATH" ] ; then
    SERVER_PATH=/home/pi/projects/blink1/commandline/blink1-tiny-server
fi
    
case "$1" in
    start)
	echo "Starting blink1-tiny-server at ${SERVER_PATH}"
	${SERVER_PATH} -p 8888 >/dev/null 2>&1 < /dev/null &
	;;
    stop)
	echo "Stopping blink1-tiny-server"
	killall blink1-tiny-server
	;;
    *)
	echo "Usage: /etc/init.d/blink1-tiny-server start|stop"
	exit 1
	;;
esac

exit 0
