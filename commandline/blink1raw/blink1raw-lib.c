#include <linux/hidraw.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "blink1raw-lib.h"

#define HIDDEV_DIR	"/sys/bus/hid/devices"
#define BLINK1_VENDOR	0x27B8
#define BLINK1_PRODUCT	0x01ED

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
	if (ioctl(d, HIDIOCGRAWINFO, &info) < 0)
	{
		close(d);
		return -1;
	}

	if (info.vendor != BLINK1_VENDOR || info.product != BLINK1_PRODUCT)
	{
		close(d);
		errno = ENXIO;
		return -1;
	}

	return d;
}

int blink1_open()
{
	char hidpath[512] = HIDDEV_DIR;
	int d = -1;
	struct dirent *ent;
	DIR *hiddir = opendir(hidpath);

	if (!hiddir)
		return -1;
	while (d < 0 && (ent = readdir(hiddir)))
	{
		unsigned short int vend, prod;
		if (sscanf(ent->d_name, "%*4x:%4hx:%4hx.", &vend, &prod) != 2
				|| vend != BLINK1_VENDOR || prod != BLINK1_PRODUCT)
			continue;
		snprintf(hidpath, sizeof(hidpath), "%s/%s/hidraw", HIDDEV_DIR, ent->d_name);
		DIR *rawdir = opendir(hidpath);
		if (!rawdir)
			continue;
		while (d < 0 && (ent = readdir(rawdir)))
		{
			if (strncmp(ent->d_name, "hidraw", 6))
				continue;
			snprintf(hidpath, sizeof(hidpath), "/dev/%s", ent->d_name);
			d = blink1_openByPath(hidpath);
		}
		closedir(rawdir);
	}
	closedir(hiddir);
	if (d < 0)
		errno = ENODEV;
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
