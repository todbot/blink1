
#ifndef __BLINKMUSB_LIB_H__
#define __BLINKMUSB_LIB_H__

#include <stdint.h>
#include "hiddata.h"
#include "usbconfig.h"  //for VID, PID, vendor name & product name 


usbDevice_t* blinkmusb_getDevice(int i);

int blinkmusb_openall(void);
int blinkmusb_openstatic(usbDevice_t** dev);
usbDevice_t* blinkmusb_open(void);
void blinkmusb_close( usbDevice_t* dev );

int blinkmusb_write( usbDevice_t* dev, void* buf, int len, double timeout);

int blinkmusb_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                               uint8_t r, uint8_t g, uint8_t b );

int blinkmusb_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );

char *blinkmusb_error_msg(int errCode);


#endif
