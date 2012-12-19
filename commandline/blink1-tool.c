/* 
 * blink1-tool.c -- command-line tool for controlling blink(1) usb rgb leds
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 *
 * Fade to RGB value #FFCC33 in 50 msec:
 * ./blink1-tool --hidwrite 0x63,0xff,0xcc,0x33,0x00,0x32
 * ./blink1-tool -m 50 -rgb 0xff,0xcc,0x33
 *
 * Read EEPROM position 1:
 * ./blink1-tool --hidwrite 0x65,0x01 && ./blink1-tool --hidread
 * ./blink1-tool --eeread 1
 *
 * Write color pattern entry {#EEDD44, 50} to position 2
 * ./blink1-tool --hidwrite 0x50,0xee,0xdd,0x44,0x00,0x32,0x2
 *
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>    // for memset(), strcmp(), et al
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>    // for getopt_long()
#include <time.h>
#include <unistd.h>    // getuid()

#include "blink1-lib.h"


int millis = 300;
int delayMillis = 500;
int countDevices = 0;   /* total devices found */
uint8_t currentR = 255;
uint8_t currentG = 255;
uint8_t currentB = 255;

hid_device **dev;
//const wchar_t* dev_serial;
char  deviceIds[blink1_max_devices];

char cmdbuf[9];
char rgbbuf[4];
int verbose;


//---------------------------------------------------------------------------- 
/*
  TBD: replace printf()s with something like this
void logpri(int loglevel, char* fmt, ...)
{
    if( loglevel < verbose ) return;
    va_list ap;
    va_start(ap,fmt);
    vprintf(fmt,ap);
    va_end(ap);
}
*/


//
static void hexdump(char *buffer, int len)
{
int     i;
FILE    *fp = stdout;

    for(i = 0; i < len; i++){
        if(i != 0){
            if(i % 16 == 0){
                fprintf(fp, "\n");
            }else{
                fprintf(fp, " ");
            }
        }
        fprintf(fp, "0x%02x", buffer[i] & 0xff);
    }
    if(i != 0)
        fprintf(fp, "\n");
}

//
static int  hexread(char *buffer, char *string, int buflen)
{
char    *s;
int     pos = 0;

    while((s = strtok(string, ", ")) != NULL && pos < buflen){
        string = NULL;
        buffer[pos++] = (char)strtol(s, NULL, 0);
    }
    return pos;
}


// --------------------------------------------------------------------------- 

//
static void usage(char *myName)
{
    fprintf(stderr,
"Usage: \n"
"  %s <cmd> [options]\n"
"where <cmd> is one of:\n"
"  --blink <numtimes>          Blink on/off (specify --rgb before to blink a color)\n"
"  --random <numtimes>         Flash a number of random colors \n"
"  --rgb <red>,<green>,<blue>  Fade to RGB value\n"
"  --on                        Turn blink(1) full-on white \n"
"  --off                       Turn blink(1) off \n"
"  --red                       Turn blink(1) red \n"
"  --green                     Turn blink(1) green \n"
"  --blue                      Turn blink(1) blue \n"
"  --savergb <r>,<g>,<b>,<pos> Write pattern RGB value at pos\n" 
"  --readrgb <pos>             Read pattern RGB value at pos\n" 
"  --play <1/0,pos>            Start playing color sequence (at pos)\n"
"  --servertickle <1/0>        Turn on/off servertickle (uses -t msec) \n"
"  --list                      List connected blink(1) devices \n"
" Nerd functions: (not used normally) \n"
"  --hidread                   Read a blink(1) USB HID GetFeature report \n"
"  --hidwrite <listofbytes>    Write a blink(1) USB HID SetFeature report \n"
"  --eeread <addr>             Read an EEPROM byte from blink(1)\n"
"  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) \n"
"  --version                   Display blink(1) firmware version \n"
"and [options] are: \n"
"  -g -nogamma                 Disable autogamma correction\n"
"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
"  -m ms,   --millis=millis    Set millisecs for color fading (default 300)\n"
"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
"  --vid=vid --pid=pid         Specifcy alternate USB VID & PID\n"
"  -v, --verbose               verbose debugging msgs\n"
"\n"
"Examples \n"
"  blink1-tool -m 100 --rgb 255,0,255    # fade to #FF00FF in 0.1 seconds \n"
"  blink1-tool -t 2000 --random 100      # every 2 seconds new random color\n"
"  blink1-tool --rgb 0xff,0,00 --blink 3 # blink red 3 times\n"
"\n"
            ,myName);
}

