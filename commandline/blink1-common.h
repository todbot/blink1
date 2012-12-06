#ifndef __BLINK1_COMMON_H__
#define __BLINK1_COMMON_H__

#include <stdint.h>

/* These must be provided by low-level interface */
extern int blink1_write(blink1_dev dev, const void* buf, int len);
extern int blink1_read(blink1_dev dev, void* buf, int len);

/* These are implemented by blink1-common.c */
int blink1_getVersion(blink1_dev dev);

int blink1_eeread(blink1_dev dev, uint16_t addr, uint8_t* val);
int blink1_eewrite(blink1_dev dev, uint16_t addr, uint8_t val);

int blink1_serialnumread(blink1_dev dev, uint8_t serialnumstr[8]);
int blink1_serialnumwrite(blink1_dev dev, const uint8_t serialnumstr[8]);

int blink1_fadeToRGB(blink1_dev dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );

int blink1_setRGB(blink1_dev dev, uint8_t r, uint8_t g, uint8_t b );

//int blink1_nightlight(blink1_dev dev, uint8_t on);
int blink1_serverdown(blink1_dev dev, uint8_t on, uint16_t millis);

int blink1_play(blink1_dev dev, uint8_t play, uint8_t pos);
int blink1_writePatternLine(blink1_dev dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
int blink1_readPatternLine(blink1_dev dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos);
//int blink1_playPattern(blink1_dev dev,,);

void blink1_enableDegamma();
void blink1_disableDegamma();
int blink1_degamma(int n);

void blink1_sleep(uint16_t delayMillis);

#ifdef DEBUG_PRINTF
#define BLINK1_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define BLINK1_LOG(...) do {} while (0)
#endif

#endif
