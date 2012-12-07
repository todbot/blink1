/*
 * blink(1) C library using raw hiddev linux interface
 *
 * Interface from blink1-lib
 * Based on blink1raw.c
 *
 * 2012, Dylan Simon
 *
 */

#ifndef __BLINK1RAW_LIB_H__
#define __BLINK1RAW_LIB_H__

typedef int blink1_dev;
#define blink1_error(X) ((X) < 0)

blink1_dev blink1_open();
blink1_dev blink1_openByPath(const char *path);
void blink1_close(blink1_dev dev);

int blink1_write(blink1_dev dev, const void *buf, int len);
int blink1_read(blink1_dev dev, void *buf, int len);

#include "../blink1-common.h"

#endif
