/*
 * blink(1) C library -- 
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */


#ifndef __BLINK1_LIB_H__
#define __BLINK1_LIB_H__

#include <stdint.h>
#include "hiddata.h"
#include "usbconfig.h" // from firmware, for VID,PID,vendor name & product name 

#define blink1_max_devices 16

int blink1_maxDevices(void);
usbDevice_t* blink1_getDevice(int i);

int blink1_openall(void);
int blink1_openall_byid( int vid, int pid );
int blink1_openstatic(usbDevice_t** dev);
usbDevice_t* blink1_open(void);
void blink1_close( usbDevice_t* dev );

int blink1_write( usbDevice_t* dev, void* buf, int len);
int blink1_read( usbDevice_t* dev, void* buf, int* len);

int blink1_getVersion(usbDevice_t *dev);

int blink1_fadeToRGB(usbDevice_t *dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );

int blink1_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );

int blink1_eeread(usbDevice_t *dev, uint16_t addr, uint8_t* val);
int blink1_eewrite(usbDevice_t *dev, uint16_t addr, uint8_t val);

int blink1_nightlight(usbDevice_t *dev, uint8_t on);
int blink1_serverdown(usbDevice_t *dev, uint8_t on, uint16_t millis);

int blink1_writePatternLine(usbDevice_t *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
//int blink1_playPattern(usbDevice_t *dev, 

char *blink1_error_msg(int errCode);


#endif
