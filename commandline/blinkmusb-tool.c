/* Name: hidtool.c
 * Project: hid-data example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>    // for usleep()

#include "blinkmusb-lib.h"


int millis = 500;

int fadeMillis = 500;


//---------------------------------------------------------------------------- 

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
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s read\n", myName);
    fprintf(stderr, "  %s write <listofbytes>\n", myName);
    fprintf(stderr, "  %s blink \n", myName);
    fprintf(stderr, "  %s random \n", myName);
    fprintf(stderr, "  %s rgb <red>,<green>,<blue> \n", myName);

}

//
int main(int argc, char **argv)
{
    usbDevice_t *dev;
    char        buffer[9];    /* room for dummy report ID */
    int         err;
    
    if(argc < 2){
        usage(argv[0]);
        exit(1);
    }
    char* cmd = argv[1];


    if((dev = blinkmusb_open()) == NULL)
        exit(1);

    if(strcasecmp(cmd, "read") == 0){
        int len = sizeof(buffer);
        if((err = usbhidGetReport(dev, 0, buffer, &len)) != 0){
            fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
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
        if((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) 
            fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));

    }
    else if( strcasecmp(cmd, "rgb") == 0 ) { 
        char colbuf[8];  // 5 more than we need  //FIXME: make uint8_t
        hexread(colbuf, argv[2], sizeof(colbuf));  // cmd w/ hexlist arg
        printf("setting rgb: %2.2x,%2.2x,%2.2x\n", 
               (uint8_t)colbuf[0], (uint8_t)colbuf[1], (uint8_t)colbuf[2]);
        err = blinkmusb_fadeToRGB(dev,fadeMillis,colbuf[0],colbuf[1],colbuf[2]);
        if( err ) { // on error, do something, anything. come on.
            printf("error on fadeToRGB\n");
        }
    }
    else if( strcasecmp(cmd, "random") == 0 ) {
        while( 1 ) { 
            uint8_t r = rand();
            uint8_t g = rand();
            uint8_t b = rand();
            printf("%2.2x,%2.2x,%2.2x \n", r,g,b);

            err = blinkmusb_fadeToRGB( dev, fadeMillis, r,g,b  );
            if( err ) { // on error, do something, anything. come on.
                break;
            }
            usleep( fadeMillis * 1000);
        }
    }
    else if( strcasecmp(cmd, "ramp") == 0 ) {
        uint8_t v = 0;
        while( 1 )  { 
            err = blinkmusb_setRGB( dev, v,v,v );
            if( err ) { // on error, do something, anything. come on.
                break;
            }
            v++;
            printf("%x,%x,%x \n", v,v,v);
            usleep(millis/10 * 1000 ); // sleep milliseconds
        }
    }
    else if( strcasecmp(cmd, "blink") == 0 ) {
        uint8_t v = 0;
        while( 1 )  { 
            err = blinkmusb_setRGB( dev, v,v,v );
            if( err )  // on error, do something, anything. come on.
                break;

            v = (v) ? 0 : 255;
            millis = millis * 100 / 110;
            if( millis < 10 ) millis = 250;

            printf("%d: %x,%x,%x \n", millis, v,v,v );
            usleep(millis * 1000 ); // sleep milliseconds
        }
    }
    else if( strcasecmp(cmd,"eeprom") == 0 ) {
        buffer[1] = 'e';
        if((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) 
            fprintf(stderr,"error writing data: %s\n",usbErrorMessage(err));
        printf("done\n");
    }
    else{
        usage(argv[0]);
        exit(1);
    }
    usbhidCloseDevice(dev);
    return 0;
}

