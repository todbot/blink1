/*
 * blink(1) C library -- 
 *
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // for toupper()

#ifdef _WIN32
#include <windows.h>
#define   swprintf   _snwprintf
#else
#include <unistd.h>    // for usleep()
#endif

#include "blink1-lib.h"

//#define DEBUG_PRINTF

#ifdef DEBUG_PRINTF
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) do {} while (0)
#endif

#define blink1_report_id  1
#define blink1_report_size 8
#define blink1_buf_size (blink1_report_size+1)

// addresses in EEPROM for mk1 blink(1) devices
#define blink1_eeaddr_osccal        0
#define blink1_eeaddr_bootmode      1
#define blink1_eeaddr_serialnum     2
#define blink1_serialnum_len        4
#define blink1_eeaddr_patternstart (blink1_eeaddr_serialnum + blink1_serialnum_len)


static blink1_info blink1_infos[cache_max];
static int blink1_cached_count = 0;  // number of cached entities

static int blink1_enable_degamma = 1;

//----------------------------------------------------------------------------
void blink1_sortCache(void);

//
int blink1_enumerate(void)
{
    return blink1_enumerateByVidPid( blink1_vid(), blink1_pid() );
}

// get all matching devices by VID/PID pair
int blink1_enumerateByVidPid(int vid, int pid)
{
    struct hid_device_info *devs, *cur_dev;

    int p = 0; 
    devs = hid_enumerate(vid, pid);
    cur_dev = devs;    
    while (cur_dev) {
        if( (cur_dev->vendor_id != 0 && cur_dev->product_id != 0) &&  
            (cur_dev->vendor_id == vid && cur_dev->product_id == pid) ) { 
            if( cur_dev->serial_number != NULL ) { // can happen if not root
                strcpy( blink1_infos[p].path,   cur_dev->path );
                sprintf( blink1_infos[p].serial, "%ls", cur_dev->serial_number );
                //wcscpy( blink1_infos[p].serial, cur_dev->serial_number );
                //uint32_t sn = wcstol( cur_dev->serial_number, NULL, 16);
                uint32_t serialnum = strtol( blink1_infos[p].serial, NULL, 16);
                blink1_infos[p].type = BLINK1_MK1;
                if( serialnum >= blink1mk2_serialstart ) {
                    blink1_infos[p].type = BLINK1_MK2;
                }
                p++;
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    
    blink1_cached_count = p;

    blink1_sortCache();

    return p;
}

//
int blink1_getCachedCount(void)
{
    return blink1_cached_count;
}

//
const char* blink1_getCachedPath(int i)
{
    return blink1_infos[i].path;
}
//
const char* blink1_getCachedSerial(int i)
{
    return blink1_infos[i].serial;
}

int blink1_getCacheIndexByPath( const char* path ) 
{
    for( int i=0; i< cache_max; i++ ) { 
        if( strcmp( blink1_infos[i].path, (const char*) path ) == 0 ) return i;
    }
    return -1;
}

int blink1_getCacheIndexBySerial( const char* serial ) 
{
    for( int i=0; i< cache_max; i++ ) { 
        if( strcmp( blink1_infos[i].serial, serial ) == 0 ) return i;
    }
    return -1;
}

int blink1_getCacheIndexByDev( hid_device* dev ) 
{
    for( int i=0; i< cache_max; i++ ) { 
        if( blink1_infos[i].dev == dev ) return i;
    }
    return -1;
}

const char* blink1_getSerialForDev(hid_device* dev)
{
    int i = blink1_getCacheIndexByDev( dev );
    if( i>=0 ) return blink1_infos[i].serial;
    return NULL;
}

int blink1_clearCacheDev( hid_device* dev ) 
{
    int i = blink1_getCacheIndexByDev( dev );
    if( i>=0 ) blink1_infos[i].dev == NULL; // FIXME: hmmmm
    return i;
}

int blink1_isMk2ById( int i )
{
    if( i>=0  && blink1_infos[i].type == BLINK1_MK2 ) return 1;
    return 0;
}

int blink1_isMk2( hid_device* dev )
{
    return blink1_isMk2ById( blink1_getCacheIndexByDev(dev) );
}

//
hid_device* blink1_openByPath(const char* path)
{
    if( path == NULL || strlen(path) == 0 ) return NULL;

    LOG("blink1_openByPath %s\n", path);

    hid_device* handle = hid_open_path( path ); 

    int i = blink1_getCacheIndexByPath( path );
    if( i >= 0 ) {  // good
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
    }
    
    return handle;
}

//
hid_device* blink1_openBySerial(const char* serial)
{
    if( serial == NULL || strlen(serial) == 0 ) return NULL;
    int vid = blink1_vid();
    int pid = blink1_pid();
    
    LOG("blink1_openBySerial %s at vid/pid %x/%x\n", serial, vid,pid);

    wchar_t wserialstr[serialstrmax] = {L'\0'};
#ifdef _WIN32   // omg windows you suck
    swprintf( wserialstr, serialstrmax, L"%S", serial); // convert to wchar_t*
#else
    swprintf( wserialstr, serialstrmax, L"%s", serial); // convert to wchar_t*
#endif
    LOG("serialstr: '%ls' \n", wserialstr );
    hid_device* handle = hid_open(vid,pid, wserialstr ); 
    if( handle ) LOG("got a hid_device handle\n"); 

    int i = blink1_getCacheIndexBySerial( serial );
    if( i >= 0 ) {
        LOG("good, serial was in cache\n");
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
        LOG("uh oh, serial was not in cache\n");
    }

    return handle;
}

//
hid_device* blink1_openById( uint32_t i ) 
{ 
    if( i > blink1_max_devices ) { // then i is a serial number not an array index
        char serialstr[serialstrmax];
        sprintf( serialstr, "%X", i);  // convert to wchar_t* 
        return blink1_openBySerial( serialstr );  
    } 
    else {
        return blink1_openByPath( blink1_getCachedPath(i) );
    }
}

//
hid_device* blink1_open(void)
{
    blink1_enumerate();
    
    return blink1_openById( 0 );
}

//
// FIXME: search through blink1s list to zot it too?
void blink1_close( hid_device* dev )
{
    if( dev != NULL ) {
        blink1_clearCacheDev(dev); // FIXME: hmmm 
        hid_close(dev);
    }
    dev = NULL;
    hid_exit(); // FIXME: this cleans up libusb in a way that hid_close doesn't
}

//
int blink1_write( hid_device* dev, void* buf, int len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int rc = hid_send_feature_report( dev, buf, len );
    // FIXME: put this in an ifdef?
    if( rc==-1 ) {
        fprintf(stderr, "blink1_write error: %ls\n", hid_error(dev));
    }
    return rc;
}

// len should contain length of buf
// after call, len will contain actual len of buf read
int blink1_read( hid_device* dev, void* buf, int len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int rc = hid_send_feature_report(dev, buf, len); // FIXME: check rc

    if( (rc = hid_get_feature_report(dev, buf, len) == -1) ) {
      LOG("error reading data: %s\n",blink1_error_msg(rc));
    }
    return rc;
}


// -------------------------------------------------------------------------
// everything below here doesn't need to know about USB details
// except for a "hid_device*"
// -------------------------------------------------------------------------

#include <unistd.h>

//
int blink1_getVersion(hid_device *dev)
{
    char buf[blink1_buf_size] = {blink1_report_id, 'v' };
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
int blink1_eeread(hid_device *dev, uint16_t addr, uint8_t* val)
{
    char buf[blink1_buf_size] = {blink1_report_id, 'e', addr };
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
int blink1_eewrite(hid_device *dev, uint16_t addr, uint8_t val)
{
    char buf[blink1_buf_size] = {blink1_report_id, 'E', addr, val };

    int rc = blink1_write(dev, buf, sizeof(buf) );
        
    return rc;
}

// FIXME: this doesn't work
int blink1_serialnumread(hid_device *dev, uint8_t** serialnum)
{
    int rc = 0;
    for( int i=0; i<blink1_serialnum_len; i++ ) { // serial num is 8 chars long
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
int blink1_serialnumwrite(hid_device *dev, uint8_t* serialnumstr)
{
    uint8_t serialnum[4];
    serialnum[0] = parseHex( serialnumstr[0] )*16 + parseHex( serialnumstr[1] );
    serialnum[1] = parseHex( serialnumstr[2] )*16 + parseHex( serialnumstr[3] );
    serialnum[2] = parseHex( serialnumstr[4] )*16 + parseHex( serialnumstr[5] );
    serialnum[3] = parseHex( serialnumstr[6] )*16 + parseHex( serialnumstr[7] );

    int rc = 0;
    for( int i=0; i<blink1_serialnum_len; i++ ) { // serialnum is 4 chars long
        blink1_sleep(50); //FIXME: 
        uint8_t v = serialnum[i];
        int rc = blink1_eewrite( dev, blink1_eeaddr_serialnum+i, v);
        if( rc == -1 ) { // try again
            LOG("blink1_serialwrite: oops, trying again on char %d\n",i);
            rc = blink1_eewrite(dev,blink1_eeaddr_serialnum+i, v);
            if( rc == -1 ) { 
                LOG("blink1_serialwrite: error on try again\n");
                break;
            }
        }

    }
    return rc;
}

int blink1_fadeToRGBN(hid_device *dev,  uint16_t fadeMillis,
                      uint8_t r, uint8_t g, uint8_t b, uint8_t n)
{
    int dms = fadeMillis/10;  // millis_divided_by_10

    char buf[blink1_buf_size];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'c';   // command code for 'fade to rgb'
    buf[2] = ((blink1_enable_degamma) ? blink1_degamma(r) : r );
    buf[3] = ((blink1_enable_degamma) ? blink1_degamma(g) : g );
    buf[4] = ((blink1_enable_degamma) ? blink1_degamma(b) : b );
    buf[5] = (dms >> 8);
    buf[6] = dms % 0xff;
    buf[7] = n;

    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}


//
int blink1_fadeToRGB(hid_device *dev,  uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b)
{
    int dms = fadeMillis/10;  // millis_divided_by_10

    uint8_t buf[9];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'c';   // command code for 'fade to rgb'
    buf[2] = ((blink1_enable_degamma) ? blink1_degamma(r) : r );
    buf[3] = ((blink1_enable_degamma) ? blink1_degamma(g) : g );
    buf[4] = ((blink1_enable_degamma) ? blink1_degamma(b) : b );
    buf[5] = (dms >> 8);
    buf[6] = dms % 0xff;
    buf[7] = 0;

    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}

//
int blink1_setRGB(hid_device *dev, uint8_t r, uint8_t g, uint8_t b )
{
    uint8_t buf[blink1_buf_size];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'n';   // command code for "set rgb now"
    buf[2] = ((blink1_enable_degamma) ? blink1_degamma(r) : r );     // red
    buf[3] = ((blink1_enable_degamma) ? blink1_degamma(g) : g );     // grn
    buf[4] = ((blink1_enable_degamma) ? blink1_degamma(b) : b );     // blu
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
    
    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}


// 
// args:
// - on == 1 or 0, enable or disable
// - millis == milliseconds to wait until triggering 
// - st == 1 or 0, stay lit or set off()  (mk2 firmware only)
int blink1_serverdown(hid_device *dev, uint8_t on, uint16_t millis, uint8_t st)
{
    int dms = millis/10;  // millis_divided_by_10

    uint8_t buf[blink1_buf_size];
    buf[0] = blink1_report_id;
    buf[1] = 'D';
    buf[2] = on;
    buf[3] = (dms>>8);
    buf[4] = (dms % 0xff);
    buf[5] = st;
    buf[6] = 0;
    buf[7] = 0;

    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_play(hid_device *dev, uint8_t play, uint8_t pos)
{
    uint8_t buf[blink1_buf_size] = {blink1_report_id, 'p', play, pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}
    
//
int blink1_writePatternLine(hid_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos)
{
    int dms = fadeMillis/10;  // millis_divided_by_10
    r = (blink1_enable_degamma) ? blink1_degamma(r) : r ;
    g = (blink1_enable_degamma) ? blink1_degamma(g) : g ;
    b = (blink1_enable_degamma) ? blink1_degamma(b) : b ;

    uint8_t buf[blink1_buf_size] = {blink1_report_id, 'P', r,g,b, (dms>>8), (dms % 0xff), pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_readPatternLine(hid_device *dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos)
{
    uint8_t buf[blink1_buf_size] = {blink1_report_id, 'R', 0,0,0, 0,0, pos };
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

//
int blink1_savePattern( hid_device *dev )
{
    uint8_t buf[blink1_buf_size];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'W';   // command code for "write pattern to flash"
    buf[2] = 0x55;
    buf[3] = 0xAA;
    buf[4] = 0xCA;
    buf[5] = 0xFE;
    buf[6] = 0x00;
    buf[7] = 0x00;
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_testtest( hid_device *dev)
{
    uint8_t buf[blink1_buf_size] = {blink1_report_id, '!', 0,0,0, 0,0,0 };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    blink1_sleep( 50 ); //FIXME:
    if( rc != -1 ) { // no error
        rc = blink1_read(dev, buf, sizeof(buf));
        for( int i=0; i<sizeof(buf); i++ ) { 
            printf("%2.2x,",(uint8_t)buf[i]);
        }
        printf("\n");
    }
    else { 
        printf("testtest error: rc=%d\n", rc);
    }
    return rc;
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
// qsort C-string comparison function 
int cmp_path(const void *a, const void *b) 
{ 
    return strncmp( (const char *)a, (const char *)b, pathstrmax);
} 

// qsort wchar_t string comparison function 
int cmp_serial(const void *a, const void *b) 
{ 
    return wcsncmp( (const wchar_t *)a, (const wchar_t *)b, serialmax);
} 

//
void blink1_sortPaths(void)
{
    size_t elemsize = sizeof( blink1_cached_paths[0] ); // 128 
    //size_t count = sizeof(blink1_cached_paths) / elemsize; // 16
    
    return qsort( blink1_cached_paths, blink1_cached_count,elemsize,cmp_path);
}

//
void blink1_sortSerials(void)
{
    size_t elemsize = sizeof( blink1_cached_serials[0] ); //  
    //size_t count = sizeof(blink1_cached_serials) / elemsize; // 
    
    qsort( blink1_cached_serials, 
           blink1_cached_count, 
           elemsize, 
           cmp_serial);
}


// qsort wchar_t string comparison function 
int cmp_blink1_info_serial_old(const void *a, const void *b) 
{ 
    blink1_info* bia = (blink1_info*) a;
    blink1_info* bib = (blink1_info*) b;

    return wcsncmp( bia->serial, 
                    bib->serial, 
                    serialstrmax);
} 
*/

