/* 
 * blinkmusb-tool.c --
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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>    // for getopt_long()
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>    // for usleep()
#endif


#include "blink1-lib.h"


int millis = 300;
int delayMillis = 1000;
int multiMillis = 1000;
int numDevicesToUse = 1;

hid_device* dev;
const wchar_t* dev_serial;
char  deviceIds[blink1_max_devices];

char  cmdbuf[9];    // room at front for reportID
int verbose;


//---------------------------------------------------------------------------- 

// a simple logarithmic -> linear mapping as a sort of gamma correction
// maps from 0-255 to 0-255
//static int log2lin( int n )  
//{
  //return  (int)(1.0* (n * 0.707 ));  // 1/sqrt(2)
//  return (((1<<(n/32))-1) + ((1<<(n/32))*((n%32)+1)+15)/32);
//}

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
"  --hidread                   Read a blink(1) USB HID GetFeature report \n"
"  --hidwrite <listofbytes>    Write a blink(1) USB HID SetFeature report \n"
"  --eeread <addr>             Read an EEPROM byte from blink(1)\n"
"  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) \n"
"  --blink <numtimes>          Blink on/off \n"
"  --random <numtimes>         Flash a number of random colors \n"
"  --rgb <red>,<green>,<blue>  Fade to RGB value\n"
"  --savergb <red>,<grn>,<blu>,<pos> Save RGB value at pos\n" 
"  --serverdown <on/off,<millis>\n"
"  --on                        Turn blink(1) full-on white \n"
"  --off                       Turn blink(1) off \n"
"  --list                      List connected blink(1) devices \n"
"  --version                   Display blink(1) firmware version \n"
"  --serialnumwread            Read current serial number \n"
"  --serialnumwrite <listofchars> Write new serial number (CAUTION)\n"
"and [options] are: \n"
"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
"// --serial <num>              Connect to blin(1) by its serial number \n"  
"  -m ms,   --miilis=millis    Set millisecs for color fading (default 300)\n"
"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
"  --vid=vid --pid=pid         Specifcy alternate USB VID & PID\n"
"  -v, --verbose               verbose debugging msgs\n"
"\n"
"Examples \n"
"  blink1-tool -m 100 --rgb 255,0,255   # fade to #FF00FF in 0.1 seconds \n"
"\n"
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
    CMD_OFF,
    CMD_ON,
    CMD_RANDOM,
    CMD_VERSION,
    CMD_SERVERDOWN,
    CMD_SERIALNUMREAD,
    CMD_SERIALNUMWRITE,
};

//
int main(int argc, char** argv)
{
    int openall = 0;
    int16_t arg=0;
    static int vid,pid;
    int  rc;
    char tmpstr[100];

    uint16_t seed = time(NULL);
    srand(seed);

    static int cmd  = CMD_NONE;

    vid = blink1_vid(), pid = blink1_pid();

    // parse options
    int option_index = 0, opt;
    char* opt_str = "avm:t:d:U:u:";
    static struct option loptions[] = {
        {"all",        no_argument,       0,      'a'},
        {"verbose",    optional_argument, 0,      'v'},
        {"millis",     required_argument, 0,      'm'},
        {"delay",      required_argument, 0,      't'},
        {"id",         required_argument, 0,      'd'},
        {"list",       no_argument,       &cmd,   CMD_LIST },
        {"hidread",    no_argument,       &cmd,   CMD_HIDREAD },
        {"hidwrite",   required_argument, &cmd,   CMD_HIDWRITE },
        {"eeread",     required_argument, &cmd,   CMD_EEREAD },
        {"eewrite",    required_argument, &cmd,   CMD_EEWRITE },
        {"rgb",        required_argument, &cmd,   CMD_RGB },
        {"savergb",    required_argument, &cmd,   CMD_SAVERGB },
        {"off",        no_argument,       &cmd,   CMD_OFF },
        {"on",         no_argument,       &cmd,   CMD_ON },
        {"random",     required_argument, &cmd,   CMD_RANDOM },
        {"version",    no_argument,       &cmd,   CMD_VERSION },
        {"serialnumread", no_argument,    &cmd,   CMD_SERIALNUMREAD },
        {"serialnumwrite",required_argument, &cmd,CMD_SERIALNUMWRITE },
        {"serverdown", required_argument, &cmd,   CMD_SERVERDOWN },
        {"vid",        required_argument, 0,      'U'}, // FIXME: This sucks
        {"pid",        required_argument, 0,      'u'},
        //{"serial",     required_argument, 0,      'z'},
        {NULL,         0,                 0,      0}
    };
    while(1) {
        opt = getopt_long(argc, argv, opt_str, loptions, &option_index);
        if (opt==-1) break; // parsed all the args
        switch (opt) {
         case 0:             // deal with long opts that have no short opts
            switch(cmd) { 
            case CMD_HIDWRITE:
            case CMD_EEREAD:
            case CMD_EEWRITE:
            case CMD_RGB:
            case CMD_SAVERGB:
                hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
                break;
            case CMD_RANDOM:
            case CMD_SERVERDOWN:
                if( optarg ) 
                    arg = strtol(optarg,NULL,0);   // cmd w/ number arg
                break;
            case CMD_SERIALNUMWRITE:
                strcpy(cmdbuf, optarg);
                break;
            } // switch(cmd)
            break;
        case 'a':
            openall = 1;
            break;
        case 'm':
            millis = strtol(optarg,NULL,10);
            break;
        case 't':
            delayMillis = strtol(optarg,NULL,10);
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
            } else {
                numDevicesToUse = hexread(deviceIds,optarg,sizeof(deviceIds));
            }
            break;
        case 'U': 
            vid = strtol(optarg,NULL,0);
            break;
        case 'u':
            pid = strtol(optarg,NULL,0);
            break;
        }
    }

    if(argc < 2){
        usage( "blink1-tool" );
        exit(1);
    }

    // get a list of all devices and their paths
    int count = blink1_enumerateByVidPid(vid,pid);
    if( count == 0 ) { 
        printf("no blink(1) devices found\n");
        exit(1);
    }
    if( numDevicesToUse == 0 ) numDevicesToUse = count; 

    if( !dev_serial ) 
        dev_serial = blink1_getCachedSerial( deviceIds[0] );

    if( verbose ) { 
        printf("deviceId[0] = %d\n", deviceIds[0]);
        printf("cached path = '%ls'\n", dev_serial);
        for( int i=0; i< count; i++ ) { 
            printf("%d: serial: '%ls'\n", i,blink1_getCachedSerial(i) );
        }
    }

    // actually open up the device to start talking to it
    dev = blink1_openById( deviceIds[0] );
    if( dev == NULL ) { 
        printf("cannot open blink(1), bad serial number\n");
        exit(1);
    }

    if( cmd == CMD_LIST ) { 
        printf("blink(1) list: \n");
        for( int i=0; i< count; i++ ) { 
            printf("id:%d - serialnum:%ls\n", i, blink1_getCachedSerial(i) );
        }
    }
    /*
    else if( cmd == CMD_SERIAL ) { 
        char buf[8];
        blink1_getSerialNumber(dev,buf);
        printf("serial number: %s\n", buf);
        }*/
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
        printf("hidwrite: "); hexdump(cmdbuf,sizeof(cmdbuf));
        //memmove( cmdbuf+1, cmdbuf, sizeof(cmdbuf)-1 );
        //printf("hidwrite: "); hexdump(cmdbuf,sizeof(cmdbuf));
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
        printf("eewrite: \n");
        rc = blink1_eewrite(dev, cmdbuf[0], cmdbuf[1] );
        if( rc==-1 ) { // error
        }
    }
    else if( cmd == CMD_VERSION ) { 
        printf("version: ");
        rc = blink1_getVersion(dev);
        printf("%d\n", rc );
    }
    else if( cmd == CMD_RGB ) { 
        blink1_close(dev); // close global device, open as needed

        uint8_t r = cmdbuf[0];
        uint8_t g = cmdbuf[1];
        uint8_t b = cmdbuf[2];

        //int rn = blink1_degamma(r); //log2lin( r );
        //int gn = blink1_degamma(g); //log2lin( g );
        //int bn = blink1_degamma(b); //log2lin( b );

        for( int i=0; i< numDevicesToUse; i++ ) {
            dev_serial = blink1_getCachedSerial( deviceIds[i] );
            dev = blink1_openById( deviceIds[i] );
            if( dev == NULL ) continue;
            printf("set %d rgb: 0x%2.2x,0x%2.2x,0x%2.2x\n",deviceIds[i],r,g,b);
            rc = blink1_fadeToRGB(dev,millis, r,g,b);
            if( rc == -1 ) { // on error, do something, anything. come on.
                printf("error on fadeToRGB\n");
            }
        }
    }
    else if( cmd == CMD_SAVERGB ) {
        uint8_t r = cmdbuf[0];
        uint8_t g = cmdbuf[1];
        uint8_t b = cmdbuf[2];
        uint8_t p = cmdbuf[3];
        printf("saving rgb: 0x%2.2x,0x%2.2x,0x%2.2x to pos %d\n", r,g,b,p );
        rc = blink1_writePatternLine(dev, millis, r,g,b, p );
        if( rc ) { 
            printf("error on writePatternLine\n");
        }
    }
    else if( cmd == CMD_ON ) {
        blink1_close(dev); // close global device, open as needed
        for( int i=0; i< numDevicesToUse; i++ ) {
            int id = deviceIds[i];
            dev_serial = blink1_getCachedSerial( id );
            dev = blink1_openById( id );
            if( dev == NULL ) { 
                printf("error opening blink1 #%d\n",id);
            }
            printf("turning on device %d ('%ls')\n", id, dev_serial);
            rc = blink1_fadeToRGB(dev, millis, 255,255,255);
            if( rc == -1 ) {// on error, do something, anything. come on.
                printf("error on ON fadeToRGB\n");
            }
            blink1_close( dev );
        }
    }
    else if( cmd == CMD_OFF ) { 
        blink1_close(dev); // close global device, open as needed
        for( int i=0; i< numDevicesToUse; i++ ) {
            int id = deviceIds[i];
            dev_serial = blink1_getCachedSerial( id );
            dev = blink1_openById( id );
            if( dev == NULL ) {
                printf("error opening blink1 #%d\n",id);
            }
            printf("turning off device %d ('%ls')\n", id, dev_serial);
            rc = blink1_fadeToRGB(dev, millis, 0,0,0);
            if( rc == -1) { // on error, do something, anything. come on.
                printf("error on OFF fadeToRGB\n");
            }
            blink1_close( dev );
        }
    }
    else if( cmd == CMD_RANDOM ) { 
        printf("random %d times: \n", arg);
        for( int i=0; i<arg; i++ ) { 
            uint8_t r = rand()%255;
            uint8_t g = rand()%255;
            uint8_t b = rand()%255 ;
            uint8_t id = rand() % blink1_getCachedCount();

            printf("%d: %d : %2.2x,%2.2x,%2.2x \n", i, id, r,g,b);

            hid_device* mydev = blink1_openById( id );
            rc = blink1_fadeToRGB(mydev, millis,r,g,b);
            if( rc == -1 ) { // on error, do something, anything. come on.
                //break;
            }
            blink1_close(mydev);

#ifdef WIN32
            Sleep(delayMillis);
#else 
            usleep( delayMillis * 1000);
#endif
        }
    }
    else if( cmd == CMD_SERVERDOWN ) { 
        int on  = arg;
        printf("setting serverdown to %d (at %d millis)\n", on, delayMillis);
        blink1_serverdown( dev, on, delayMillis );
    }
    else if( cmd == CMD_SERIALNUMWRITE ) { 
        printf("serial number write: %s\n", cmdbuf);
        if( (rc = blink1_serialnumwrite( dev, cmdbuf)) == -1 ) { 
            fprintf(stderr,"error writing new serial number: %d\n",rc);
        }
    }

    return 0;
}



            /* 
            printf("%d : %2.2x,%2.2x,%2.2x \n", numDevicesToUse, r,g,b);
            for( int i=0; i< numDevicesToUse; i++ ) {
                hid_device* mydev = blink1_open_bypath( blink1_cached_path(i) );
                rc = blink1_fadeToRGB(dev, millis,r,g,b);
                if( rc == -1 ) { // on error, do something, anything. come on.
                    break;
                }
                blink1_close(mydev)
            }
            */
