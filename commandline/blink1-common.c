#include <stdint.h>
#include <ctype.h>  // for toupper()

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>    // for usleep()
#endif

typedef void *blink1_dev; /* sufficiently general type */
#include "blink1-common.h"

#define blink1_report_id 1

// addresses in EEPROM 
#define blink1_eeaddr_osccal        0
#define blink1_eeaddr_bootmode      1
#define blink1_eeaddr_serialnum     2
#define blink1_serialnum_len        4
#define blink1_eeaddr_patternstart (blink1_eeaddr_serialnum + blink1_serialnum_len)

static int blink1_enable_degamma = 1;

//
int blink1_getVersion(blink1_dev dev)
{
    char buf[9] = {blink1_report_id, 'v' };
    int len = sizeof(buf);

    //hid_set_nonblocking(dev, 0);
    int rc = blink1_write(dev, buf, sizeof(buf));
    blink1_sleep( 50 ); //FIXME:
    if( rc != -1 ) // no error
        rc = blink1_read(dev, buf, len);
    if( rc != -1 ) // also no error
        rc = ((buf[3]-'0') * 100) + (buf[4]-'0'); 
    // rc is now version number or error  
    // FIXME: we don't know vals of errcodes
    return rc;
}

//
int blink1_eeread(blink1_dev dev, uint16_t addr, uint8_t* val)
{
    char buf[9] = {blink1_report_id, 'e', addr };
    int len = sizeof(buf);

    int rc = blink1_write(dev, buf, len );
    blink1_sleep( 50 ); // FIXME:
    if( rc != -1 ) // no error
        rc = blink1_read(dev, buf, len );
    if( rc != -1 ) 
        *val = buf[3];
    return rc;
}

//
int blink1_eewrite(blink1_dev dev, uint16_t addr, uint8_t val)
{
    char buf[9] = {blink1_report_id, 'E', addr, val };

    int rc = blink1_write(dev, buf, sizeof(buf) );
        
    return rc;
}

// FIXME: this doesn't work
int blink1_serialnumread(blink1_dev dev, uint8_t *serialnum)
{
    int rc = 0;
    int i;
    for( i=0; i<blink1_serialnum_len; i++ ) { // serial num is 8 chars long
        //blink1_eeread( dev, blink1_eeaddr_serialnum+i, (serialnum+i) );
    }
    return rc;
}

//
static uint8_t parseHex(char c) 
{
    c = toupper(c);
    if (c >= '0' && c <= '9')  return (c - '0');
    if (c >= 'A' && c <= 'F')  return (c - 'A')+10;
    return 0;
}

// serialnum comes in as an ascii set of 8 characters representing
// 4-bytes 
int blink1_serialnumwrite(blink1_dev dev, const uint8_t serialnumstr[8])
{
    uint8_t serialnum[4];
    serialnum[0] = parseHex( serialnumstr[0] )*16 + parseHex( serialnumstr[1] );
    serialnum[1] = parseHex( serialnumstr[2] )*16 + parseHex( serialnumstr[3] );
    serialnum[2] = parseHex( serialnumstr[4] )*16 + parseHex( serialnumstr[5] );
    serialnum[3] = parseHex( serialnumstr[6] )*16 + parseHex( serialnumstr[7] );

    int rc = 0;
    int i;
    for( i=0; i<blink1_serialnum_len; i++ ) { // serialnum is 4 chars long
        blink1_sleep(50); //FIXME: 
        uint8_t v = serialnum[i];
        int rc = blink1_eewrite( dev, blink1_eeaddr_serialnum+i, v);
        if( rc == -1 ) { // try again
            BLINK1_LOG("blink1_serialwrite: oops, trying again on char %d\n",i);
            rc = blink1_eewrite(dev,blink1_eeaddr_serialnum+i, v);
            if( rc == -1 ) { 
                BLINK1_LOG("blink1_serialwrite: error on try again\n");
                break;
            }
        }

    }
    return rc;
}

//
int blink1_fadeToRGB(blink1_dev dev,  uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b)
{
    int dms = fadeMillis/10;  // millis_divided_by_10

    char buf[9];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'c';   // command code for 'fade to rgb'
    buf[2] = ((blink1_enable_degamma) ? blink1_degamma(r) : r );
    buf[3] = ((blink1_enable_degamma) ? blink1_degamma(g) : g );
    buf[4] = ((blink1_enable_degamma) ? blink1_degamma(b) : b );
    buf[5] = (dms >> 8);
    buf[6] = dms & 0xff;

    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}

