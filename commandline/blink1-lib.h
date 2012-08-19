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
void blink1_sortPaths(void);
void blink1_sortSerials(void);

int blink1_enumerate();
int blink1_enumerateByVidPid(int vid, int pid);
const char* blink1_getCachedPath(int i);
const wchar_t* blink1_getCachedSerial(int i);
int blink1_getCachedCount(void);

hid_device* blink1_open(void);
hid_device* blink1_openByPath(const char* path);
hid_device* blink1_openBySerial(const wchar_t* serial);
hid_device* blink1_openById( int i );

void blink1_close( hid_device* dev );

int blink1_write( hid_device* dev, void* buf, int len);
int blink1_read( hid_device* dev, void* buf, int len);

int blink1_getSerialNumber(hid_device *dev, char* buf);
int blink1_getVersion(hid_device *dev);

int blink1_fadeToRGB(hid_device *dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );

int blink1_setRGB(hid_device *dev, uint8_t r, uint8_t g, uint8_t b );

int blink1_eeread(hid_device *dev, uint16_t addr, uint8_t* val);
int blink1_eewrite(hid_device *dev, uint16_t addr, uint8_t val);

int blink1_serialnumread(hid_device *dev, uint8_t** serialnumstr);
int blink1_serialnumwrite(hid_device *dev, uint8_t* serialnumstr);

//int blink1_nightlight(hid_device *dev, uint8_t on);
int blink1_serverdown(hid_device *dev, uint8_t on, uint16_t millis);

int blink1_play(hid_device *dev, uint8_t play, uint8_t pos);
int blink1_writePatternLine(hid_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
int blink1_readPatternLine(hid_device *dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos);
//int blink1_playPattern(hid_device *dev,,);

char *blink1_error_msg(int errCode);

void blink1_enableDegamma();
void blink1_disableDegamma();
int blink1_degamma(int n);

void blink1_sleep(uint16_t delayMillis);

#ifdef __cplusplus
}
#endif

#endif