// qsort char* string comparison function 
int cmp_blink1_info_serial(const void *a, const void *b) 
{ 
    blink1_info* bia = (blink1_info*) a;
    blink1_info* bib = (blink1_info*) b;

    return strncmp( bia->serial, 
                    bib->serial, 
                    serialstrmax);
} 

void blink1_sortCache(void)
{
    size_t elemsize = sizeof( blink1_info ); //  
    
    qsort( blink1_infos, 
           blink1_cached_count, 
           elemsize, 
           cmp_blink1_info_serial);
}


//
int blink1_vid(void)
{
    uint8_t  rawVid[2] = {USB_CFG_VENDOR_ID};
    int vid = rawVid[0] + 256 * rawVid[1];
    return vid;
}
//
int blink1_pid(void)
{
    uint8_t  rawPid[2] = {USB_CFG_DEVICE_ID};
    int pid = rawPid[0] + 256 * rawPid[1];
    return pid;
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

//
char *blink1_error_msg(int errCode)
{
    /*
    static char buf[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:    return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:  return "The specified device was not found";
        case USBOPEN_ERR_IO:        return "Communication error with device";
        default:
            sprintf(buf, "Unknown USB error %d", errCode);
            return buf;
    }
    */
    return NULL;    /* not reached */
}


/*

//
int blink1_nightlight(hid_device *dev, uint8_t on)
{
    char buf[8] = { blink1_report_id, 'N', on };

    int rc = blink1_write(dev, buf, sizeof(buf) );
    
    return rc;
}


//
int blink1_command(hid_device* dev, int num_send, int num_recv,
                       uint8_t* buf_send, uint8_t* buf_recv )
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int err = 0;
    if( (err = usbhidSetReport(dev, (char*)buf_send, num_send)) != 0) {
        fprintf(stderr,"error writing data: %s\n",blink1_error_msg(err));
        return err;
    }
     
    if( num_recv > 0 ) { 
        int len = num_recv;
        if((err = usbhidGetReport(dev, 0, (char*)buf_recv, &len)) != 0) {
            fprintf(stderr,"error reading data: %s\n",blink1_error_msg(err));
        } else {  // it was good
        }
    }
    return err;
}
*/
