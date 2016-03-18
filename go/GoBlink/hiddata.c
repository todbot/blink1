/* Name: hiddata.c
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: hiddata.c 743 2009-04-15 15:00:49Z cs $
 */

/* 2012-12: Gary Boone: removed win32 section */

#include <stdio.h>
#include "hiddata.h"
#include <string.h>
#include <usb.h>

#define usbDevice   usb_dev_handle  /* use libusb's device structure */

/* ------------------------------------------------------------------------- */

#define USBRQ_HID_GET_REPORT    0x01
#define USBRQ_HID_SET_REPORT    0x09

#define USB_HID_REPORT_TYPE_FEATURE 3


static int  usesReportIDs;

/* ------------------------------------------------------------------------- */

static int usbhidGetStringAscii(usb_dev_handle *dev, int index, char *buf, int buflen)
{
char    buffer[256];
int     rval, i;

    if((rval = usb_get_string_simple(dev, index, buf, buflen)) >= 0) /* use libusb version if it works */
        return rval;
    if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, 0x0409, buffer, sizeof(buffer), 5000)) < 0)
        return rval;
    if(buffer[1] != USB_DT_STRING){
        *buf = 0;
        return 0;
    }
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0];
    rval /= 2;
    /* lossy conversion to ISO Latin1: */
    for(i=1;i<rval;i++){
        if(i > buflen)              /* destination buffer overflow */
            break;
        buf[i-1] = buffer[2 * i];
        if(buffer[2 * i + 1] != 0)  /* outside of ISO Latin1 range */
            buf[i-1] = '?';
    }
    buf[i-1] = 0;
    return i-1;
}

int usbhidOpenDevice(usbDevice_t **device, int vendor, char *vendorName, int product, char *productName, int _usesReportIDs)
{
struct usb_bus      *bus;
struct usb_device   *dev;
usb_dev_handle      *handle = NULL;
int                 errorCode = USBOPEN_ERR_NOTFOUND;
static int          didUsbInit = 0;

    if(!didUsbInit){
        usb_init();
        didUsbInit = 1;
    }
    usb_find_busses();
    usb_find_devices();
    for(bus=usb_get_busses(); bus; bus=bus->next){
        for(dev=bus->devices; dev; dev=dev->next){
            if(dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product){
                char    string[256];
                int     len;
                int     detachrc;
                char    drivername[2];
                handle = usb_open(dev); /* we need to open the device in order to query strings */
                if(!handle){
                    errorCode = USBOPEN_ERR_ACCESS;
                    fprintf(stderr, "Warning: cannot open USB device: %s\n", usb_strerror());
                    continue;
                }

#ifdef linux
                // Likely no need to capture the return code.  It will return an error if no driver is
                // attached, which is A-OK.  Nothing will be detached in that case.
                usb_get_driver_np(handle, 0, drivername, sizeof(drivername));

                // If we have an empty then nothing is attached, otherwise will be something like "dummy".
                if(strlen(drivername) > 0){
                    // Not doing this causes several issues writing control messages to the interface.
                    detachrc = usb_detach_kernel_driver_np(handle, 0);
                    if (detachrc != 0) {
                        errorCode = USBOPEN_ERR_IO;
                        fprintf(stderr, "Warning: cannot detach kernel driver from interface 0: %s\n", usb_strerror());
                        continue;
                    }
                }
#endif
                if(vendorName == NULL && productName == NULL){  /* name does not matter */
                    break;
                }
                /* now check whether the names match: */
                len = usbhidGetStringAscii(handle, dev->descriptor.iManufacturer, string, sizeof(string));
                if(len < 0){
                    errorCode = USBOPEN_ERR_IO;
                    fprintf(stderr, "Warning: cannot query manufacturer for device: %s\n", usb_strerror());
                }else{
                    errorCode = USBOPEN_ERR_NOTFOUND;
                    /* fprintf(stderr, "seen device from vendor ->%s<-\n", string); */
                    if(strcmp(string, vendorName) == 0){
                        len = usbhidGetStringAscii(handle, dev->descriptor.iProduct, string, sizeof(string));
                        if(len < 0){
                            errorCode = USBOPEN_ERR_IO;
                            fprintf(stderr, "Warning: cannot query product for device: %s\n", usb_strerror());
                        }else{
                            errorCode = USBOPEN_ERR_NOTFOUND;
                            /* fprintf(stderr, "seen product ->%s<-\n", string); */
                            if(strcmp(string, productName) == 0)
                                break;
                        }
                    }
                }
                usb_close(handle);
                handle = NULL;
            }
        }
        if(handle)
            break;
    }
    if(handle != NULL){
        errorCode = 0;
        *device = (void *)handle;
        usesReportIDs = _usesReportIDs;
    }
    return errorCode;
}

