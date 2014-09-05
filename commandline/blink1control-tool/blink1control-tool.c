/**
 *
 *
 * blink1control-tool --rgb 255,0,0
 * blink1control-tool --list
 * blink1control-tool --version
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>    // vararg stuff
#include <getopt.h>    // for getopt_long()
#include <time.h>

#include <curl/curl.h>


char baseUrl[100] = "http://127.0.0.1:8934/"; // kinda hacky in many ways

char progname[] = "blink1control-tool";

// start from blink1-lib.h
#define blink1_max_devices 16

#define cache_max 16  
#define serialstrmax (8 + 1) 
#define pathstrmax 128

#define blink1mk2_serialstart 0x20000000

#define blink1_report_id  1
#define blink1_report_size 8
#define blink1_buf_size (blink1_report_size+1)
// end from blink1-lib.h

int millis = 300;
int delayMillis = 500;
int numDevicesToUse = 0;
int ledn = 0;

char  deviceIds[blink1_max_devices][10];

uint8_t cmdbuf[blink1_buf_size]; 
uint8_t rgbbuf[3];
int verbose;
int quiet=0;

struct curlMemoryStruct {
  char *memory;
  size_t size;
};


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

//
static size_t 
curlWriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct curlMemoryStruct *mem = (struct curlMemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {     /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

//
int curlDoHttpTransaction(const char* urlstr)
{
  CURL *curl_handle;
  CURLcode res;

  struct curlMemoryStruct chunk;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  curl_handle = curl_easy_init();  /* init the curl session */
  curl_easy_setopt(curl_handle, CURLOPT_URL, urlstr);  /* specify URL to get */
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curlWriteMemoryCallback);
  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);    /* get it! */
  if(res != CURLE_OK) {   /* check for errors */
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * FIXME: Do something nice with it!
     */
    msg("%lu bytes retrieved\n", (long)chunk.size);
    for( int i=0; i<chunk.size; i++ ){ 
        msg("%c",chunk.memory[i]); 
    }
  }

  curl_easy_cleanup(curl_handle);   /* cleanup curl stuff */
  if(chunk.memory)
      free(chunk.memory);
  return 0;
}

//
int blink1control_fadeToRGBN( int r, int g, int b, int tmillis, char* idstr, int ledn)
{
    char urlbuf[200];
    char idarg[100] = "";
    if( idstr != NULL ) { 
        sprintf(idarg, "id=%s",idstr);
    }
    sprintf(urlbuf,
            "%s/blink1/fadeToRGB?rgb=%%23%2.2x%2.2x%2.2x&time=%2.2f&ledn=%d&%s",
            baseUrl, r,g,b, (tmillis/1000.0), ledn, idarg);
    printf("urlbuf: %s\n",urlbuf);
    return curlDoHttpTransaction( urlbuf );
}

// simple cross-platform millis sleep func
void blink1_sleep(uint16_t millis)
{
#ifdef WIN32
            Sleep(millis);
#else 
            usleep( millis * 1000);
#endif
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

//
static void usage(char *myName)
{
    fprintf(stderr,
"Usage: \n"
"  %s <cmd> [options]\n"
"where <cmd> is one of:\n"
"  --blink <numtimes>          Blink on/off (use --rgb to blink a color)\n"
"  --random, --random=<num>    Flash a number of random colors \n"
"  --glimmer, --gimmer=<num>   Glimmer a color with --rgb (num times)\n"
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
"  --fwversion                 Display blink(1) firmware version \n"
"  --version                   Display blink1control-tool version info \n"
"and [options] are: \n"
"  -U <url> --baseUrl <url>    Use url instead of 'http://localhost:8934/'\n"
"  -d dNums --id all|deviceIds Use these blink(1) ids (from --list) \n"
"  -g -nogamma                 Disable autogamma correction\n"
"  -m ms,   --millis=millis    Set millisecs for color fading (default 300)\n"
"  -q, --quiet                 Mutes all stdout output (supercedes --verbose)\n"
"  -t ms,   --delay=millis     Set millisecs between events (default 500)\n"
"  -l <led>, --led=<led>       Set which RGB LED in a blink(1) mk2 to use\n"
"  -v, --verbose               verbose debugging msgs\n"
"\n"
"Examples \n"
"  blink1control-tool -m 100 --rgb=255,0,255    # fade to #FF00FF in 0.1 seconds \n"
"  blink1control-tool -t 2000 --random=100      # every 2 seconds new random color\n"
"  blink1control-tool --rgb=0xff,0,00 --blink 3 # blink red 3 times\n"
"\n"
            ,myName);
}

// local states for the "cmd" option variable
enum { 
    CMD_NONE = 0,
    CMD_LIST,
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
    char* opt_str = "aqvhm:t:d:U:gl:";
    static struct option loptions[] = {
        {"baseurl",    required_argument, 0,      'U'},
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
                numDevicesToUse = 0; //blink1_max_devices;  //FIXME
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
                // verbose
                for( int i=0; i<numDevicesToUse; i++ ) { 
                    printf("deviceId[%d]: %d\n", i, deviceIds[i]);
                }
            }
            break;
        case 'u':
            strncpy(baseUrl, optarg, 100);
            break;
        case 'h':
            usage( progname );
            exit(1);
            break;
        }
    } // while(1)

    if(argc < 2){
        usage( progname );
        //exit(1);
    }

    char idstr[100];
        
    for( int i=0; i< numDevicesToUse; i++ ) {
        sprintf(idstr, "%d,", deviceIds[i]);
    }
    printf("idstr: %s\n",idstr);

    curl_global_init(CURL_GLOBAL_ALL);

    if( cmd == CMD_VERSION ) { 
        // FIXME: do something here
    } 
    else if( cmd == CMD_RGB || cmd == CMD_ON  || cmd == CMD_OFF ||
             cmd == CMD_RED || cmd == CMD_BLU || cmd == CMD_GRN ||
             cmd == CMD_CYAN || cmd == CMD_MAGENTA || cmd == CMD_YELLOW ) { 

        uint8_t r = rgbbuf[0];
        uint8_t g = rgbbuf[1];
        uint8_t b = rgbbuf[2];
               
        blink1control_fadeToRGBN( r,g,b, millis, idstr, ledn );
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
            blink1control_fadeToRGBN( r,g,b, millis, idstr, ledn );
            blink1_sleep(delayMillis);
            blink1control_fadeToRGBN( 0,0,0, millis, idstr, ledn );
            blink1_sleep(delayMillis);
        }
    }

    curl_global_cleanup();   // we're done with libcurl, so clean it up 

}
