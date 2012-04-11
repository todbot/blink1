//
//
//
//
//
//

#include "blinkmusb-lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

// Misc stuff
int printf_verbose(const char *format, ...);
int verbose = 3;



int blinkmusb_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                        uint8_t r, uint8_t g, uint8_t b )
{
    char buffer[8];
    int dms = fadeMillis/10;

    buffer[0] = 'c';
    buffer[1] = r;
    buffer[2] = g;
    buffer[3] = b;
    buffer[4] = (dms >> 8);
    buffer[5] = dms % 0xff;
    
    int err = blinkmusb_write( dev, buffer, sizeof(buffer), 100 );

    return err;  // FIXME: remove fprintf
}

int blinkmusb_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buffer[8];

    buffer[0] = 'n';
    buffer[1] = r;
    buffer[2] = g;
    buffer[3] = b;
    
    int err = blinkmusb_write( dev, buffer, sizeof(buffer), 100 );

    return err;  // FIXME: remove fprintf
}


/****************************************************************/
/*                                                              */
/*             USB Access - Apple's IOKit, Mac OS-X             */
/*                                                              */
/****************************************************************/

#if defined(USE_APPLE_IOKIT)

// http://developer.apple.com/technotes/tn2007/tn2187.html
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDDevice.h>

struct usb_list_struct {
	IOHIDDeviceRef ref;
	int pid;
	int vid;
	struct usb_list_struct *next;
};

static struct usb_list_struct *usb_list=NULL;
static IOHIDManagerRef hid_manager=NULL;

void attach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	CFTypeRef type;
	struct usb_list_struct *n, *p;
	int32_t pid, vid;

	if (!dev) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDVendorIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &vid)) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDProductIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &pid)) return;
	n = (struct usb_list_struct *)malloc(sizeof(struct usb_list_struct));
	if (!n) return;
	//printf("attach callback: vid=%04X, pid=%04X\n", vid, pid);
	n->ref = dev;
	n->vid = vid;
	n->pid = pid;
	n->next = NULL;
	if (usb_list == NULL) {
		usb_list = n;
	} else {
		for (p = usb_list; p->next; p = p->next) ;
		p->next = n;
	}
}

void detach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	struct usb_list_struct *p, *tmp, *prev=NULL;

	p = usb_list;
	while (p) {
		if (p->ref == dev) {
			if (prev) {
				prev->next = p->next;
			} else {
				usb_list = p->next;
			}
			tmp = p;
			p = p->next;
			free(tmp);
		} else {
			prev = p;
			p = p->next;
		}
	}
}

void init_hid_manager(void)
{
	CFMutableDictionaryRef dict;
	IOReturn ret;

	if (hid_manager) return;
	hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	if (hid_manager == NULL || CFGetTypeID(hid_manager) != IOHIDManagerGetTypeID()) {
		if (hid_manager) CFRelease(hid_manager);
		printf_verbose("no HID Manager - maybe this is a pre-Leopard (10.5) system?\n");
		return;
	}
	dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (!dict) return;
	IOHIDManagerSetDeviceMatching(hid_manager, dict);
	CFRelease(dict);
	IOHIDManagerScheduleWithRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	IOHIDManagerRegisterDeviceMatchingCallback(hid_manager, attach_callback, NULL);
	IOHIDManagerRegisterDeviceRemovalCallback(hid_manager, detach_callback, NULL);
	ret = IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);
	if (ret != kIOReturnSuccess) {
		IOHIDManagerUnscheduleFromRunLoop(hid_manager,
			CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		CFRelease(hid_manager);
		printf_verbose("Error opening HID Manager");
	}
}

static void do_run_loop(void)
{
	while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true) == kCFRunLoopRunHandledSource) ;
}

IOHIDDeviceRef open_usb_device(int vid, int pid)
{
	struct usb_list_struct *p;
	IOReturn ret;

	init_hid_manager();
	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		if (p->vid == vid && p->pid == pid) {
			ret = IOHIDDeviceOpen(p->ref, kIOHIDOptionsTypeNone);
			if (ret == kIOReturnSuccess) return p->ref;
		}
	}
	return NULL;
}

//
void close_usb_device(IOHIDDeviceRef dev)
{
	struct usb_list_struct *p;

	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		if (p->ref == dev) {
			IOHIDDeviceClose(dev, kIOHIDOptionsTypeNone);
			return;
		}
	}
}

//

static IOHIDDeviceRef iokit_blinkmusb_reference = NULL;

