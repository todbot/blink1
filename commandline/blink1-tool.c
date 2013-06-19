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
int numDevicesToUse = 1;
int ledn = 0;

hid_device* dev;
uint32_t  deviceIds[blink1_max_devices];

char  cmdbuf[9]; 
char rgbbuf[4];
int verbose;
int quiet;


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
"  --servertickle <1/0>[,1/0]  Turn on/off servertickle (w/on/off, uses -t msec)\n"
"  --list                      List connected blink(1) devices \n"
" Nerd functions: (not used normally) \n"
"  --hidread                   Read a blink(1) USB HID GetFeature report \n"
"  --hidwrite <listofbytes>    Write a blink(1) USB HID SetFeature report \n"
"  --eeread <addr>             Read an EEPROM byte from blink(1)\n"
"  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) \n"
"  --version                   Display blink(1) firmware version \n"
"and [options] are: \n"
"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
"  -g -nogamma                 Disable autogamma correction\n"
"  -m ms,   --millis=millis    Set millisecs for color fading (default 300)\n"
"  -q, --quiet                 Mutes all stdout output (supercedes --verbose)\n"
"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
"  -l <led>, --led <led>       Set which RGB LED in a blink(1) mk2 to use\n"
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
    CMD_LIST,
    CMD_HIDREAD,
    CMD_HIDWRITE,
    CMD_EEREAD,
    CMD_EEWRITE,
    CMD_RGB,
    CMD_SAVERGB,
    CMD_READRGB,
    CMD_SAVEPATTERN,
    CMD_OFF,
    CMD_ON,
    CMD_RED,
    CMD_GRN,
    CMD_BLU,
    CMD_BLINK,
    CMD_GLIMMER,
    CMD_PLAY,
    CMD_RANDOM,
    CMD_RUNNING,
    CMD_VERSION,
    CMD_SERVERDOWN,
    CMD_SERIALNUMREAD,
    CMD_SERIALNUMWRITE,
    CMD_TESTTEST,
};

