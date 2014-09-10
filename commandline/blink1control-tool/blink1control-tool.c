/**
 *
 *
 * blink1control-tool --rgb 255,0,0
 * blink1control-tool --list
 * blink1control-tool --version
 *
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

#include "json.h"  // https://github.com/udp/json-parser


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

char urlbuf[200];

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


json_value* json_convert_value( json_value* jv)
{
    switch(jv->type) { 
    case json_object:
        printf("json_object: \n");
        for( int i=0; i< jv->u.object.length; i++ ) {
            char* name = jv->u.object.values[i].name;
            json_value* jjv = jv->u.object.values[i].value;
            printf("- '%s'\n", name);
            json_convert_value( jjv );
        }
        break;
    case json_array:
        printf("json_array:\n");
        for( int i=0; i< jv->u.array.length; i++ ) { 
            return json_convert_value( jv->u.array.values[i] );
        }
        break;
    case json_string:
        printf("json_string: '%s'\n", jv->u.string.ptr);
        break;
    case json_integer:
        printf("json_integer:%lld\n", jv->u.integer );
        break;
    case json_double:
        printf("json_double:\n");
        break;
    case json_boolean:
        printf("json_boolean:\n");
        break;
    default:
        printf("default:\n");        
        break;
    }
    printf("at end\n");
    return NULL;
}

//
char* curl_fetch(const char* urlstr)
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
        fprintf(stderr, "curl_fetch() failed: %s\n",
                curl_easy_strerror(res));
    }
    else {
        // Now, our chunk.memory points to a memory block that is chunk.size
        // bytes big and contains the remote file.
        if( verbose > 1 ) { 
            msg("%lu bytes retrieved\n", (long)chunk.size);
            for( int i=0; i<chunk.size; i++ ){ 
                msg("%c",chunk.memory[i]); 
            }
        }
        
        //printf("chunk.memory:%s\n", chunk.memory);
        //json_value* jv = json_parse( chunk.memory, chunk.size );
        //json_convert_value(jv);
        //blink1control_getIds( jv );

    }
    
    curl_easy_cleanup(curl_handle);   /* cleanup curl stuff */
    //if(chunk.memory)
    //    free(chunk.memory);
    //return 0;
    return chunk.memory;
}

//
void blink1control_printIds()
{
    sprintf(urlbuf, "%s/blink1/id", baseUrl);
    if( verbose > 1 ) msg("url: %s\n",urlbuf);

    char* js = curl_fetch( urlbuf );
    json_value* jv = json_parse( js, strlen(js));

    if( jv->type != json_object ) {
        printf("bad response from Blink1Control\n");
        free(js);
        return;
    }
    for( int i=0; i< jv->u.object.length; i++ ) {
        char* name = jv->u.object.values[i].name;
        json_value* jjv = jv->u.object.values[i].value;
        
        if( strcmp(name,"blink1_serialnums")==0 ) { 
            if( jjv->u.array.length==0 ) {
                printf("no blink(1) devices found\n");
                free(js);
                return;
            }
            for( int j=0; j < jjv->u.array.length; j++ ) { 
                json_value* jjjv = jjv->u.array.values[j];
                //printf("id: %s\n", (int) jjjv->u.integer);
                char* serialnum = jjjv->u.string.ptr;
                printf("id:%d - serialnum:%s %s\n", j, serialnum,  "");
                       //(blink1_isMk2ById(i)) ? "(mk2)":"");
            }
        }
    }
    free(js);
}

