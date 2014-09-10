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
#include <stdarg.h>    // vararg stuff
#include <string.h>    // for memset(), strcmp(), et al
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>    // for getopt_long()
#include <time.h>
#include <unistd.h>    // getuid()

#include "blink1-lib.h"

#ifndef BLINK1_VERSION
#define BLINK1_VERSION "v0.0"
#endif


int millis = 300;
int delayMillis = 500;
int numDevicesToUse = 1;
int ledn = 0;

blink1_device* dev;
uint32_t  deviceIds[blink1_max_devices];

uint8_t cmdbuf[blink1_buf_size]; 
uint8_t rgbbuf[3];
int verbose;
int quiet=0;


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

// printf that can be shut up
void msg(char* fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    if( !quiet ) {
        vprintf(fmt,args);
    }
    va_end(args);
}

// take an array of bytes and spit them out as a hex string
static void hexdump(uint8_t *buffer, int len)
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

// parse a comma-delimited string containing numbers (dec,hex) into a byte arr
static int  hexread(uint8_t *buffer, char *string, int buflen)
{
    char    *s;
    int     pos = 0;
    memset(buffer,0,buflen);  // bzero() not defined on Win32?
    while((s = strtok(string, ", ")) != NULL && pos < buflen){
        string = NULL;
        buffer[pos++] = (char)strtol(s, NULL, 0);
    }
    return pos;
}

// integer-only hsbtorgb
// from: http://web.mit.edu/storborg/Public/hsvtorgb.c
static void hsbtorgb( uint8_t* hsb, uint8_t* rgb)
{
    uint8_t h = hsb[0];
    uint8_t s = hsb[1];
    uint8_t v = hsb[2];

    unsigned char region, fpart, p, q, t;
    uint8_t r,g,b;

    if(s == 0) {          // color is grayscale 
        r = g = b = v;
        return;
    }
    
    region = h / 43;      // make hue 0-5 
    fpart = (h - (region * 43)) * 6; // find remainder part, make it from 0-255 
    
    // calculate temp vars, doing integer multiplication 
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;
        
    // assign temp vars based on color cone region 
    switch(region) {
        case 0:   r = v; g = t; b = p; break;
        case 1:   r = q; g = v; b = p; break;
        case 2:   r = p; g = v; b = t; break;
        case 3:   r = p; g = q; b = v; break;
        case 4:   r = t; g = p; b = v; break;
        default:  r = v; g = p; b = q; break;
    }    
    rgb[0]=r;
    rgb[1]=g;
    rgb[2]=b;
}



// --------------------------------------------------------------------------- 

//
static void usage(char *myName)
{
    fprintf(stderr,
"Usage: \n"
"  %s <cmd> [options]\n"
"where <cmd> is one of:\n"
"  --blink <numtimes>          Blink on/off (use --rgb to blink a color)\n"
"  --random, --random=<num>    Flash a number of random colors \n"
"  --glimmer, --glimmer=<num>  Glimmer a color with --rgb (num times)\n"
"  --running, --running=<num>  Do running animation (num times)\n"
"  --rgb=<red>,<green>,<blue>  Fade to RGB value\n"
"  --hsb=<hue>,<sat>,<bri>     Fade to HSB value\n"
"  --rgbread                   Read last RGB color sent (post gamma-correction)\n"
"  --on | --white              Turn blink(1) full-on white \n"
"  --off                       Turn blink(1) off \n"
"  --red                       Turn blink(1) red \n"
"  --green                     Turn blink(1) green \n"
"  --blue                      Turn blink(1) blue \n"
"  --cyan                      Turn blink(1) cyan (green + blue) \n"
"  --magenta                   Turn blink(1) magenta (red + blue) \n"
"  --yellow                    Turn blink(1) yellow (red + green) \n"
"  --setpattline <pos>         Write pattern RGB val at pos (--rgb/hsb to set)\n"
"  --getpattline <pos>         Read pattern RGB value at pos\n" 
"  --savepattern               Save color pattern to flash (mk2)\n"
"  --play <1/0,pos>            Start playing color sequence (at pos)\n"
"  --play <1/0,start,end,cnt>  Playing color sequence sub-loop (mk2)\n"
"  --servertickle <1/0>[,1/0]  Turn on/off servertickle (w/on/off, uses -t msec)\n"
"  --running                   Multi-LED effect (uses --led & --rgb)\n"
"  --list                      List connected blink(1) devices \n"
" Nerd functions: (not used normally) \n"
"  --eeread <addr>             Read an EEPROM byte from blink(1)\n"
"  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) \n"
"  --fwversion                 Display blink(1) firmware version \n"
"  --version                   Display blink1-tool version info \n"
"and [options] are: \n"
"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
"  -g -nogamma                 Disable autogamma correction\n"
"  -m ms,   --millis=millis    Set millisecs for color fading (default 300)\n"
"  -q, --quiet                 Mutes all stdout output (supercedes --verbose)\n"
"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
"  -l <led>, --led=<led>       Set which RGB LED in a blink(1) mk2 to use\n"
"  -v, --verbose               verbose debugging msgs\n"
"\n"
"Examples \n"
"  blink1-tool -m 100 --rgb=255,0,255    # fade to #FF00FF in 0.1 seconds \n"
"  blink1-tool -t 2000 --random=100      # every 2 seconds new random color\n"
"  blink1-tool --rgb=0xff,0,00 --blink 3 # blink red 3 times\n"
"\n"
            ,myName);
//"  --hidread                  Read a blink(1) USB HID GetFeature report \n"
//"  --hidwrite <listofbytes>   Write a blink(1) USB HID SetFeature report \n"
}

