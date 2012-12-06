#include <linux/hidraw.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "blink1raw-lib.h"

#define IDENT_VENDOR        0x27B8
#define IDENT_PRODUCT       0x01ED

#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len) _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len) _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif /* HIDIOCSFEATURE */

int blink1_openByPath(const char *path)
{
	int d = open(path, O_RDWR|O_NONBLOCK);
	if (d < 0)
		return d;

	struct hidraw_devinfo info = {};
	if (ioctl(d, HIDIOCGRAWINFO, &info) < 0
			|| info.vendor != IDENT_VENDOR
			|| info.product != IDENT_PRODUCT)
	{
		close(d);
		return -1;
	}

	return d;
}

void blink1_close(int d)
{
	if (d >= 0)
		close(d);
}

int blink1_write(int d, const void *buf, int len)
{
	return ioctl(d, HIDIOCSFEATURE(len), buf);
}

int blink1_read(int d, void *buf, int len)
{
	int r = blink1_write(d, buf, len);
	if (r < 0)
		return r;
	return ioctl(d, HIDIOCGFEATURE(len), buf);
}
