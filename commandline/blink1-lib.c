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

#include "blink1-lib.h"


#define LOG(...) BLINK1_LOG(__VA_ARGS__)

#define pathmax 16
#define pathstrmax 128
#define serialmax (8 + 1) 


// FIXME: use hid_device_info instead with custom sorter on serial or path
static char blink1_cached_paths[pathmax][pathstrmax]; 
static int blink1_cached_count = 0;
static wchar_t blink1_cached_serials[pathmax][serialmax];

//----------------------------------------------------------------------------

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
	        strcpy( blink1_cached_paths[p], cur_dev->path );
	        wcscpy( blink1_cached_serials[p], cur_dev->serial_number );
		p++;
	    }
	}
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    
    blink1_cached_count = p;

    blink1_sortSerials();

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
    return blink1_cached_paths[i];    
}
//
const wchar_t* blink1_getCachedSerial(int i)
{
    return blink1_cached_serials[i];
}

//
hid_device* blink1_openByPath(const char* path)
{
    if( path == NULL || strlen(path) == 0 ) return NULL;
    hid_device* handle = hid_open_path( path ); 
    return handle;
}

//
hid_device* blink1_openBySerial(const wchar_t* serial)
{
    if( serial == NULL || wcslen(serial) == 0 ) return NULL;
    int vid = blink1_vid();
    int pid = blink1_pid();
    
    LOG("opening %ls at vid/pid %x/%x\n", serial, vid,pid);
    hid_device* handle = hid_open(vid,pid, serial ); 
    return handle;
}

//
hid_device* blink1_openById( int i ) 
{ 
    //return blink1_openByPath( blink1_getCachedPath(i) );
    return blink1_openBySerial( blink1_getCachedSerial(i) );
}

//
hid_device* blink1_open(void)
{
    int vid = blink1_vid();
    int pid = blink1_pid();

    hid_device* handle = hid_open(vid,pid, NULL);  // FIXME?

    return handle;
}

//
// FIXME: search through blink1s list to zot it too?
void blink1_close( hid_device* dev )
{
    if( dev != NULL ) 
        hid_close(dev);
    dev = NULL;
    hid_exit(); // FIXME: this cleans up libusb in a way that hid_close doesn't
}

//
int blink1_write( hid_device* dev, const void* buf, int len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int rc = hid_send_feature_report( dev, buf, len );
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
int blink1_getSerialNumber(hid_device *dev, char* buf)
{
    if( dev == NULL ) return -1;
    /*
    wchar_t* wbuf = dev->serial_number;
    int i=0;
    while( wbuf ) { 
        buf[i++] = *wbuf;
    }
    return i;
    */
    return -1;
}

//

// FIXME: 
int readUUID( hid_device* dev, uint8_t** uuid )
{
    return -1;
}
// FIXME:
int setUUID( hid_device* dev, uint8_t* uuid )
{
    return -1;
}


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