//
usbDevice_t* blinkmusb_open(void)
{
	blinkmusb_close((usbDevice_t*)iokit_blinkmusb_reference);  //FIXME
	//iokit_blinkmusb_reference = open_usb_device( 0x16C0, 0x0478 );
    //{USB_CFG_VENDOR_ID}, {USB_CFG_DEVICE_ID}
	iokit_blinkmusb_reference = open_usb_device( 0x16C0, 0x05df );
    return (usbDevice_t*)iokit_blinkmusb_reference;  // FIXME
	//if (iokit_blinkmusb_reference) return 1;
	//return 0;
}

//
int blinkmusb_write( usbDevice_t* dev, void* buf, int len, double timeout)
{
	IOReturn ret;

	// timeouts do not work on OS-X
	// IOHIDDeviceSetReportWithCallback is not implemented
	// even though Apple documents it with a code example!
	// submitted to Apple on 22-sep-2009, problem ID 7245050
	if (!iokit_blinkmusb_reference) return -1;
	ret = IOHIDDeviceSetReport( iokit_blinkmusb_reference,
                                kIOHIDReportTypeOutput, 0,  // <- reportid
                                buf, len);
	if (ret == kIOReturnSuccess) return 0;
	return ret;
}

//
int blinkmusb_read( void* buf, int len, double timeout)
{
	IOReturn ret;
    CFIndex len2= len;
	if (!iokit_blinkmusb_reference) return -1;
    ret = IOHIDDeviceGetReport( iokit_blinkmusb_reference,
                                kIOHIDReportTypeInput, 0,  // <- reportid
                                buf, &len2);
    return ret;
}


//
void blinkmusb_close(usbDevice_t* dev)
{
	if (!iokit_blinkmusb_reference) return;
	close_usb_device(iokit_blinkmusb_reference);
	iokit_blinkmusb_reference = NULL;
}

/*
int hard_reboot(void)
{
	IOHIDDeviceRef rebootor;
	IOReturn ret;

	rebootor = open_usb_device(0x16C0, 0x0477);
	if (!rebootor) return 0;
	ret = IOHIDDeviceSetReport(rebootor,
		kIOHIDReportTypeOutput, 0, (uint8_t *)("reboot"), 6);
	close_usb_device(rebootor);
	if (ret == kIOReturnSuccess) return 1;
	return 0;
}
*/
#endif



/****************************************************************/
/*                                                              */
/*                       Misc Functions                         */
/*                                                              */
/****************************************************************/

int printf_verbose(const char *format, ...)
{
	va_list ap;
	int r;

	va_start(ap, format);
	if (verbose) {
		r = vprintf(format, ap);
		fflush(stdout);
		return r;
	}
	return 0;
}

void delay(double seconds)
{
	#ifdef WIN32
	Sleep(seconds * 1000.0);
	#else
	usleep(seconds * 1000000.0);
	#endif
}



//----------------------------------------------------------------------------
 /*
//
usbDevice_t* blinkmusb_open(void)
{
    usbDevice_t     *dev = NULL;
    unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, 
        rawPid[2] = {USB_CFG_DEVICE_ID};
    char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, 
        productName[] = {USB_CFG_DEVICE_NAME, 0};
    int             vid = rawVid[0] + 256 * rawVid[1];
    int             pid = rawPid[0] + 256 * rawPid[1];
    int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
        fprintf(stderr, "error finding %s: %s\n", productName, usbErrorMessage(err));
        return NULL;
    }
    return dev;
}

//
void blinkmusb_close( usbDevice_t* dev )
{
    usbhidCloseDevice(dev);
    dev = NULL;
}

//
int blinkmusb_fadeToRGB(usbDevice_t *dev, int fadeMillis,
                        uint8_t r, uint8_t g, uint8_t b )
{
    char buffer[9];
    int err;

    int dms = fadeMillis/10;

    buffer[0] = 0;
    buffer[1] = 'c';
    buffer[2] = r;
    buffer[3] = g;
    buffer[4] = b;
    buffer[5] = (dms >> 8);
    buffer[6] = dms % 0xff;
    
    if( (err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) {
        fprintf(stderr,"error writing data: %s\n",usbErrorMessage(err));
    }
    return err;  // FIXME: remove fprintf
}

//
int blinkmusb_setRGB(usbDevice_t *dev, uint8_t r, uint8_t g, uint8_t b )
{
    char buffer[9];
    int err;

    buffer[0] = 0;
    buffer[1] = 'n';
    buffer[2] = r;
    buffer[3] = g;
    buffer[4] = b;
    
    if( (err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) {
        fprintf(stderr,"error writing data: %s\n",usbErrorMessage(err));
    }
    return err;  // FIXME: remove fprintf
}

// ------------------------------------------------------------------------- 

//
char *usbErrorMessage(int errCode)
{
    static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:    return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:  return "The specified device was not found";
        case USBOPEN_ERR_IO:        return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    // not reached 
}
*/
