/* 
 * blink1-mini-tool -- minimal command-line tool for controlling blink(1)s
 *                     
 * Will work on small unix-based systems that have just libusb-0.1.4
 * No need for pthread & iconv, which is needed for hidapi-based tools
 * 
 * Known to work on:
 * - Ubuntu Linux
 * - Mac OS X 
 * - TomatoUSB WRT / OpenWrt / DD-WRT
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // for memset() et al
#include <stdint.h>    // for uint8_t
#include <unistd.h>    // for usleep()

#include "hiddata.h"

// taken from blink1/hardware/firmware/usbconfig.h
#define IDENT_VENDOR_NUM        0x27B8
#define IDENT_PRODUCT_NUM       0x01ED
#define IDENT_VENDOR_STRING     "ThingM"
#define IDENT_PRODUCT_STRING    "blink(1)"


int millis = 100;

int delayMillis = 1000;

const int blink1_debug = 2;


int blink1_open(usbDevice_t **dev);
char *blink1_error_msg(int errCode);
void blink1_close(usbDevice_t *dev);
int blink1_fadeToRGB(usbDevice_t *dev, int fadeMillis, 
                     uint8_t r, uint8_t g, uint8_t b );
int blink1_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );
static int  hexread(char *buffer, char *string, int buflen);

//
static void usage(char *myName)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s blink [<num>]\n", myName);
    fprintf(stderr, "  %s random [<num>]\n", myName);
    fprintf(stderr, "  %s rgb <red>,<green>,<blue> \n", myName);
    //fprintf(stderr, "  %s read\n", myName);
    //fprintf(stderr, "  %s write <listofbytes>\n", myName);
}

//
int main(int argc, char **argv)
{
    usbDevice_t *dev;
    int         rc;
    
    char argbuf[16];  
    
    if(argc < 2) {
        usage(argv[0]);
        exit(1);
    }
    char* cmd = argv[1];

    if( blink1_open(&dev) ) {
        fprintf(stderr, "error: couldn't open blink1\n");
        exit(1);
    }

    if( strcasecmp(cmd, "rgb") == 0 ) { 
        hexread(argbuf, argv[2], sizeof(argbuf));  // cmd w/ hexlist arg
        uint8_t r = argbuf[0];
        uint8_t g = argbuf[1];
        uint8_t b = argbuf[2];
        printf("cmd:%s rgb:%2.2x,%2.2x,%2.2x in %d ms\n", cmd, r,g,b, millis );
        rc = blink1_fadeToRGB(dev, millis, r,g,b);
        if( rc ) { // on error, do something
            printf("error on fadeToRGB\n");
        }
    }
    else if( strcasecmp(cmd, "blink") == 0 ) {
        if( argc < 3 ) { 
            argbuf[0] = 3;  // blink 3 times if none specified
        } else {
            hexread(argbuf, argv[2], sizeof(argbuf));
        }

        uint8_t v = 0;
        for( int i=0; i< argbuf[0]*2; i++ ) {
            uint8_t r = v;
            uint8_t g = v;
            uint8_t b = v;
            printf("cmd:%s rgb:%2.2x,%2.2x,%2.2x in %d ms\n",cmd, r,g,b,millis);
            rc = blink1_fadeToRGB( dev, millis, v,v,v );
            if( rc ) { // on error, do something
                printf("error on fadeToRGB\n");
            }
            v = (v) ? 0 : 255;
            usleep(millis * 1000 ); // sleep milliseconds
        }
    }
    else if( strcasecmp(cmd, "random") == 0 ) { 
        if( argc < 3 ) { 
            argbuf[0] = 5;  // random 10 times if none specified
        } else {
            hexread(argbuf, argv[2], sizeof(argbuf));
        }
        for( int i=0; i<argbuf[0]; i++ ) { 
            uint8_t r = rand()%255;
            uint8_t g = rand()%255;
            uint8_t b = rand()%255 ;
            printf("cmd:%s rgb:%2.2x,%2.2x,%2.2x in %d ms\n",cmd, r,g,b,millis);
            rc = blink1_fadeToRGB(dev, millis, r,g,b);
            if( rc )  // on error, do something
                printf("error on fadeToRGB\n");
            usleep(millis * 1000 ); // sleep milliseconds
        }
    }
    else { 
        usage(argv[0]);
        exit(1);
    }
    /*
    else if(strcasecmp(cmd, "read") == 0){
        int len = sizeof(buffer);
        if((rc = usbhidGetReport(dev, 0, buffer, &len)) != 0){
            fprintf(stderr,"error reading data: %s\n",blink1_error_msg(rc));
        }else{
            hexdump(buffer + 1, sizeof(buffer) - 1);
        }
    }
    else if(strcasecmp(cmd, "write") == 0){
        int i, pos;
        memset(buffer, 0, sizeof(buffer));
        for(pos = 1, i = 2; i < argc && pos < sizeof(buffer); i++){
            pos += hexread(buffer + pos, argv[i], sizeof(buffer) - pos);
        }

        // add a dummy report ID 
        if((rc = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) 
            fprintf(stderr,"error writing data: %s\n",blink1_error_msg(rc));

    }
    else 
    */

}


/**
 * Open up a blink(1) for transactions.
 * returns 0 on success, and opened device in "dev"
 * or returns non-zero error that can be decoded with blink1_error_msg()
 * FIXME: what happens when multiple are plugged in?
 */
int blink1_open(usbDevice_t **dev)
{
    return usbhidOpenDevice(dev, 
                            IDENT_VENDOR_NUM,  NULL,
                            IDENT_PRODUCT_NUM, NULL,
                            1);  // NOTE: '0' means "not using report IDs"
}

/**
 * Close a Blink1 
 */
void blink1_close(usbDevice_t *dev)
{
    usbhidCloseDevice(dev);
}

/**
 *
 */
int blink1_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                        uint8_t r, uint8_t g, uint8_t b )
{
    char buf[9];
    int err;

    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }

    int dms = fadeMillis/10;  // millis_divided_by_10

    buf[0] = 1;
    buf[1] = 'c';
    buf[2] = r;
    buf[3] = g;
    buf[4] = b;
    buf[5] = (dms >> 8);
    buf[6] = dms % 0xff;
    buf[7] = 0; // ledn
    //buf[8] = 0; // unused

    if( (err = usbhidSetReport(dev, buf, sizeof(buf))) != 0) {
        fprintf(stderr,"fadeToRGB: error writing: %s\n",blink1_error_msg(err));
    }
    return err;  // FIXME: remove fprintf
}

/**
 *
 */
int blink1_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buf[9];
    int err;

    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }

    buf[0] = 1;
    buf[1] = 'n';
    buf[2] = r;
    buf[3] = g;
    buf[4] = b;
    
    if( (err = usbhidSetReport(dev, buf, sizeof(buf))) != 0) {
        fprintf(stderr,"setRGB: error writing: %s\n",blink1_error_msg(err));
    }
    return err;  // FIXME: remove fprintf
}


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