// local states for the "cmd" option variable
enum { 
    CMD_NONE = 0,
    CMD_LIST,
    CMD_EEREAD,
    CMD_EEWRITE,
    CMD_RGB,
    CMD_HSB,
    CMD_RGBREAD,
    CMD_SETPATTLINE,
    CMD_GETPATTLINE,
    CMD_SAVEPATTERN,
    CMD_OFF,
    CMD_ON,
    CMD_RED,
    CMD_GRN,
    CMD_BLU,
    CMD_CYAN,
    CMD_MAGENTA,
    CMD_YELLOW,
    CMD_BLINK,
    CMD_GLIMMER,
    CMD_PLAY,
    CMD_STOP,
    CMD_GETPLAYSTATE,
    CMD_RANDOM,
    CMD_RUNNING,
    CMD_VERSION,
    CMD_FWVERSION,
    CMD_SERVERDOWN,
    CMD_TESTTEST,
};

//
int main(int argc, char** argv)
{
    int openall = 0;
    int nogamma = 0;
    int16_t arg=0;

    int  rc;
    uint8_t tmpbuf[100];
    char serialnumstr[serialstrmax] = {'\0'}; 

    uint16_t seed = time(NULL);
    srand(seed);
    memset( cmdbuf, 0, sizeof(cmdbuf));

    static int cmd  = CMD_NONE;


    // parse options
    int option_index = 0, opt;
    char* opt_str = "aqvhm:t:d:gl:";
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
        {"eeread",     required_argument, &cmd,   CMD_EEREAD },
        {"eewrite",    required_argument, &cmd,   CMD_EEWRITE },
        {"rgb",        required_argument, &cmd,   CMD_RGB },
        {"hsb",        required_argument, &cmd,   CMD_HSB },
        {"rgbread",    no_argument,       &cmd,   CMD_RGBREAD},
        {"savepattline",required_argument,&cmd,   CMD_SETPATTLINE },//backcompat
        {"setpattline",required_argument, &cmd,   CMD_SETPATTLINE },
        {"getpattline",required_argument, &cmd,   CMD_GETPATTLINE },
        {"savepattern",no_argument,       &cmd,   CMD_SAVEPATTERN },
        {"off",        no_argument,       &cmd,   CMD_OFF },
        {"on",         no_argument,       &cmd,   CMD_ON },
        {"white",      no_argument,       &cmd,   CMD_ON },
        {"red",        no_argument,       &cmd,   CMD_RED },
        {"green",      no_argument,       &cmd,   CMD_GRN },
        {"blue",       no_argument,       &cmd,   CMD_BLU},
        {"cyan",       no_argument,       &cmd,   CMD_CYAN},
        {"magenta",    no_argument,       &cmd,   CMD_MAGENTA},
        {"yellow",     no_argument,       &cmd,   CMD_YELLOW},
        {"blink",      required_argument, &cmd,   CMD_BLINK},
        {"glimmer",    optional_argument, &cmd,   CMD_GLIMMER},
        {"play",       required_argument, &cmd,   CMD_PLAY},
        {"stop",       no_argument,       &cmd,   CMD_STOP},
        {"playstate",  no_argument,       &cmd,   CMD_GETPLAYSTATE},
        {"random",     optional_argument, &cmd,   CMD_RANDOM },
        {"running",    optional_argument, &cmd,   CMD_RUNNING },
        {"version",    no_argument,       &cmd,   CMD_VERSION },
        {"fwversion",  no_argument,       &cmd,   CMD_FWVERSION },
        //{"serialnumread", no_argument,    &cmd,   CMD_SERIALNUMREAD },
        //{"serialnumwrite",required_argument, &cmd,CMD_SERIALNUMWRITE },
        {"servertickle",  required_argument, &cmd,CMD_SERVERDOWN },
        {"testtest",   no_argument,       &cmd,   CMD_TESTTEST },
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
            case CMD_HSB:
                hexread(tmpbuf, optarg, 4);
                hsbtorgb( tmpbuf, rgbbuf );
                cmd = CMD_RGB; // haha! 
                break;
            case CMD_EEREAD:
            case CMD_EEWRITE:
            case CMD_SETPATTLINE:
            case CMD_GETPATTLINE:
            case CMD_BLINK:
            case CMD_PLAY:
            case CMD_SERVERDOWN:
                hexread(cmdbuf, optarg, sizeof(cmdbuf));  // cmd w/ hexlist arg
                break;
            case CMD_RANDOM:
            case CMD_RUNNING:
            case CMD_GLIMMER:
                arg = (optarg) ? strtol(optarg,NULL,0) : 0;// cmd w/ number arg
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
            case CMD_CYAN:
                rgbbuf[1] = 255; rgbbuf[2] = 255; 
                break;
            case CMD_MAGENTA:
                rgbbuf[0] = 255; rgbbuf[2] = 255; 
                break;
            case CMD_YELLOW:
                rgbbuf[0] = 255; rgbbuf[1] = 255; 
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
            if( verbose > 3 ) {
                fprintf(stderr,"going REALLY verbose\n");
            }
            break;
        case 'd':
            if( strcmp(optarg,"all") == 0 ) {
                numDevicesToUse = 0; //blink1_max_devices;
                for( int i=0; i< blink1_max_devices; i++) {
                    deviceIds[i] = i;
                }
            } 
            else { // if( strcmp(optarg,",") != -1 ) { // comma-separated list
                char* pch;
                int base = 0;
                pch = strtok( optarg, " ,");
                numDevicesToUse = 0;
                while( pch != NULL ) { 
                    int base = (strlen(pch)==8) ? 16:0;
                    deviceIds[numDevicesToUse++] = strtol(pch,NULL,base);
                    pch = strtok(NULL, " ,");
                }
                for( int i=0; i<numDevicesToUse; i++ ) { 
                    printf("deviceId[%d]: %d\n", i, deviceIds[i]);
                }
            }
            /*
            else if( strlen(optarg) == 8 ) { //
                deviceIds[0] = strtol( optarg, NULL, 16);
                numDevicesToUse = 1;
                //sprintf( serialnumstr, "%s", optarg);  // strcpy
            } 
            else {
                numDevicesToUse = hexread((uint8_t*)deviceIds,optarg,sizeof(deviceIds));
            }
            */
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
    int count = blink1_enumerate();

    if( cmd == CMD_VERSION ) { 
        char verbuf[40] = "";
        if( count ) { 
            dev = blink1_openById( deviceIds[0] );
            rc = blink1_getVersion(dev);
            blink1_close(dev);
            sprintf(verbuf, ", fw version: %d", rc);
        }
        msg("blink1-tool version: %s %s\n",BLINK1_VERSION,verbuf);
        exit(0);
    }


    if( count == 0 ) {
        msg("no blink(1) devices found\n");
        exit(1);
    }


    if( numDevicesToUse == 0 ) numDevicesToUse = count; 

    if( verbose ) { 
        printf("deviceId[0] = %X\n", deviceIds[0]);
        printf("cached list:\n");
        for( int i=0; i< count; i++ ) { 
            printf("%d: serial: '%s' '%s'\n", 
                   i, blink1_getCachedSerial(i), blink1_getCachedPath(i) );
        }
    }

    // actually open up the device to start talking to it
    if(verbose) printf("openById: %X\n", deviceIds[0]);
    dev = blink1_openById( deviceIds[0] );

    if( dev == NULL ) { 
        msg("cannot open blink(1), bad id or serial number\n");
        exit(1);
    }

    // FIXME: verify mk2 does better gamma correction 
    // (now thinking maybe it doesn't, or is not perfectly visually linear)
#if 0
    if( blink1_isMk2(dev) )  { 
        if( verbose ) printf("blink1(1)mk2 detected. disabling degamma\n");
        blink1_disableDegamma();  
        if( nogamma ) {
            blink1_enableDegamma();  
            if( verbose ) printf("overriding, re-enabling gamma\n");
        }
    }
    else {
        // for original mk1 owners who want to disable degamma
        if( nogamma ) {      //FIXME: confusing
            msg("disabling auto degamma\n");
            blink1_disableDegamma();  
        }
    }
#else
    if( nogamma ) {      //FIXME: confusing
        msg("disabling auto degamma\n");
        blink1_disableDegamma();  
    }
#endif

    // begin command processing

    if( cmd == CMD_LIST ) { 
        printf("blink(1) list: \n");
        for( int i=0; i< count; i++ ) {
            printf("id:%d - serialnum:%s %s\n", i, blink1_getCachedSerial(i), 
                   (blink1_isMk2ById(i)) ? "(mk2)":"");
        }
#ifdef USE_HIDDATA
        printf("(Listing not supported in HIDDATA builds)\n"); 
#endif
    }
    else if( cmd == CMD_EEREAD ) {  // FIXME
        msg("eeread:  addr 0x%2.2x = ", cmdbuf[0]);
        uint8_t val = 0;
        rc = blink1_eeread(dev, cmdbuf[0], &val );
        if( rc==-1 ) { // on error
            printf("error\n");
        } else { 
            printf("%2.2x\n", val);
        }
    }
    else if( cmd == CMD_EEWRITE ) {  // FIXME
        msg("eewrite: \n");
        rc = blink1_eewrite(dev, cmdbuf[0], cmdbuf[1] );
        if( rc==-1  && !quiet ) { // error
            printf("error\n");
        }
    }
    else if( cmd == CMD_FWVERSION ) { 
        rc = blink1_getVersion(dev);
        msg("blink1-tool: firmware version: ");
        printf("%d\n",rc);
    }
    else if( cmd == CMD_RGB || cmd == CMD_ON  || cmd == CMD_OFF ||
             cmd == CMD_RED || cmd == CMD_BLU || cmd == CMD_GRN ||
	     cmd == CMD_CYAN || cmd == CMD_MAGENTA || cmd == CMD_YELLOW ) { 
        blink1_close(dev); // close global device, open as needed
        
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];

        for( int i=0; i< numDevicesToUse; i++ ) {
            dev = blink1_openById( deviceIds[i] );
            if( dev == NULL ) continue;
            msg("set dev:%X to rgb:0x%2.2x,0x%2.2x,0x%2.2x over %d msec\n",
                deviceIds[i],r,g,b,millis);
            if( ledn==0 ) {
                rc = blink1_fadeToRGB(dev,millis, r,g,b);
            } else {
                rc = blink1_fadeToRGBN(dev,millis,r,g,b, ledn);
            }
            if( rc == -1 && !quiet ) { // on error, do something, anything. 
                printf("error on fadeToRGB\n");
            }
            blink1_close( dev );
        }
    }
    else if( cmd == CMD_RGBREAD ) { 
        uint8_t r,g,b;
        uint16_t msecs;
        msg("reading led %d rgb: ", ledn );
        rc = blink1_readRGB(dev, &msecs, &r,&g,&b, ledn );
        if( rc==-1 && !quiet ) {
            printf("error on readRGB\n");
        }
        printf("0x%2.2x,0x%2.2x,0x%2.2x\n", r,g,b);
    }
    else if( cmd == CMD_PLAY || cmd == CMD_STOP ) { 
        //if( cmd == CMD_STOP ) play = 0
        uint8_t play     = cmdbuf[0];
        uint8_t startpos = cmdbuf[1];
        uint8_t endpos   = cmdbuf[2]; 
        uint8_t count    = cmdbuf[3];

        msg("%s color pattern from pos %d-%d (%d times)\n", 
                   ((play)?"playing":"stopping"),startpos,endpos,count);

        rc = blink1_playloop(dev, play, startpos,endpos,count);
        if( rc == -1 && !quiet ) { 
            // hmm, do what here
        }
    }
    else if( cmd == CMD_GETPLAYSTATE ) { 
        msg("playstate: ");
        uint8_t playing;
        uint8_t startpos;
        uint8_t endpos;
        uint8_t playcount;
        uint8_t playpos;
        rc = blink1_readPlayState(dev, &playing, &startpos, &endpos, 
                                  &playcount, &playpos);
        printf("playing:%d start-end:%d-%d count:%d pos:%d\n",
               playing, startpos, endpos, playcount, playpos);
    }
    else if( cmd == CMD_SAVEPATTERN ) {
        msg("writing pattern to flash\n");
        rc = blink1_savePattern(dev);
        if( rc==-1 && !quiet ) {
            printf("error on savePattern\n");
        }
    }
    else if( cmd == CMD_SETPATTLINE ) {
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
        uint8_t p = cmdbuf[0];
        msg("saving rgb: 0x%2.2x,0x%2.2x,0x%2.2x @ %d, ms:%d\n",r,g,b,p,millis);
        if( ledn>0 ) { // NOTE: only works for unreleased mk2a
            blink1_setLEDN(dev, ledn);  // FIXME: doesn't check return code
        }
        rc = blink1_writePatternLine(dev, millis, r,g,b, p );
        if( rc==-1 && !quiet ) {
            printf("error on writePatternLine\n");
        }
    }
    else if( cmd == CMD_GETPATTLINE ) { 
        uint8_t p = cmdbuf[0];
        uint8_t r,g,b,n;
        uint16_t msecs;
        msg("reading rgb at pos %2d: ", p );
        rc = blink1_readPatternLineN(dev, &msecs, &r,&g,&b, &n, p );
        if( rc==-1 && !quiet ) {
            printf("error on writePatternLine\n");
        }
        printf("r,g,b = 0x%2.2x,0x%2.2x,0x%2.2x (%d) ms:%d\n", r,g,b, n, msecs);
    }
    else if( cmd == CMD_RANDOM ) { 
        int cnt = blink1_getCachedCount();
        if( arg==0 ) arg = 1;
        if( cnt>1 ) blink1_close(dev); // close global device, open as needed
        msg("random %d times: \n", arg);
        for( int i=0; i<arg; i++ ) { 
            uint8_t r = rand()%255;
            uint8_t g = rand()%255;
            uint8_t b = rand()%255 ;
            uint8_t id = rand() % blink1_getCachedCount();

            msg("%d: %d/%d : %2.2x,%2.2x,%2.2x \n", 
                i, id, blink1_getCachedCount(), r,g,b);

            blink1_device* mydev = dev;
            if( cnt > 1 ) mydev = blink1_openById( id );
            if( ledn == 0 ) { 
                rc = blink1_fadeToRGB(mydev, millis,r,g,b);
            } else {
                uint8_t n = 1 + rand() % ledn;
                rc = blink1_fadeToRGBN(mydev, millis,r,g,b,n);
            }
            if( rc == -1 && !quiet ) { // on error, do something, anything.
                printf("error during random\n");
                //break;
            }
            if( cnt > 1 ) blink1_close( mydev );
            
            blink1_sleep(delayMillis);
        }
    }
    // this whole thing is a huge mess currently // FIXME
    else if( cmd == CMD_RUNNING ) { 
        if( ledn == 0 ) ledn = 18;

        uint8_t do_rand = 0;
        uint8_t running_cnt = 0;
        uint8_t leds[ledn][3];
        uint8_t c[3] = { rgbbuf[0], rgbbuf[1], rgbbuf[2] };
        if( c[0] == 0 && c[1] == 0 && c[2] == 0 ) { // no rgb specified
            c[0] = rand()%255; c[1] = rand()%255; c[2] = rand()%255; 
            do_rand =1;
        }
        char ledstr[16];
        sprintf(ledstr, "#%2.2x%2.2x%2.2x", rgbbuf[0],rgbbuf[1],rgbbuf[2]);
        printf("running effect with %d LEDs, color %s\n",
               ledn, ((do_rand) ? "random" : ledstr) );
        
        // make gradient
        for( int i=0; i<ledn; i++ ) {
            leds[i][0] = c[0] * i / ledn;
            leds[i][1] = c[1] * i / ledn;
            leds[i][2] = c[2] * i / ledn;
        }
        
        int loopcnt = (arg*ledn);
        do { 
            if(arg==0) loopcnt=1;
            // output
            for( int n=0; n<ledn; n++ )  {
                uint8_t r = leds[n][0];
                uint8_t g = leds[n][1];
                uint8_t b = leds[n][2];
                rc = blink1_fadeToRGBN(dev, 10 + (millis/ledn), r,g,b,n+1 );
            }
            blink1_sleep(delayMillis/ledn);
            
            // rotate
            memcpy( c,      leds[ledn-1], sizeof(c) );
            memcpy( leds+1, leds,         sizeof(c)*(ledn-1) );
            memcpy( leds[0],   c,         sizeof(c) );

            // change color if random
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
        } while( loopcnt-- );
        // turn everyone off
        blink1_sleep(delayMillis/ledn/2);
        rc = blink1_fadeToRGB(dev, 10 + (millis/ledn), 0,0,0 );

    }
    else if( cmd == CMD_BLINK ) { 
        uint8_t n = cmdbuf[0]; 
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
        if( r == 0 && b == 0 && g == 0 ) {
            r = g = b = 255;
        }
        msg("blink %d times rgb:%x,%x,%x: \n", n,r,g,b);
        for( int i=0; i<n; i++ ) { 
            rc = blink1_fadeToRGBN(dev, millis,r,g,b,ledn);
            blink1_sleep(delayMillis);
            rc = blink1_fadeToRGBN(dev, millis,0,0,0,ledn);
            blink1_sleep(delayMillis);
        }
    }
    else if( cmd == CMD_GLIMMER ) {
        uint8_t n = arg;
        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
        if( r == 0 && b == 0 && g == 0 ) {
            r = g = b = 127;
            if( n == 0 ) n = 3;
        }
        msg("glimmering %d times rgb:#%2.2x%2.2x%2.2x: \n", n,r,g,b);
        for( int i=0; i<n; i++ ) {
            blink1_fadeToRGBN(dev, millis,r,g,b, 1);
            blink1_fadeToRGBN(dev, millis,r/2,g/2,b/2, 2);
            blink1_sleep(delayMillis/2);
            blink1_fadeToRGBN(dev, millis,r/2,g/2,b/2, 1);
            blink1_fadeToRGBN(dev, millis,r,g,b, 2);
            blink1_sleep(delayMillis/2);
        }
        // turn them both off
        blink1_fadeToRGBN(dev, millis, 0,0,0, 1);
        blink1_fadeToRGBN(dev, millis, 0,0,0, 2);
    }
    else if( cmd == CMD_SERVERDOWN ) { 
        //int on  = arg;
        int on = cmdbuf[0];
        int st = cmdbuf[1];
        msg("setting servertickle %s (@ %d millis)\n",((on)?"ON":"OFF"),delayMillis);
        blink1_serverdown( dev, on, delayMillis, st );
    }
    else if( cmd == CMD_TESTTEST ) { 
        msg("test test\n");
        rc = blink1_testtest(dev);
    }


    return 0;
}


