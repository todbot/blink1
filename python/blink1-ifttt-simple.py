#!/usr/bin/python
#
# originally from https://gist.github.com/tomconte/4496283
#
# Requiremenets:
# - "blink1-tool" is in your path and works without sudo
#      (e.g. install udev rules, see "/blink1/linux/51-blink1.rules)
# - Generate an IFTTT key using Blink1Control and register it on IFTTT
#

import urllib2
import json
import time
import calendar
import subprocess
import os

### begin configuration
ifttt_key ='99234567DEADC0DE'

rule_map = {
    'my_rule_name' : '#ff00ff',
    'bob' : '#00ff00',
    'new mail' : '2,  1.0,#ffff00,0,  0.5,#00ff00,1',
    
    }

# set to empty to auto-search for it
blink1_tool_path = ''

debug = True
### end configurable

if blink1_tool_path == '':
    blink1_tool_path = os.popen("which %s" % "blink1-tool").read().strip()

if debug: print "using blink1-tool: " + blink1_tool_path

base_url = 'http://api.thingm.com/blink1/events/'

sleep_time = 5
 
#last_time = time.localtime()
last_time = time.localtime()

url = base_url + ifttt_key
if debug:  print 'events url: '+url

#
def play_pattern(patt):
    print "patt: "+patt
    pattl = [x.strip() for x in patt.split(',')]
    count = pattl.pop(0)
    #xfor x in pattl:
    color_cmds = zip(pattl[0::3],pattl[1::3],pattl[2::3])
    #print "d: "+str(d)
    for i in range(0,int(count)):
        for (secs,color,ledn) in color_cmds:
            secs = float(secs)
            millis = secs * 1000
            cmd = blink1_tool_path + ' --led '+ ledn +' -m '+ str(millis) + ' --rgb '+ color
            print "cmd: "+cmd
            #os.popen( cmd )
            time.sleep( secs ) 

    
play_pattern('2,   1.0,#ffff00,0, 0.5,#00ff00,1')


#
# main loop
#
while True:
    try:
        req = urllib2.Request(url)
        res = urllib2.urlopen(req)
        data = res.read()
    except:
        data = '{ "event_count": 0 }'
        print "event feed not found"
    
    ev = json.loads(data)
 
    if ev['event_count'] > 0:
        print 'events to process:'
        for e in ev['events']:
            print '  ' + str(e)
            if int(e['date']) > last_time:
                rule_name = e['name']
                print 'trigger: ' + rule_name
                if rule_map.has_key( rule_name ) :
                    color = rule_map[ rule_name ]
                    print 'MATCH: ' + color
                    cmd = blink1_tool_path + ' --rgb '+ color 
                    print "cmd: "+ cmd
                    os.popen( cmd )
                    #subprocess.call(['./'+e['name']+'.sh'])

    last_time = calendar.timegm(time.gmtime())
    time.sleep(sleep_time)
  
