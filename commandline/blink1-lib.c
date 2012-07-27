/*
 * blink(1) C library -- 
 *
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */

#include "blink1-lib.h"

#include <stdio.h>
#include <stdlib.h>

#define blink1_report_id 1

static hid_device* blink1s[ blink1_max_devices ];
static int blink1s_inuse[ blink1_max_devices ];
//static int blink1s_count = 0;

///static blink1_error = 0;

int blink1_maxDevices(void)
{
    return blink1_max_devices;
}

//
hid_device* blink1_getDevice(int i)
{
    return blink1s[i];
}

//----------------------------------------------------------------------------

//
int blink1_openstatic(hid_device **dev)
{
    //uint8_t  rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    //int vid = rawVid[0] + 256 * rawVid[1];
    //int pid = rawPid[0] + 256 * rawPid[1];

    return -1;
}

//
int blink1_openall_byid( int vid, int pid )
{
    for( int i=0; i< 16; i++) { 
        blink1s[i] = NULL;
        blink1s_inuse[i] = 0;
    }
    hid_device* handle = blink1_open(); // FIXME
    blink1s[0] = handle;
    blink1s_inuse[0] = 1;
    if( handle == NULL ) return -1;
    return 1;
}

// returns number of devices opened
int blink1_openall(void)
{
    uint8_t rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    int vid = rawVid[0] + 256 * rawVid[1];
    int pid = rawPid[0] + 256 * rawPid[1];

    return blink1_openall_byid( vid,pid );
}

//
hid_device* blink1_open(void)
{
    uint8_t rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
    int vid = rawVid[0] + 256 * rawVid[1];
    int pid = rawPid[0] + 256 * rawPid[1];

	hid_device* handle = hid_open(vid,pid, NULL);  // FIXME?
	if (!handle) {
		printf("unable to open device\n");
 		return NULL;
	}

    return handle;
}

//
// FIXME: search through blink1s list to zot it too?
void blink1_close( hid_device* dev )
{
    if( dev != NULL ) 
        hid_close(dev);
    dev = NULL;
}

//
int blink1_write( hid_device* dev, void* buf, int len)
{
    //for( int i=0; i<len; i++) printf("0x%2.2x,", ((uint8_t*)buf)[i]);
    //printf("\n");

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
        fprintf(stderr,"error reading data: %s\n",blink1_error_msg(rc));
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
    char buf[9] = {blink1_report_id, 'v' };
    int len = sizeof(buf);

	//hid_set_nonblocking(dev, 0);
    int rc = blink1_write(dev, buf, sizeof(buf));
    usleep(50*1000); // FIXME
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
    char buf[9] = {blink1_report_id, 'e', addr };
    int len = sizeof(buf);

    int rc = blink1_write(dev, buf, len );
    usleep(50*1000); // FIXME
    if( rc != -1 ) // no error
        rc = blink1_read(dev, buf, len );
    if( rc != -1 ) 
        *val = buf[3];
    return rc;
}

//
int blink1_eewrite(hid_device *dev, uint16_t addr, uint8_t val)
{
    char buf[9] = {blink1_report_id, 'E', addr, val };

    int rc = blink1_write(dev, buf, sizeof(buf) );
        
    return rc;
}

//
int blink1_fadeToRGB(hid_device *dev,  uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b)
{
    int dms = fadeMillis/10;  // millis_divided_by_10

    char buf[9];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'c';   // command code for 'fade to rgb'
    buf[2] = r;
    buf[3] = g;
    buf[4] = b;
    buf[5] = (dms >> 8);
    buf[6] = dms % 0xff;

    int rc = blink1_write(dev, buf, sizeof(buf) );

    return rc; 
}

//
int blink1_setRGB(hid_device *dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buf[9];

    buf[0] = blink1_report_id;     // report id
    buf[1] = 'n';   // command code for "set rgb now"
    buf[2] = r;     // red
    buf[3] = g;     // grn
    buf[4] = b;     // blu
    
    int rc = blink1_write(dev, buf, sizeof(buf) );

    if( rc == -1 ) {
        fprintf(stderr,"error writing data: %s\n",blink1_error_msg(rc));
    }
    return rc;  // FIXME: remove fprintf
}

//
int blink1_nightlight(hid_device *dev, uint8_t on)
{
    char buf[9] = { blink1_report_id, 'N', on };

    int rc = blink1_write(dev, buf, sizeof(buf) );
    
    return rc;
}

//
int blink1_serverdown(hid_device *dev, uint8_t on, uint16_t millis)
{
    int dms = millis/10;  // millis_divided_by_10

    char buf[9] = {blink1_report_id, 'D', on, (dms>>8), (dms % 0xff) };

    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}

//
int blink1_writePatternLine(hid_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos)
{
    int dms = fadeMillis/10;  // millis_divided_by_10
    char buf[9] = {blink1_report_id, 'P', r,g,b, (dms>>8), (dms % 0xff), pos };
    int rc = blink1_write(dev, buf, sizeof(buf) );
    return rc;
}



/* ------------------------------------------------------------------------- */

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
