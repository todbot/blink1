#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>    // for usleep()

#include "blinkmusb-lib.h"


int millis = 500;



//
static void usage(char *myName)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s read\n", myName);
    fprintf(stderr, "  %s write <listofbytes>\n", myName);
}

//
int main(int argc, char **argv)
{
    usbDevice_t *dev;
    //char   buffer[9];    /* room for dummy report ID */
    int rc;
    
    if(argc < 2){
        usage(argv[0]);
        exit(1);
    }
    char* cmd = argv[1];


    if((dev = blinkmusb_open()) == NULL) {
        printf("no blinkmusb found\n");
        exit(1);
    }
    printf("doing %s...\n",cmd);
    if( strcasecmp(cmd, "random") == 0 ) {
        int fadeMillis = 1000;
        while( 1 ) { 
            uint8_t r = rand();
            uint8_t g = rand();
            uint8_t b = rand();
            printf("%2.2x,%2.2x,%2.2x \n", r,g,b);

            //rc = blinkmusb_fadeToRGB( dev, fadeMillis, r,g,b  );
            if( !rc ) { // on error, do something, anything. come on.
                break;
            }
            usleep( fadeMillis * 1000);
        }
    }
    else if( strcasecmp(cmd, "ramp") == 0 ) {
        uint8_t v = 0;
        while( 1 )  { 
            rc = blinkmusb_setRGB( dev, v,v,v );
            if( !rc ) { // on error, do something, anything. come on.
                break;
            }
            v++;
            printf("%x,%x,%x \n", v,v,v);
            usleep(millis/10 * 1000 ); // sleep milliseconds
        }
    }
    else if( strcasecmp(cmd, "blink") == 0 ) {
        printf("blinking...\n");
        uint8_t v = 0;
        while( 1 )  { 
            rc = blinkmusb_setRGB( dev, v,v,v );
            if( !rc )  // on error, do something, anything. come on.
                break;

            v = (v) ? 0 : 255;
            millis = millis * 100 / 110;
            if( millis < 10 ) millis = 250;

            printf("%d: %x,%x,%x \n", millis, v,v,v );
            usleep(millis * 1000 ); // sleep milliseconds
        }
    }
    else{
        usage(argv[0]);
        exit(1);
    }

    blinkmusb_close(dev);
    return 0;
}

