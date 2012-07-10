/* 
 * blinkmusb-tool.c --
 *
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>    // for usleep()

#include "blinkmusb-lib.h"


int millis = 500;

int fadeMillis = 200;

int multiMillis = 1000;

int numDevicesToUse = 1;

//---------------------------------------------------------------------------- 

// a simple logarithmic -> linear mapping as a sort of gamma correction
// maps from 0-255 to 0-255
static int log2lin( int n )  
{
  //return  (int)(1.0* (n * 0.707 ));  // 1/sqrt(2)
  return (((1<<(n/32))-1) + ((1<<(n/32))*((n%32)+1)+15)/32);
}

//
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

// --------------------------------------------------------------------------- 

//
static void usage(char *myName)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s read\n", myName);
    fprintf(stderr, "  %s write <listofbytes>\n", myName);
    fprintf(stderr, "  %s blink \n", myName);
    fprintf(stderr, "  %s random <numdevices>\n", myName);
    fprintf(stderr, "  %s rgb <red>,<green>,<blue> \n", myName);
}

//
int main(int argc, char **argv)
{
    usbDevice_t *dev;
    char        buffer[9];    /* room for dummy report ID */
    int         rc;
    
    if(argc < 2){
        usage(argv[0]);
        exit(1);
    }
    char* cmd = argv[1];

    if( 0 ) {     
        if((dev = blinkmusb_open()) == NULL)
            exit(1);
    }
    else { 
        if( blinkmusb_openall() != 0 ) { 
            fprintf(stderr, "no devices found\n");
            exit(1);
        }
        if(1) {
        for( int i=0; i<4; i++ ) {
            fprintf(stderr,"device:%p\n", (void*)blinkmusb_getDevice(i));
        }
        }
        dev = blinkmusb_getDevice(0);
    }
    
    if(strcasecmp(cmd, "read") == 0){
        int len = sizeof(buffer);
        if((rc = usbhidGetReport(dev, 0, buffer, &len)) != 0){
            fprintf(stderr,"error reading data: %s\n",blinkmusb_error_msg(rc));
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
            fprintf(stderr,"error writing data: %s\n",blinkmusb_error_msg(rc));

    }
    else if( strcasecmp(cmd, "rgb") == 0 ) { 
        char colbuf[8];  // 5 more than we need  //FIXME: make uint8_t
        hexread(colbuf, argv[2], sizeof(colbuf));  // cmd w/ hexlist arg
        uint8_t r = colbuf[0];
        uint8_t g = colbuf[1];
        uint8_t b = colbuf[2];
        printf("setting rgb: %2.2x,%2.2x,%2.2x\n", r,g,b );
        int rn = log2lin( r );
        int gn = log2lin( g );
        int bn = log2lin( b );

        rc = blinkmusb_fadeToRGB(dev,fadeMillis, rn,gn,bn);
        //rc=blinkmusb_fadeToRGB(dev,fadeMillis,colbuf[0],colbuf[1],colbuf[2]);
        if( rc ) { // on error, do something, anything. come on.
            printf("error on fadeToRGB\n");
        }
    }
    else if( strcasecmp(cmd, "random") == 0 ) {
        if( argc > 2 ) {
            numDevicesToUse = strtol( argv[2],NULL,0);
            if( numDevicesToUse <1 || numDevicesToUse > 16 ) 
                numDevicesToUse = 1;
        }
        
        while( 1 ) { 
            uint8_t r = log2lin( rand() );
            uint8_t g = log2lin( rand() );
            uint8_t b = log2lin( rand() );
            printf("%d : %2.2x,%2.2x,%2.2x \n", numDevicesToUse, r,g,b);
            
            for( int i=0; i< numDevicesToUse; i++ ) {
                usbDevice_t* mydev = blinkmusb_getDevice(i);
                rc = blinkmusb_fadeToRGB(mydev, fadeMillis,r,g,b);
                if( rc ) { // on error, do something, anything. come on.
                    break;
                }
            }
            usleep( fadeMillis * 1000);
        }
    }
    else if(strcasecmp(cmd, "multi") == 0 ) {
        while( 1 ) {
            uint8_t r = log2lin( rand() );
            uint8_t g = log2lin( rand() );
            uint8_t b = log2lin( rand() );
            printf("%2.2x,%2.2x,%2.2x \n", r,g,b);

            for( int i=0; i< 2; i++ ) { 
                usbDevice_t* bu = blinkmusb_getDevice(i);
                rc = blinkmusb_fadeToRGB(bu, fadeMillis,r,g,b);
                if( rc ) { // on error, do something, anything. come on.
                    printf("error!\n");
                    break;
                }
            }
            usleep( multiMillis * 1000);
        }
    }
    else if( strcasecmp(cmd, "ramp") == 0 ) {
        uint8_t v = 0;
        while( 1 )  { 
            rc = blinkmusb_setRGB( dev, v,v,v );
            if( rc ) { // on error, do something, anything. come on.
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
            rc = blinkmusb_setRGB( dev, v,v,v );
            if( rc )  // on error, do something, anything. come on.
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
        if((rc = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) 
            fprintf(stderr,"error writing data: %s\n",blinkmusb_error_msg(rc));
        printf("done\n");
    }
    else{
        usage(argv[0]);
        exit(1);
    }
    usbhidCloseDevice(dev);
    return 0;
}