// local states for the "cmd" option variable
enum { 
    CMD_NONE = 0,
    CMD_HELP,
    CMD_LIST,
    CMD_HIDREAD,
    CMD_HIDWRITE,
    CMD_EEREAD,
    CMD_EEWRITE,
    CMD_RGB,
    CMD_SAVERGB,
    CMD_READRGB,
    CMD_DUMPRGB,
    CMD_OFF,
    CMD_ON,
    CMD_RED,
    CMD_GRN,
    CMD_BLU,
    CMD_BLINK,
    CMD_PLAY,
    CMD_RANDOM,
    CMD_VERSION,
    CMD_SERVERDOWN,
    CMD_SERIALNUMREAD,
    CMD_SERIALNUMWRITE,
};

#define FOREACH(block) \
  for( int i=0; i< countDevices; i++ ) { \
    if (deviceIds[i]) { \
      if( dev[i] == NULL ) continue; \
      block \
    } \
  }

static void set_rgb (uint8_t r, uint8_t g, uint8_t b) {
  int rc;
  FOREACH ({
      printf("dev:%d set to rgb:0x%2.2x,0x%2.2x,0x%2.2x over %d msec\n",
             i,r,g,b,millis);
      rc = blink1_fadeToRGB (dev[i], millis, r, g, b);
      if( rc == -1 ) { // on error, do something, anything. come on.
          printf("error on fadeToRGB\n");
      }
  })

  blink1_sleep( delayMillis );
}

static void setSpecificDeviceOn (int i) {
  if ( i < countDevices ) {
    deviceIds[i] = 1;
  }
}

static void setSpecificDeviceOff (int i) {
  if ( i < countDevices ) {
    deviceIds[i] = 0;
  }
}

static void setAllDevicesOn () {
  /* actually just set defined devices to enabled */
  for( int i=0; i< blink1_max_devices; i++ ) {
    if (i < countDevices) {
      deviceIds[i] = 1;
    } else {
      deviceIds[i] = 0;
    }
  }
}

static void setAllDevicesOff () {
  /* actually just set defined devices to enabled */
  for( int i=0; i< blink1_max_devices; i++ ) {
    deviceIds[i] = 0;
  }
}