/* ------------------------------------------------------------------------- */

void    usbhidCloseDevice(usbDevice_t *device)
{
    if(device != NULL)
        usb_close((void *)device);
}

/* ------------------------------------------------------------------------- */

int usbhidSetReport(usbDevice_t *device, char *buffer, int len)
{
int bytesSent, reportId = buffer[0];
int claimrc, releaserc;

    if(!usesReportIDs){
        buffer++;   /* skip dummy report ID */
        len--;
    }
    // original hiddata.h
    //    bytesSent = usb_control_msg((void *)device, USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USBRQ_HID_SET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | (reportId & 0xff), 0, buffer, len, 5000);
    // modification by todbot, matches roughly what hiddata does
    // change by tod to use USB_RECIP_INTERFACE instead of USB_RECIP_DEVICE
    claimrc = usb_claim_interface((void *)device, 0);
    if(claimrc != 0){
        fprintf(stderr, "Error claiming interface 0 before sending a control message: %s\n", usb_strerror());
        return USBOPEN_ERR_IO;
    }

    bytesSent = usb_control_msg((void *)device, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_ENDPOINT_OUT, USBRQ_HID_SET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | (reportId & 0xff), 0, buffer, len, 5000);
    if(bytesSent != len){
        if(bytesSent < 0)
            fprintf(stderr, "Error sending message: %s\n", usb_strerror());

        // Ehhh
        usb_release_interface((void *)device, 0);

        return USBOPEN_ERR_IO;
    }

    releaserc = usb_release_interface((void *)device, 0);
    if(claimrc != 0){
        fprintf(stderr, "Error releasing interface 0 after sending a control message: %s\n", usb_strerror());
        return USBOPEN_ERR_IO;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

int usbhidGetReport(usbDevice_t *device, int reportNumber, char *buffer, int *len)
{
int bytesReceived, maxLen = *len;
int claimrc, releaserc;

    if(!usesReportIDs){
        buffer++;   /* make room for dummy report ID */
        maxLen--;
    }
    // original hiddata.h
    //bytesReceived = usb_control_msg((void *)device, USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USBRQ_HID_GET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | reportNumber, 0, buffer, maxLen, 5000);
    // change by tod to use USB_RECIP_INTERFACE instead of USB_RECIP_DEVICE

    claimrc = usb_claim_interface((void *)device, 0);
    if(claimrc != 0){
        fprintf(stderr, "Error claiming interface 0 before sending a control message: %s\n", usb_strerror());
        return USBOPEN_ERR_IO;
    }

    bytesReceived = usb_control_msg((void *)device, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_ENDPOINT_IN, USBRQ_HID_GET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | reportNumber, 0, buffer, maxLen, 5000);
    if(bytesReceived < 0){
        fprintf(stderr, "Error sending message: %s\n", usb_strerror());

        usb_release_interface((void *)device, 0);

        return USBOPEN_ERR_IO;
    }
    *len = bytesReceived;
    if(!usesReportIDs){
        buffer[-1] = reportNumber;  /* add dummy report ID */
        (*len)++;
    }

    releaserc = usb_release_interface((void *)device, 0);
    if(claimrc != 0){
        fprintf(stderr, "Error releasing interface 0 after sending a control message: %s\n", usb_strerror());
        return USBOPEN_ERR_IO;
    }

    return 0;
}