//
int blink1control_fadeToRGBN( int tmillis, int r, int g, int b, char* idstr, int ledn)
{
    char idarg[100] = "";
    if( idstr != NULL ) { 
        sprintf(idarg, "id=%s",idstr);
    }
    sprintf(urlbuf,
            "%s/blink1/fadeToRGB?rgb=%%23%2.2x%2.2x%2.2x&time=%2.2f&ledn=%d&%s",
            baseUrl, r,g,b, (tmillis/1000.0), ledn, idarg);
    if( verbose > 1 ) msg("url: %s\n",urlbuf);

    char* js = curl_fetch( urlbuf );

    free(js);
    
    return 0;  // FIXME:
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
"  -U <url> --baseurl <url>    Use url instead of 'http://localhost:8934/'\n"
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

    //int  rc;
    uint8_t tmpbuf[100];
    //char serialnumstr[serialstrmax] = {'\0'}; 

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
                //int base = 0;
                pch = strtok( optarg, " ,");
                numDevicesToUse = 0;
                while( pch != NULL ) { 
                    //int base = (strlen(pch)==8) ? 16:0;
                    strcpy( deviceIds[numDevicesToUse++], pch );
                    pch = strtok(NULL, " ,");
                }
                // verbose
                for( int i=0; i<numDevicesToUse; i++ ) { 
                    printf("deviceId[%d]: %s\n", i, deviceIds[i]);
                }
            }
            break;
        case 'U':
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
        sprintf(idstr, "%s,", deviceIds[i]);
    }
    //printf("idstr: %s\n",idstr);

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
               
        blink1control_fadeToRGBN( millis, r,g,b, idstr, ledn );
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
            blink1control_fadeToRGBN( millis, r,g,b, idstr, ledn );
            blink1_sleep(delayMillis);
            blink1control_fadeToRGBN( millis, 0,0,0, idstr, ledn );
            blink1_sleep(delayMillis);
        }
    }
    else if( cmd == CMD_RANDOM ) { 
        //int cnt = blink1_getCachedCount();
        if( arg==0 ) arg = 1;
        msg("random %d times: \n", arg);
        for( int i=0; i<arg; i++ ) { 
            uint8_t r = rand()%255;
            uint8_t g = rand()%255;
            uint8_t b = rand()%255 ;
            //uint8_t id = rand() % blink1_getCachedCount();

            msg("%d: %d/%d : %2.2x,%2.2x,%2.2x \n", 
                i, 0,  0, r,g,b);
              //i, id, blink1_getCachedCount(), r,g,b);

            uint8_t n = (ledn!=0) ? (1 + rand() %ledn) : 0;
            blink1control_fadeToRGBN( millis, r,g,b, idstr, n );
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
            blink1control_fadeToRGBN( millis,r,g,b,       idstr, 1);
            blink1control_fadeToRGBN( millis,r/2,g/2,b/2, idstr, 2);
            blink1_sleep(delayMillis/2);
            blink1control_fadeToRGBN( millis,r/2,g/2,b/2, idstr, 1);
            blink1control_fadeToRGBN( millis,r,g,b,       idstr, 2);
            blink1_sleep(delayMillis/2);
        }
        // turn them both off
        blink1control_fadeToRGBN( millis, 0,0,0, idstr, 1);
        blink1control_fadeToRGBN( millis, 0,0,0, idstr, 2);
    }
    else if( cmd == CMD_LIST ) { 
        //int count = 0;
        printf("blink(1) list: \n");
        //for( int i=0; i< count; i++ ) {
            //printf("id:%d - serialnum:%s %s\n", i, blink1_getCachedSerial(i), 
            //       (blink1_isMk2ById(i)) ? "(mk2)":"");
        //}
        
        blink1control_printIds();
    }

    curl_global_cleanup();   // we're done with libcurl, so clean it up 

}



/*

        // parse output. look for:
        // - is valid json
        // - presence of 'status'
        // - presence of command name in status (doesn't work for on/off)

        //char* theJson = chunk.memory;
        //int resultCode;
        //jsmn_parser jsparser;
        //jsmntok_t jstokens[128]; // a number >= total number of tokens

        //jsmn_parser parser;
        //jsm_init(&parser);

        //jsmntok_t tokens[10];

        // js - pointer to JSON string
        // tokens - an array of tokens available
        // 10 - number of tokens available
        //jsmn_init_parser(&parser, js, tokens, 10);
        //jsmn_init(&parser);
        //resultCode = jsmn_parse(&parser, theJson, 100, jstokens, 256);

        char URL[] = "http://127.0.0.1:8934/blink1/id";
        char *KEYS[] = { "blink1_serialnums", "blink1_id", "status", "blink1control_version", "blink1control_config" };

        char *js = json_fetch(URL);
        jsmntok_t *tokens = json_tokenise(js);

       // The GitHub user API response is a single object. States required to
       // parse this are simple: start of the object, keys, values we want to
       // print, values we want to skip, and then a marker state for the end. 

    typedef enum { START, KEY, PRINT, SKIP, STOP } parse_state;
    parse_state state = START;

    size_t object_tokens = 0;

    for (size_t i = 0, j = 1; j > 0; i++, j--)
    {
        jsmntok_t *t = &tokens[i];

        // Should never reach uninitialized tokens
        log_assert(t->start != -1 && t->end != -1);

        if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
            j += t->size;

        switch (state)
        {
            case START:
                if (t->type != JSMN_OBJECT)
                    log_die("Invalid response: root element must be an object.");

                state = KEY;
                object_tokens = t->size;

                if (object_tokens == 0)
                    state = STOP;

                if (object_tokens % 2 != 0)
                    log_die("Invalid response: object must have even number of children.");

                break;

            case KEY:
                object_tokens--;

                if (t->type != JSMN_STRING)
                    log_die("Invalid response: object keys must be strings.");

                state = SKIP;

                for (size_t i = 0; i < sizeof(KEYS)/sizeof(char *); i++)
                {
                    if (json_token_streq(js, t, KEYS[i]))
                    {
                        printf("%s: ", KEYS[i]);
                        state = PRINT;
                        break;
                    }
                }

                break;

            case SKIP:
                if (t->type != JSMN_STRING && t->type != JSMN_PRIMITIVE)
                    log_die("Invalid response: object values must be strings or primitives.");

                object_tokens--;
                state = KEY;

                if (object_tokens == 0)
                    state = STOP;

                break;

            case PRINT:
                if (t->type != JSMN_STRING && t->type != JSMN_PRIMITIVE) {
                    //   log_die("Invalid response: object values must be strings or primitives.");
                }

                char *str = json_token_tostr(js, t);
                puts(str);

                object_tokens--;
                state = KEY;

                if (object_tokens == 0)
                    state = STOP;

                break;

            case STOP:
                // Just consume the tokens
                break;

            default:
                log_die("Invalid state %u", state);
        }
    }
*/
