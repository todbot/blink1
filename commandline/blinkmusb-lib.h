
#ifndef __BLINKMUSB_LIB_H__
#define __BLINKMUSB_LIB_H__

#include <stdint.h>
#include "hiddata.h"
#include "usbconfig.h"  //for VID, PID, vendor name & product name 

usbDevice_t* blinkmusb_open(void);
void blinkmusb_close( usbDevice_t* dev );

int blinkmusb_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                               uint8_t r, uint8_t g, uint8_t b );

int blinkmusb_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );

char *usbErrorMessage(int errCode);


#endif