//
int main(int argc, char** argv)
{
    int nogamma = 0;
    int16_t arg=0;
    static int vid,pid;
    int  rc;
    char tmpstr[100];

    uint16_t seed = time(NULL);
    srand(seed);

    static int cmd  = CMD_NONE;


    if(argc < 2){
        usage( "blink1-tool" );
        exit(1);
    }

    // FIXME: not sure how these work, so not sure if just overriding them
    // below does as expected
    vid = blink1_vid();
    pid = blink1_pid();

    // parse options
    int option_index = 0, opt;
    char* opt_str = "avm:t:d:U:u:g";
    static struct option loptions[] = {
        {"all",        no_argument,       0,      'a'},
        {"verbose",    optional_argument, 0,      'v'},
        {"millis",     required_argument, 0,      'm'},
        {"delay",      required_argument, 0,      't'},
        {"id",         required_argument, 0,      'd'},
        {"nogamma",    no_argument,       0,      'g'},
        {"help",       no_argument,       &cmd,   CMD_HELP },
        {"list",       no_argument,       &cmd,   CMD_LIST },
        {"hidread",    no_argument,       &cmd,   CMD_HIDREAD },
        {"hidwrite",   required_argument, &cmd,   CMD_HIDWRITE },
        {"eeread",     required_argument, &cmd,   CMD_EEREAD },
        {"eewrite",    required_argument, &cmd,   CMD_EEWRITE },
        {"rgb",        required_argument, &cmd,   CMD_RGB },
        {"savergb",    required_argument, &cmd,   CMD_SAVERGB },
        {"readrgb",    required_argument, &cmd,   CMD_READRGB },
        {"dumprgb",    no_argument,       &cmd,   CMD_DUMPRGB },
        {"off",        no_argument,       &cmd,   CMD_OFF },
        {"on",         no_argument,       &cmd,   CMD_ON },
        {"red",        no_argument,       &cmd,   CMD_RED },
        {"green",      no_argument,       &cmd,   CMD_GRN },
        {"blue",       no_argument,       &cmd,   CMD_BLU},
        {"blink",      required_argument, &cmd,   CMD_BLINK},
        {"play",       required_argument, &cmd,   CMD_PLAY},
        {"random",     required_argument, &cmd,   CMD_RANDOM },
        {"version",    no_argument,       &cmd,   CMD_VERSION },
        {"serialnumread", no_argument,    &cmd,   CMD_SERIALNUMREAD },
        {"serialnumwrite",required_argument, &cmd,CMD_SERIALNUMWRITE },
        {"servertickle", required_argument, &cmd, CMD_SERVERDOWN },
        {"vid",        required_argument, 0,      'U'}, // FIXME: This sucks
        {"pid",        required_argument, 0,      'u'},
        {NULL,         0,                 0,      0}
    };

    /* parse global options */
    while(1) {
      opt = getopt_long(argc, argv, opt_str, loptions, &option_index);
      if (opt==-1) break; // parsed all the args
      switch (opt) {
        case 'U':
          vid = strtol(optarg,NULL,0);
          break;
        case 'u':
          pid = strtol(optarg,NULL,0);
          break;
        case 'g':
          printf("disabling auto degamma\n");
          blink1_disableDegamma();
          break;
        case 'v':
          if( optarg==NULL ) verbose++;
          else verbose = strtol(optarg,NULL,0);
          break;

        default: // skip anything else
          break;
      }
    }

    // get a list of all devices and their paths
    countDevices = blink1_enumerateByVidPid(vid,pid);
    if( countDevices == 0 ) {
      printf("no blink(1) devices found\n");
      exit(1);
    }
    /* initialize all devices to NULL */
    dev = (hid_device **)malloc (blink1_max_devices * sizeof (hid_device *));
    for (int i = 0; i < blink1_max_devices; i++)
      {
        dev[i] = NULL;
      }

    setAllDevicesOn();

    // open all devices
    for( int i=0; i< countDevices; i++ ) {
      dev[i] = blink1_openById (i);
      if( dev[i] == NULL ) {
        printf("cannot open blink(1), bad serial number\n");
        exit(1);
      }
    }

    /* reset options and start running commands */
    optreset=1;
    optind=1;
    while(1) {
        opt = getopt_long(argc, argv, opt_str, loptions, &option_index);
        if (opt==-1) break; // parsed all the args
        memset( cmdbuf, 0, sizeof(cmdbuf));
        switch (opt) {
         case 0:             // deal with long opts that have no short opts
            switch(cmd) {

            case CMD_HELP:
              usage( "blink1-tool" );
              exit(1);
              break;

            case CMD_LIST:
              FOREACH({
                printf("dev:%d has serialnum:%ls\n",
                       i, blink1_getCachedSerial(i));
              })
              break;

            case CMD_VERSION:
              FOREACH({
                printf("dev:%d has firmware:%d\n",
                       i, blink1_getVersion(dev[i]));
              })
              break;

            case CMD_RGB:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));
              currentR = cmdbuf[0];
              currentG = cmdbuf[1];
              currentB = cmdbuf[2];
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_ON:
              currentR = 255;
              currentG = 255;
              currentB = 255;
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_OFF:
              currentR = 0;
              currentG = 0;
              currentB = 0;
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_RED:
              currentR = 255;
              currentG = 0;
              currentB = 0;
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_GRN:
              currentR = 0;
              currentG = 255;
              currentB = 0;
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_BLU:
              currentR = 0;
              currentG = 0;
              currentB = 255;
              set_rgb (currentR, currentG, currentB);
              break;

            case CMD_BLINK:
              {
                hexread(cmdbuf, optarg, sizeof(cmdbuf));
                uint8_t n = cmdbuf[0];
                printf("blink %d times rgb:0x%02x,0x%02x,0x%02x: \n",
                       n,currentR,currentG,currentB);
                for( int i=0; i<n; i++ ) {
                  set_rgb (currentR, currentG, currentB);
                  set_rgb (0, 0, 0);
                }
              }
              break;

            case CMD_RANDOM:
              {
                hexread(cmdbuf, optarg, sizeof(cmdbuf));
                uint8_t n = cmdbuf[0];
                printf("random %d times: \n", n);
                for( int i=0; i<n; i++ ) {
                  /* pick random color */
                  currentR = rand()%255;
                  currentG = rand()%255;
                  currentB = rand()%255;
                  /* pick random device */
                  uint8_t id = rand() % countDevices;
                  setAllDevicesOff();
                  setSpecificDeviceOn(id);
                  set_rgb(currentR, currentG, currentB);
                }
                /* clear all devices of color */
                setAllDevicesOn();
                set_rgb (0, 0, 0);
              }
              break;

            case CMD_HIDREAD:
              FOREACH({
                int len = sizeof(cmdbuf);
                if ((rc = hid_get_feature_report(dev[i], cmdbuf, len)) == -1) {
                  fprintf(stderr,"error reading data: %s\n",
                          blink1_error_msg (rc));
                } else {
                  printf("dev:%d hidread ",i);
                  hexdump(cmdbuf, sizeof(cmdbuf));
                }
              })
              break;

            case CMD_HIDWRITE:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));
              FOREACH({
                printf("dev:%d hidwrite ", i);
                hexdump(cmdbuf,sizeof(cmdbuf));
                int len = sizeof(cmdbuf);
                if ((rc = hid_send_feature_report(dev[i], cmdbuf, len)) == -1) {
                  fprintf(stderr,"error writing data: %s\n",
                          blink1_error_msg (rc));
                }
              })
              break;

            case CMD_EEREAD:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
              FOREACH({
                printf("dev:%d eeread:  addr 0x%2.2x = ", i, cmdbuf[0]);
                uint8_t val = 0;
                rc = blink1_eeread(dev[i], cmdbuf[0], &val );
                if( rc==-1 ) { // on error
                    printf("error\n");
                } else { 
                    printf("%2.2x\n", val);
                }
              })
              break;

            case CMD_EEWRITE:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
              FOREACH({
                printf("dev:%d eewrite: ", i);
                rc = blink1_eewrite(dev[i], cmdbuf[0], cmdbuf[1] );
                if( rc==-1 ) { // error
                  printf("error\n");
                } else {
                  printf("ok\n");
                }
              })
              break;

            case CMD_SAVERGB:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));
              FOREACH({
                uint8_t r = cmdbuf[0];
                uint8_t g = cmdbuf[1];
                uint8_t b = cmdbuf[2];
                uint8_t p = cmdbuf[3];
                printf("dev:%d saving rgb: 0x%2.2x,0x%2.2x,0x%2.2x "
                       "to pos %d\n", i,r,g,b,p );
                rc = blink1_writePatternLine(dev[i], millis, r,g,b, p );
                if( rc==-1 ) {
                    printf("error on writePatternLine\n");
                }
              })
              break;

            case CMD_READRGB:
              {
                hexread(cmdbuf, optarg, sizeof(cmdbuf));
                uint8_t p = cmdbuf[0];
                uint8_t r,g,b;
                uint16_t msecs;

                FOREACH({
                  rc = blink1_readPatternLine(dev[i], &msecs, &r,&g,&b, p );
                  if( rc==-1 ) {
                      printf("error on writePatternLine\n");
                  }
                  printf ("dev:%d pos:%d millis:%d "
                            "color:0x%2.2x,0x%2.2x,0x%2.2x\n",
                            i, p, msecs, r, g, b);
                })
              }
              break;

            case CMD_DUMPRGB:
              {
                uint8_t r,g,b;
                uint16_t msecs;
                FOREACH({
                  for (int p = 0; p < 255 ; p++) {
                    if (blink1_readPatternLine(dev[i], &msecs, &r,&g,&b, p )==-1){
                      printf("error on blink1_readPatternLine\n");
                    }
                    printf ("dev:%d pos:%d millis:%d "
                            "color:0x%2.2x,0x%2.2x,0x%2.2x\n",
                            i, p, msecs, r, g, b);
                  }
                })
              }
              break;

            case CMD_PLAY:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));
              FOREACH({
                uint8_t play = cmdbuf[0];
                uint8_t pos = cmdbuf[1];

                printf("dev:%d %s color pattern at pos %d\n",
                       i, ((play)?"playing":"stopping"),pos);
                blink1_play (dev[i], play, pos);
              })
              break;

            case CMD_SERVERDOWN:
              hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
              int on  = cmdbuf[0];
              FOREACH({
                printf("dev:%d setting serverdown %s (at %d millis)\n",
                       i,((on)?"ON":"OFF"), delayMillis);
                blink1_serverdown( dev[i], on, delayMillis );
              })
              break;

            case CMD_SERIALNUMWRITE:
              // use caution with this, could make your blink(1) unusable
              // --serialnumwrite abcd1234
              strcpy(tmpstr, optarg);

              FOREACH({
                printf("serial number write: %s\n",tmpstr);
                if ((rc = blink1_serialnumwrite( dev[i], tmpstr)) == -1 ) { 
                  fprintf(stderr,"error writing new serial number: %d\n",rc);
                }
              })
              break;
            } // switch(cmd)
            break;

        case 'a':
          /* enable all devices */
          setAllDevicesOn();
          break;
        case 'm':
          millis = strtol(optarg,NULL,10);
          break;
        case 't':
          delayMillis = strtol(optarg,NULL,10);
          break;
        case 'd':
          if( strcmp(optarg,"all") == 0 ) {
            setAllDevicesOn();
          } else {
            /* enable only device specified in arg */
            hexread(cmdbuf, optarg, sizeof(cmdbuf));
            setAllDevicesOff();
            setSpecificDeviceOn (cmdbuf[0]);
          }
          break;
        default:
          break;
        }
    }

    /* segfaults if I try to close?
    for( int i=0; i < countDevices; i++ ) {
      if (dev[i] != NULL) {
        blink1_close (dev[i]);
      }
    }
    */
    free (dev);

    return 0;
}


