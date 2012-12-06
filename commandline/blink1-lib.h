/*
 * blink(1) C library -- 
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */


#ifndef __BLINK1_LIB_H__
#define __BLINK1_LIB_H__

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

typedef hid_device *blink1_dev;
#define blink1_error(X) ((X) == NULL)

blink1_dev blink1_open(void);
blink1_dev blink1_openByPath(const char* path);
blink1_dev blink1_openBySerial(const wchar_t* serial);
blink1_dev blink1_openById( int i );

void blink1_close( blink1_dev dev );

int blink1_write( hid_device* dev, const void* buf, int len);
int blink1_read( hid_device* dev, void* buf, int len);

#include "blink1-common.h"

int blink1_getSerialNumber(blink1_dev dev, char* buf);

char *blink1_error_msg(int errCode);

#ifdef __cplusplus
}
#endif

#endif
