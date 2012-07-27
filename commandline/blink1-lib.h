/*
 * blink(1) C library -- 
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */


#ifndef __BLINK1_LIB_H__
#define __BLINK1_LIB_H__

#include <stdint.h>

#include "hidapi.h"
#include "usbconfig.h" // from firmware, for VID,PID,vendor name & product name 

#ifdef __cplusplus
extern "C" {
#endif

#define blink1_max_devices 16


int blink1_vid(void);
int blink1_pid(void);
int blink1_sortpaths(void);

int blink1_enumerate();
int blink1_enumerate_byid(int vid, int pid);
const char* blink1_cached_path(int i);

int blink1_openall(void);
int blink1_openall_byid( int vid, int pid );
int blink1_openstatic(hid_device** dev);

hid_device* blink1_open(void);
hid_device* blink1_open_path(const char* path);
void blink1_close( hid_device* dev );

int blink1_write( hid_device* dev, void* buf, int len);
int blink1_read( hid_device* dev, void* buf, int len);

int blink1_getVersion(hid_device *dev);

int blink1_fadeToRGB(hid_device *dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );

int blink1_setRGB(hid_device *dev, uint8_t r, uint8_t g, uint8_t b );

int blink1_eeread(hid_device *dev, uint16_t addr, uint8_t* val);
int blink1_eewrite(hid_device *dev, uint16_t addr, uint8_t val);

int blink1_nightlight(hid_device *dev, uint8_t on);
int blink1_serverdown(hid_device *dev, uint8_t on, uint16_t millis);

int blink1_writePatternLine(hid_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
//int blink1_playPattern(hid_device *dev,,);

char *blink1_error_msg(int errCode);


#ifdef __cplusplus
}
#endif

#endif
