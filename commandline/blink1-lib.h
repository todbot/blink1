
#ifndef __BLINK1_LIB_H__
#define __BLINK1_LIB_H__

#include <stdint.h>
#include "hiddata.h"
#include "usbconfig.h"  //for VID, PID, vendor name & product name 


usbDevice_t* blink1_getDevice(int i);

int blink1_openall(void);
int blink1_openstatic(usbDevice_t** dev);
usbDevice_t* blink1_open(void);
void blink1_close( usbDevice_t* dev );

int blink1_write( usbDevice_t* dev, void* buf, int len, double timeout);

int blink1_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                               uint8_t r, uint8_t g, uint8_t b );

int blink1_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b );

char *blink1_error_msg(int errCode);


#endif
