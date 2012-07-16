//
// blink(1) C library -- 
//
// 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
//
//

#include "blink1-lib.h"

#include <stdio.h>
#include <stdlib.h>

static usbDevice_t* blink1s[16];
static int blink1s_inuse[16];
static int blink1s_count = 0;

///static blink1_error = 0;


//
usbDevice_t* blink1_getDevice(int i)
{
    return blink1s[i];
}

//----------------------------------------------------------------------------

//
int blink1_openstatic(usbDevice_t **dev)
{
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, 
        rawPid[2] = {USB_CFG_DEVICE_ID};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];

    return usbhidOpenDevice(dev, vid,pid, NULL,NULL, 0);
}

//
int blink1_openall(void)
{
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, 
        rawPid[2] = {USB_CFG_DEVICE_ID};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];

    for( int i=0; i< 16; i++) { 
        blink1s[i] = NULL;
        blink1s_inuse[i] = 0;
    }
    int err =  usbhidOpenAllDevices(blink1s, &blink1s_count, vid,pid, 0);
    return err;
}

//
usbDevice_t* blink1_open(void)
{
    usbDevice_t     *dev = NULL;
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, 
        rawPid[2] = {USB_CFG_DEVICE_ID};
    // char         vendorName[] = {USB_CFG_VENDOR_NAME, 0}, 
    //              productName[] = {USB_CFG_DEVICE_NAME, 0};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];
    int             err;

    if((err = usbhidOpenDevice(&dev, vid,pid, NULL,NULL, 0)) != 0){
        fprintf(stderr, "error finding %x:%x: %s\n",vid,pid,blink1_error_msg(err));
        return NULL;
    }
    return dev;
}

//
void blink1_close( usbDevice_t* dev )
{
    usbhidCloseDevice(dev);
    dev = NULL;
    // FIXME: search through blink1s list to zot it too?
}

//
int blink1_write( usbDevice_t* dev, void* buf, int len)
{
    //for( int i=0; i<len; i++) printf("0x%2.2x,", ((uint8_t*)buf)[i]);
    //printf("\n");

    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int err;
    err = usbhidSetReport(dev, buf, len);
    return err;
}

// len should contain length of buf
// after call, len will contain actual len of buf read
int blink1_read( usbDevice_t* dev, void* buf, int* len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int err;
    if( (err = usbhidGetReport(dev, 0, buf, len) != 0) ) {
        fprintf(stderr,"error reading data: %s\n",blink1_error_msg(err));
    }
    return err;
}

//
int blink1_getVersion(usbDevice_t *dev)
{
    
    char buf[9] = { 0, 'v'};
    int len = sizeof(buf);
    int rc;
    rc = blink1_write(dev, buf, sizeof(buf));
    if( rc == 0 ) // no error
        rc = blink1_read(dev, buf, &len);
    if( rc == 0 ) 
        rc = ((buf[2]-'0') * 100) + (buf[3]-'0');  // rc is now version number
    return rc;
}

//
int blink1_fadeToRGB(usbDevice_t *dev,  uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b)
{
    char buffer[9];

    int dms = fadeMillis/10;  // millis_divided_by_10

    buffer[0] = 0;  // report id
    buffer[1] = 'c';
    buffer[2] = r;
    buffer[3] = g;
    buffer[4] = b;
    buffer[5] = (dms >> 8);
    buffer[6] = dms % 0xff;

    int err = blink1_write(dev, buffer, sizeof(buffer) );

    if( err != 0 ) {
        fprintf(stderr,"error writing data: %s\n",blink1_error_msg(err));
    }
    //if( (err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) {
    return err;  // FIXME: remove fprintf
}

//
int blink1_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buffer[9];

    buffer[0] = 0;
    buffer[1] = 'n';
    buffer[2] = r;
    buffer[3] = g;
    buffer[4] = b;
    
    int err = blink1_write(dev, buffer, sizeof(buffer) );

    if( err != 0 ) {
        fprintf(stderr,"error writing data: %s\n",blink1_error_msg(err));
    }
    return err;  // FIXME: remove fprintf
}

//
int blink1_eeread(usbDevice_t *dev, uint16_t addr, uint8_t* val)
{
    return -1;
}

int blink1_eewrite(usbDevice_t *dev, uint16_t addr, uint8_t val)
{
    return -1;
}

int blink1_nightlight(usbDevice_t *dev, uint8_t on)
{
    return -1;
}

int blink1_serverdown(usbDevice_t *dev, uint8_t on, uint16_t millis)
{
    return -1;
}

int blink1_writePatternLine(usbDevice_t *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos)
{
    return -1;
}



/* ------------------------------------------------------------------------- */

//
char *blink1_error_msg(int errCode)
{
    static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:    return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:  return "The specified device was not found";
        case USBOPEN_ERR_IO:        return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

/*
//
int blink1_command(usbDevice_t* dev, int num_send, int num_recv,
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
