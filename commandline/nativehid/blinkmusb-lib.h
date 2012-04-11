
#ifndef __BLINKMUSB_LIB_H__
#define __BLINKMUSB_LIB_H__

#include <stdint.h>

#include "usbconfig.h"  //for VID, PID, vendor name & product name 

// if MACOSX
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDDevice.h>
typedef IOHIDDeviceRef usbDevice_t;

usbDevice_t* blinkmusb_open(void);
void blinkmusb_close( usbDevice_t* dev );
int blinkmusb_write( usbDevice_t* dev, void *buf, int len, double timeout);

int blinkmusb_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                               uint8_t r, uint8_t g, uint8_t b );

int blinkmusb_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );

//char *usbErrorMessage(int errCode);


#endif