//
int main(int argc, char** argv)
{
    int openall = 0;
    int nogamma = 0;
    int16_t arg=0;
    static int vid,pid;
    int  rc;
    char tmpstr[100];
    char serialnumstr[serialstrmax] = {'\0'}; 

    uint16_t seed = time(NULL);
    srand(seed);
    memset( cmdbuf, 0, sizeof(cmdbuf));

    static int cmd  = CMD_NONE;

    vid = blink1_vid(), pid = blink1_pid();

    // parse options
    int option_index = 0, opt;
    char* opt_str = "aqvhm:t:d:U:u:gl:";
    static struct option loptions[] = {
        {"all",        no_argument,       0,      'a'},
        {"verbose",    optional_argument, 0,      'v'},
        {"quiet",      optional_argument, 0,      'q'},
        {"millis",     required_argument, 0,      'm'},
        {"delay",      required_argument, 0,      't'},
        {"id",         required_argument, 0,      'd'},
        {"led",        required_argument, 0,      'l'},
        {"nogamma",    no_argument,       0,      'g'},
        {"help",       no_argument,       0,      'h'},
        {"list",       no_argument,       &cmd,   CMD_LIST },
        {"hidread",    no_argument,       &cmd,   CMD_HIDREAD },
        {"hidwrite",   required_argument, &cmd,   CMD_HIDWRITE },
        {"eeread",     required_argument, &cmd,   CMD_EEREAD },
        {"eewrite",    required_argument, &cmd,   CMD_EEWRITE },
        {"rgb",        required_argument, &cmd,   CMD_RGB },
        {"savergb",    required_argument, &cmd,   CMD_SAVERGB },
        {"readrgb",    required_argument, &cmd,   CMD_READRGB },
        {"savepattern",no_argument,       &cmd,   CMD_SAVEPATTERN },
        {"off",        no_argument,       &cmd,   CMD_OFF },
        {"on",         no_argument,       &cmd,   CMD_ON },
        {"red",        no_argument,       &cmd,   CMD_RED },
        {"green",      no_argument,       &cmd,   CMD_GRN },
        {"blue",       no_argument,       &cmd,   CMD_BLU},
        {"blink",      required_argument, &cmd,   CMD_BLINK},
        {"glimmer",    required_argument, &cmd,   CMD_GLIMMER},
        {"play",       required_argument, &cmd,   CMD_PLAY},
        {"random",     required_argument, &cmd,   CMD_RANDOM },
        {"running",    required_argument, &cmd,   CMD_RUNNING },
        {"version",    no_argument,       &cmd,   CMD_VERSION },
        {"serialnumread", no_argument,    &cmd,   CMD_SERIALNUMREAD },
        {"serialnumwrite",required_argument, &cmd,CMD_SERIALNUMWRITE },
        {"servertickle", required_argument, &cmd,   CMD_SERVERDOWN },
        {"testtest",   no_argument,       &cmd,   CMD_TESTTEST },
        {"vid",        required_argument, 0,      'U'}, // FIXME: This sucks
        {"pid",        required_argument, 0,      'u'},
        {NULL,         0,                 0,      0}
    };
    while(1) {
        opt = getopt_long(argc, argv, opt_str, loptions, &option_index);
        if (opt==-1) break; // parsed all the args
        switch (opt) {
         case 0:             // deal with long opts that have no short opts
            switch(cmd) { 
            case CMD_RGB:
                hexread(rgbbuf, optarg, sizeof(rgbbuf));
                break;
            case CMD_HIDWRITE:
            case CMD_EEREAD:
            case CMD_EEWRITE:
            case CMD_SAVERGB:
            case CMD_READRGB:
            case CMD_BLINK:
            case CMD_GLIMMER:
            case CMD_RUNNING:
            case CMD_PLAY:
            case CMD_SERVERDOWN:
                hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
                break;
            case CMD_RANDOM:
                //case CMD_SERVERDOWN:
                if( optarg ) 
                    arg = strtol(optarg,NULL,0);   // cmd w/ number arg
                break;
            case CMD_SERIALNUMWRITE:
                strcpy(tmpstr, optarg);
                break;
            case CMD_ON:
                rgbbuf[0] = 255; rgbbuf[1] = 255; rgbbuf[2] = 255;
                break;
            case CMD_OFF:
                rgbbuf[0] = 0; rgbbuf[1] = 0; rgbbuf[2] = 0;
                break;
            case CMD_RED:
                rgbbuf[0] = 255;
                break;
            case CMD_GRN:
                rgbbuf[1] = 255; 
                break;
            case CMD_BLU:
                rgbbuf[2] = 255; 
                break;

            } // switch(cmd)
            break;
        case 'g':
            nogamma = 1;
        case 'a':
            openall = 1;
            break;
        case 'm':
            millis = strtol(optarg,NULL,10);
            break;
        case 't':
            delayMillis = strtol(optarg,NULL,10);
            break;
        case 'l':
            ledn = strtol(optarg,NULL,10);
            break;
        case 'q':
            if( optarg==NULL ) quiet++;
            else quiet = strtol(optarg,NULL,0);
            break;
        case 'v':
            if( optarg==NULL ) verbose++;
            else verbose = strtol(optarg,NULL,0);
            break;
        case 'd':
            if( strcmp(optarg,"all") == 0 ) {
                numDevicesToUse = 0; //blink1_max_devices;
                for( int i=0; i< blink1_max_devices; i++) {
                    deviceIds[i] = i;
                }
            } 
            else if( strlen(optarg) == 8 ) { //  
                deviceIds[0] = strtol( optarg, NULL, 16);
                numDevicesToUse = 1;
                //sprintf( serialnumstr, "%s", optarg);  // strcpy
            } 
            else {
                numDevicesToUse = hexread((uint8_t*)deviceIds,optarg,sizeof(deviceIds));
            }
            break;
        case 'U': 
            vid = strtol(optarg,NULL,0);
            break;
        case 'u':
            pid = strtol(optarg,NULL,0);
            break;
        case 'h':
            usage( "blink1-tool" );
            exit(1);
            break;
        }
    }

    if(argc < 2){
        usage( "blink1-tool" );
        exit(1);
    }

    // debug  (not on Windows though, no getuid())
    /*
    if( 0 ) { 
      uid_t id = getuid();
      printf("userid:%d\n",id);

      wchar_t myser[10];
      dev = blink1_open();
      hid_get_serial_number_string(dev, myser, 10);
      printf("ser:%ls\n",myser);
    }
    */

    // get a list of all devices and their paths
    int count = blink1_enumerateByVidPid(vid,pid);
    if( count == 0 ) {
        if ( !quiet ) {
            printf("no blink(1) devices found\n");
        }
        exit(1);
    }

    if( numDevicesToUse == 0 ) numDevicesToUse = count; 

    if( verbose ) { 
        printf("deviceId[0] = %X\n", deviceIds[0]);
        printf("cached list:\n");
        for( int i=0; i< count; i++ ) { 
            printf("%d: serial: '%s' '%s'\n", i,blink1_getCachedSerial(i), blink1_getCachedPath(i) );
        }
    }

    // actually open up the device to start talking to it
    if(verbose) printf("openById: %X\n", deviceIds[0]);
    dev = blink1_openById( deviceIds[0] );

    if( dev == NULL ) { 
        if ( !quiet ) {
            printf("cannot open blink(1), bad id or serial number\n");
        }
        exit(1);
    }

    if( blink1_isMk2(dev) )  { 
        if( verbose ) printf("blink1(1)mk2 detected. disabling degamma\n");
        blink1_disableDegamma();  
    }
    // then for original mk1 owners who want to disable degamma
    if( nogamma ) {      //FIXME: confusing
        if ( !quiet ) {
            printf("disabling auto degamma\n");
        }
        blink1_disableDegamma();  
    }


    if( cmd == CMD_LIST ) { 
        printf("blink(1) list: \n");
        for( int i=0; i< count; i++ ) {
            printf("id:%d - serialnum:%s %s\n", i, blink1_getCachedSerial(i), 
                   (blink1_isMk2ById(i)) ? "(mk2)":"");
        }
    }
    else if( cmd == CMD_HIDREAD ) { 
        printf("hidread:  ");
        int len = sizeof(cmdbuf);
        if((rc = hid_get_feature_report(dev, cmdbuf, len)) == -1){
            fprintf(stderr,"error reading data: %s\n",blink1_error_msg(rc));
        } else {
            hexdump(cmdbuf, sizeof(cmdbuf));
        }
    } 
    else if( cmd == CMD_HIDWRITE ) {
        if ( !quiet ) {
            printf("hidwrite: "); hexdump(cmdbuf,sizeof(cmdbuf));
        }
        if((rc = hid_send_feature_report(dev, cmdbuf, sizeof(cmdbuf))) == -1) {
            fprintf(stderr,"error writing data: %d\n",rc);
        }
    }
    else if( cmd == CMD_EEREAD ) {  // FIXME
        printf("eeread:  addr 0x%2.2x = ", cmdbuf[0]);
        uint8_t val = 0;
        rc = blink1_eeread(dev, cmdbuf[0], &val );
        if( rc==-1 ) { // on error
            printf("error\n");
        } else { 
            printf("%2.2x\n", val);
        }
    }
    else if( cmd == CMD_EEWRITE ) {  // FIXME
        if ( !quiet ) { printf("eewrite: \n"); }
        rc = blink1_eewrite(dev, cmdbuf[0], cmdbuf[1] );
        if( rc==-1  && !quiet ) { // error
            printf("error\n");
        }
    }
    else if( cmd == CMD_VERSION ) { 
        printf("firmware version: ");
        rc = blink1_getVersion(dev);
        printf("%d\n", rc );
    }
    else if( cmd == CMD_RGB || cmd == CMD_ON  || cmd == CMD_OFF ||
             cmd == CMD_RED || cmd == CMD_BLU || cmd ==CMD_GRN ) { 
        blink1_close(dev); // close global device, open as needed
        
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];

        for( int i=0; i< numDevicesToUse; i++ ) {
            dev = blink1_openById( deviceIds[i] );
            if( dev == NULL ) continue;
            if ( !quiet ) {
                printf("set dev:%X to rgb:0x%2.2x,0x%2.2x,0x%2.2x over %d msec\n",
                       deviceIds[i],r,g,b,millis);
            }
            if( ledn==0 ) {
                rc = blink1_fadeToRGB(dev,millis, r,g,b);
            } else {
                rc = blink1_fadeToRGBN(dev,millis,r,g,b, ledn);
            }
            if( rc == -1 && !quiet ) { // on error, do something, anything. come on.
                printf("error on fadeToRGB\n");
            }
            blink1_close( dev );
        }
    }
    else if( cmd == CMD_PLAY ) { 
        uint8_t play = cmdbuf[0];
        uint8_t pos = cmdbuf[1];

        if ( !quiet ) { printf("%s color pattern at pos %d\n", ((play)?"playing":"stopping"),pos); }
        rc = blink1_play(dev, play, pos);
        if( rc == -1 && !quiet ) { 
        }
    }
    else if( cmd == CMD_SAVEPATTERN ) {
        if( !quiet ) { printf("writing pattern to flash\n"); }
        rc = blink1_savePattern(dev);
        if( rc==-1 && !quiet ) {
            printf("error on savePattern\n");
        }
    }
    else if( cmd == CMD_SAVERGB ) {
        uint8_t r = cmdbuf[0];
        uint8_t g = cmdbuf[1];
        uint8_t b = cmdbuf[2];
        uint8_t p = cmdbuf[3];
        if ( !quiet ) { printf("saving rgb: 0x%2.2x,0x%2.2x,0x%2.2x to pos %d\n", r,g,b,p ); }
        rc = blink1_writePatternLine(dev, millis, r,g,b, p );
        if( rc==-1 && !quiet ) {
            printf("error on writePatternLine\n");
        }
    }
    else if( cmd == CMD_READRGB ) { 
        uint8_t p = cmdbuf[0];
        uint8_t r,g,b;
        uint16_t msecs;
        printf("reading rgb at pos %d: ", p );
        rc = blink1_readPatternLine(dev, &msecs, &r,&g,&b, p );
        if( rc==-1 && !quiet ) {
            printf("error on writePatternLine\n");
        }
        printf("r,g,b = %x,%x,%x millis:%d\n", r,g,b, msecs);
    }
    else if( cmd == CMD_RANDOM ) { 
        int cnt = blink1_getCachedCount();
        if( cnt>1 ) blink1_close(dev); // close global device, open as needed
        if ( !quiet ) { printf("random %d times: \n", arg); }
        for( int i=0; i<arg; i++ ) { 
            uint8_t r = rand()%255;
            uint8_t g = rand()%255;
            uint8_t b = rand()%255 ;
            uint8_t id = rand() % blink1_getCachedCount();

            if ( !quiet ) {
                printf("%d: %d/%d : %2.2x,%2.2x,%2.2x \n", 
                       i, id, blink1_getCachedCount(), r,g,b);
            }

            hid_device* mydev = dev;
            if( cnt > 1 ) mydev = blink1_openById( id );
            if( ledn == 0 ) { 
                rc = blink1_fadeToRGB(mydev, millis,r,g,b);
            } else {
                uint8_t n = 1 + rand() % ledn;
                rc = blink1_fadeToRGBN(mydev, millis,r,g,b,n);
            }
            if( rc == -1 && !quiet ) { // on error, do something, anything. come on.
                printf("error during random\n");
                //break;
            }
            if( cnt > 1 ) blink1_close( mydev );
            
            blink1_sleep(delayMillis);
        }
    }
    // this whole thing is a huge mess currently // FIXME
    else if( cmd == CMD_RUNNING ) { 
        if( ledn != 0 ) { 
            uint8_t do_rand = 0;
            uint8_t running_cnt = 0;
            uint8_t leds[ledn][3];
            uint8_t c[3] = { rgbbuf[0], rgbbuf[1], rgbbuf[2]};
            if( c[0] == 0 && c[1] == 0 && c[2] == 0 ) {
                c[0] = rand()%255; c[1] = rand()%255; c[2] = rand()%255; 
                do_rand =1;
            }
            // make gradient
            for( int i=0; i<ledn; i++ ) { 
                leds[i][0] = c[0] * i / ledn;
                leds[i][1] = c[1] * i / ledn;
                leds[i][2] = c[2] * i / ledn;
            }

            while( 1 ) {
                //memcpy( c, leds[0], sizeof(c) );
                //memcpy( leds, leds+1, sizeof(c)*(ledn-1) );
                //memcpy( leds[ledn-1], c, sizeof(c) );
                memcpy( c, leds[ledn-1], sizeof(c) );
                memcpy( leds+1, leds, sizeof(c)*(ledn-1) );
                memcpy( leds[0], c, sizeof(c) );

                // output to string
                for( int n=0; n<ledn; n++ )  { 
                    uint8_t r = leds[n][0];
                    uint8_t g = leds[n][1];
                    uint8_t b = leds[n][2];
                    rc = blink1_fadeToRGBN(dev, millis, r,g,b,n+1 );
                }
                blink1_sleep(delayMillis);

                running_cnt++;
                if( do_rand && running_cnt > (ledn*2) ) { 
                    running_cnt = 0;
                    c[0] = rand()%255; c[1] = rand()%255; c[2] = rand()%255; 
                    for( int i=0; i<ledn; i++ ) { 
                        leds[i][0] = c[0] * i / ledn;
                        leds[i][1] = c[1] * i / ledn;
                        leds[i][2] = c[2] * i / ledn;
                    }
                }
            }

        }
    }
    else if( cmd == CMD_BLINK ) { 
        uint8_t n = cmdbuf[0]; 
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
        if( r == 0 && b == 0 && g == 0 ) {
            r = g = b = 255;
        }
        if ( !quiet ) { printf("blink %d times rgb:%x,%x,%x: \n", n,r,g,b); }
        for( int i=0; i<n; i++ ) { 
            rc = blink1_fadeToRGB(dev, millis,r,g,b);
            blink1_sleep(delayMillis);
            rc = blink1_fadeToRGB(dev, millis,0,0,0);
            blink1_sleep(delayMillis);
        }
    }
    else if( cmd == CMD_GLIMMER ) { 
        uint8_t n = cmdbuf[0]; 
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
        if( r == 0 && b == 0 && g == 0 ) {
            r = g = b = 255;
        }
        if ( !quiet ) { printf("glimmering %d times rgb:%x,%x,%x: \n", n,r,g,b); }
        for( int i=0; i<n; i++ ) { 
            rc = blink1_fadeToRGBN(dev, millis,r,g,b, 1);
            rc = blink1_fadeToRGBN(dev, millis,r/2,g/2,b/2, 2);
            blink1_sleep(delayMillis);
            rc = blink1_fadeToRGBN(dev, millis,r/2,g/2,b/2, 1);
            rc = blink1_fadeToRGBN(dev, millis,r,g,b, 2);
            blink1_sleep(delayMillis);
        }
    }
    else if( cmd == CMD_SERVERDOWN ) { 
        //int on  = arg;
        int on = cmdbuf[0];
        int st = cmdbuf[1];
        if ( !quiet ) {
            printf("setting serverdown %s (at %d millis)\n", 
                   ((on)?"ON":"OFF"), delayMillis);
        }
        blink1_serverdown( dev, on, delayMillis, st );
    }
    // use caution with this, could make your blink(1) unusable
    // --serialnumwrite abcd1234
    else if( cmd == CMD_SERIALNUMWRITE ) { 
        if ( !quiet ) { printf("serial number write: %s\n",tmpstr); }

        //for( int i=0; i<4; i++)  printf("%2.2X,",cmdbuf[i]);
        //printf("\n");
        if( (rc = blink1_serialnumwrite( dev, tmpstr)) == -1 ) { 
            fprintf(stderr,"error writing new serial number: %d\n",rc);
        }
    }
    else if( cmd == CMD_TESTTEST ) { 
        printf("test test\n");
        rc = blink1_testtest(dev);
    }


    return 0;
}


