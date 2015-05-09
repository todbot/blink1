#! /usr/bin/python

# from: https://gist.github.com/dansherman/4488436

import sys
from time import sleep
import subprocess
from subprocess import call
import logging
import threading
import urllib
import json
from colorsys import hsv_to_rgb
from math import exp
 
blink_list = {'ping':[255,255,255], 'temp':[255,255,255]} # Default list of blinks

blink1_path = "/usr/local/bin/blink1-tool" # path to blink1-tool

milli = 1000 # duration for blink commands
ping_host = "8.8.8.8" # ip address for ping test
wunderground_api_key = 'KEY' # wunderground api key: get it free here http://www.wunderground.com/weather/api/
weather_zip = 'ZIP'
 
# Debugging config, used during testing
logging.basicConfig(level=logging.DEBUG,
                    format='[%(levelname)s] (%(threadName)-10s) %(message)s',
                    )
 
 
# Wrapper for blink1-tool, only rgb and off commands supported
class Blink(object):
  # takes rgb values in a list
  def rgb(self,rgb,m=milli):
    subprocess.check_call([blink1_path,"--rgb", ','.join(str(x) for x in rgb),"-m",str(milli)], stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 
  # shuts off the light
  def off(self,m=milli):
    subprocess.check_call([blink1_path,"--off","-m",str(milli)], stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 
 
def check_weather():
  # uses wunderground api to get the current "feels like" temperature
  # the temp is curved for a better sense of blue=cold/red=hot/green=comfortable
  # a-e set the parameters for the curve
  # minval and maxval set the expected range
  a = 100
  b = 1
  c = 6
  d = 10
  e = 1.2
  minval = 0
  maxval = 100
  while True:
    #try:
    # fetch the data
    api = json.loads(urllib.urlopen('http://api.wunderground.com/api/%s/conditions/q/%s.json' % (wunderground_api_key, weather_zip)).read())
    # extract the temp
    temp = int(api['current_observation']['feelslike_f'])
    logging.debug("Got temperature of %s" % str(temp))
    # curve the temp
    curved_temp = a/(b+exp(c-(e*temp/10)))
    logging.debug("Curved temperature to %s" % str(curved_temp))
    # assign the curved temp to a hue
    h = (float(maxval-curved_temp) / (maxval-minval)) * 300
    # get the rgb value from the hue
    r, g, b = hsv_to_rgb(h/360, 1., 1.)
    temp_rgb = [r*255, g*255, b*255]
    # set the new color in the blink_list
    blink_list['temp'] = temp_rgb 
    #except:
      #logging.warning("Error getting weather.")
    sleep(300)
 
 
# checks for a ping to ping_host, sets a faint white if success, bright red on fail, retrys every 5 seconds
def check_ping():
  while True:
    try:
      logging.debug("Pinging " + ping_host)
      subprocess.check_call(["ping", "-c", "1", ping_host], stdout=subprocess.PIPE,stderr=subprocess.PIPE)
      logging.debug("Pinged!")
      blink_list['ping'] = [128,128,128]
    except subprocess.CalledProcessError:
      logging.debug("Ping error")
      blink_list['ping'] = [255,0,0]
    sleep(5)
 
def blinker(argv=None):
  logging.debug("Starting blinker()")
  # start a new blink instance
  b = Blink()
  if argv is None:
    argv = sys.argv
  # loop through blink_list
  for key in blink_list.keys():
    val = blink_list[key]
    logging.debug(key + ":" + str(val))
    # set the light
    b.rgb(val)
    logging.debug("holding for 2")
    # hold for 2 seconds
    sleep(2)
  #turn the light off
  b.off()
 
def main():
  # define and spawn the daemon threads to check the weather and pings
  p = threading.Thread(name="check_ping", target=check_ping)
  w = threading.Thread(name="check_weather", target=check_weather)
  p.setDaemon(True)
  w.setDaemon(True)
  p.start()
  w.start()
  # start the blinker loop the pattern every four seconds
  while True:
    blinker()
    logging.debug("sleeping for 4")
    sleep(4)
 
if __name__=="__main__":
  sys.exit(main())
  