//
int blink1_setRGB(blink1_dev dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buf[9];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'n';   // command code for "set rgb now"
    buf[2] = ((blink1_enable_degamma) ? blink1_degamma(r) : r );     // red
    buf[3] = ((blink1_enable_degamma) ? blink1_degamma(g) : g );     // grn
    buf[4] = ((blink1_enable_degamma) ? blink1_degamma(b) : b );     // blu
    
    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}

int blink1_serverdown(blink1_dev dev, uint8_t on, uint16_t millis)
{
    int dms = millis/10;  // millis_divided_by_10

    char buf[9] = {blink1_report_id, 'D', on, (dms>>8), (dms & 0xff) };

    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_play(blink1_dev dev, uint8_t play, uint8_t pos)
{
    char buf[9] = {blink1_report_id, 'p', play, pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}
    
//
int blink1_writePatternLine(blink1_dev dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos)
{
    int dms = fadeMillis/10;  // millis_divided_by_10
    r = (blink1_enable_degamma) ? blink1_degamma(r) : r ;
    g = (blink1_enable_degamma) ? blink1_degamma(g) : g ;
    b = (blink1_enable_degamma) ? blink1_degamma(b) : b ;
    char buf[9] = {blink1_report_id, 'P', r,g,b, (dms>>8), (dms & 0xff), pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_readPatternLine(blink1_dev dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos)
{
    char buf[9] = {blink1_report_id, 'R', 0,0,0, 0,0, pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    blink1_sleep( 50 ); // FIXME:
    if( rc != -1 ) // no error
        rc = blink1_read(dev, buf, sizeof(buf) );
    if( rc != -1 ) {
        *r = buf[2];
        *g = buf[3];
        *b = buf[4];
        *fadeMillis = ((buf[5]<<8) + (buf[6] &0xff)) * 10;
    }
    return rc;
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

/* ------------------------------------------------------------------------- */

// FIXME: this is wrong
// FIXME: provide function that generated this
uint8_t degamma_lookup[256] = { 
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,
  4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,9,
  9,9,10,10,11,11,11,12,12,13,13,14,14,15,15,16,
  16,17,17,18,18,19,19,20,20,21,22,22,23,23,24,25,
  25,26,27,27,28,29,29,30,31,31,32,33,33,34,35,36,
  36,37,38,39,40,40,41,42,43,44,44,45,46,47,48,49,
  50,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
  65,66,67,68,69,70,71,72,73,74,75,76,77,79,80,81,
  82,83,84,85,87,88,89,90,91,93,94,95,96,97,99,100,
  101,102,104,105,106,108,109,110,112,113,114,116,117,118,120,121,
  122,124,125,127,128,129,131,132,134,135,137,138,140,141,143,144,
  146,147,149,150,152,153,155,156,158,160,161,163,164,166,168,169,
  171,172,174,176,177,179,181,182,184,186,188,189,191,193,195,196,
  198,200,202,203,205,207,209,211,212,214,216,218,220,222,224,225,
  227,229,231,233,235,237,239,241,243,245,247,249,251,253,255,255,
};

void blink1_enableDegamma()
{
    blink1_enable_degamma = 1;
}
void blink1_disableDegamma()
{
    blink1_enable_degamma = 0;
}

// a simple logarithmic -> linear mapping as a sort of gamma correction
// maps from 0-255 to 0-255
static int blink1_degamma_log2lin( int n )  
{
  //return  (int)(1.0* (n * 0.707 ));  // 1/sqrt(2)
  return (((1<<(n/32))-1) + ((1<<(n/32))*((n%32)+1)+15)/32);
}

//
int blink1_degamma( int n ) 
{ 
    //return degamma_lookup[n];
    return blink1_degamma_log2lin(n);
}


/*

//
int blink1_nightlight(hid_device *dev, uint8_t on)
{
    char buf[9] = { blink1_report_id, 'N', on };

    int rc = blink1_write(dev, buf, sizeof(buf) );
    
    return rc;
}
*/
