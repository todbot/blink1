/**
 * blink(1) C library -- aka "blink1-lib" 
 *
 * Part of the blink(1) open source hardware project
 * See https://github.com/todbot/blink1 for details
 *
 * 2012-2014, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
 *
 */


#ifndef __BLINK1_LIB_H__
#define __BLINK1_LIB_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define blink1_max_devices 16

#define cache_max 16  
#define serialstrmax (8 + 1) 
#define pathstrmax 128

#define blink1mk2_serialstart 0x20000000

#define  BLINK1_VENDOR_ID       0x27B8 /* = 0x27B8 = 10168 = thingm */
#define  BLINK1_DEVICE_ID       0x01ED /* = 0x01ED */

#define blink1_report_id  1
#define blink1_report_size 8
#define blink1_buf_size (blink1_report_size+1)

enum { 
    BLINK1_UNKNOWN = 0,
    BLINK1_MK1,   // the original one from the kickstarter
    BLINK1_MK2    // the updated one 
}; 

struct blink1_device_;

#if USE_HIDAPI
typedef struct hid_device_ blink1_device; /**< opaque blink1 structure */
#elif USE_HIDDATA
typedef struct usbDevice   blink1_device; /**< opaque blink1 structure */
#else
#warning "USE_HIDAPI or USE_HIDDATA wasn't defined, defaulting to USE_HIDAPI"
typedef struct hid_device_ blink1_device; /**< opaque blink1 structure */
#endif


//
// -------- BEGIN PUBLIC API ----------
//

/**
 * Scan USB for blink(1) devices.
 * @return number of devices found
 */
int          blink1_enumerate();

/**
 * Scan USB for devices by given VID,PID.
 * @param vid vendor ID
 * @param pid product ID
 * @return number of devices found
 */
int          blink1_enumerateByVidPid(int vid, int pid);

/**
 * Open first found blink(1) device.
 * @return pointer to opened blink1_device or NULL if no blink1 found
 */
blink1_device* blink1_open(void);

/**
 * Open blink(1) by USB path.
 * note: this is platform-specific, and port-specific.
 * @param path string of platform-specific path to blink1
 * @return blink1_device or NULL if no blink1 found
 */ 
blink1_device* blink1_openByPath(const char* path);

/**
 * Open blink(1) by 8-digit serial number.
 * @param serial 8-hex digit serial number
 * @return blink1_device or NULL if no blink1 found
 */
blink1_device* blink1_openBySerial(const char* serial);

/**
 * Open by "id", which if from 0-blink1_max_devices is index
 *  or if >blink1_max_devices, is numerical representation of serial number
 * @param id ordinal 0-15 id of blink1 or numerical rep of 8-hex digit serial
 * @return blink1_device or NULL if no blink1 found
 */
blink1_device* blink1_openById( uint32_t id );

/**
 * Close opened blink1 device.  
 * Safe to call blink1_close on already closed device.
 * @param dev blink1_device
 */
void blink1_close( blink1_device* dev );

/**
 * Low-level write to blink1 device.
 * Used internally by blink1-lib
 */
int blink1_write( blink1_device* dev, void* buf, int len);
/**
 * Low-level read from blink1 device.
 * Used internally by blink1-lib
 */
int blink1_read( blink1_device* dev, void* buf, int len);

/**
 * Get blink1 firmware version.
 * @param dev opened blink1 device
 * @return version as scaled int number (e.g. "v1.1" = 101)
 */
int blink1_getVersion(blink1_device *dev);

/**
 * Fade blink1 to given RGB color over specified time.
 * @param dev blink1 device to command
 * @param fadeMillis time to fade in milliseconds 
 * @param r red part of RGB color
 * @param g green part of RGB color
 * @param b blue part of RGB color
 * @return -1 on error, 0 on success
 */
int blink1_fadeToRGB(blink1_device *dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );

/**
 * Fade specific LED on blink1mk2 to given RGB color over specified time.
 * @note For mk2 devices.
 * @param dev blink1 device to command
 * @param fadeMillis time to fade in milliseconds 
 * @param r red part of RGB color
 * @param g green part of RGB color
 * @param b blue part of RGB color
 * @param n which LED to address (0=all, 1=1st LED, 2=2nd LED)
 * @return -1 on error, 0 on success
 */
int blink1_fadeToRGBN(blink1_device *dev, uint16_t fadeMillis,
                      uint8_t r, uint8_t g, uint8_t b, uint8_t n );
/**
 * Set blink1 immediately to a specific RGB color.
 * @note If mk2, sets all LEDs immediately
 * @param dev blink1 device to command
 * @param r red part of RGB color
 * @param g green part of RGB color
 * @param b blue part of RGB color
 * @return -1 on error, 0 on success
 */
int blink1_setRGB(blink1_device *dev, uint8_t r, uint8_t g, uint8_t b );

/**
 * Read current RGB value on specified LED.
 * @note For mk2 devices only.
 * @param dev blink1 device to command
 * @param r pointer to red part of RGB color
 * @param g pointer to green part of RGB color
 * @param b pointer to blue part of RGB color
 * @param n which LED to get (0=1st, 1=1st LED, 2=2nd LED)
 * @return -1 on error, 0 on success
 */
int blink1_readRGB(blink1_device *dev, uint16_t* fadeMillis, 
                   uint8_t* r, uint8_t* g, uint8_t* b, 
                   uint8_t ledn);
/**
 * Attempt to read current RGB value for mk1 devices.
 * @note Called by blink1_setRGB() if device is mk1.
 * @note Does not always work.
 * @param dev blink1 device to command
 * @param r pointer to red part of RGB color
 * @param g pointer to green part of RGB color
 * @param b pointer to blue part of RGB color
 * @return -1 on error, 0 on success
 */
int blink1_readRGB_mk1(blink1_device *dev, uint16_t* fadeMillis,
                       uint8_t* r, uint8_t* g, uint8_t* b);

/** 
 * Read eeprom on mk1 devices
 * @note For mk1 devices only
 */
int blink1_eeread(blink1_device *dev, uint16_t addr, uint8_t* val);
/** 
 * Write eeprom on mk1 devices
 * @note For mk1 devices only
 */
int blink1_eewrite(blink1_device *dev, uint16_t addr, uint8_t val);

/**
 * Read serial number from mk1 device. Does not work.
 * @note Use USB descriptor serial number instead.
 * @note for mk1 devices only.  
 * @note does not work.
 */
int blink1_serialnumread(blink1_device *dev, uint8_t** serialnumstr);
/**
 * Write serial number to mk1 device. Does not work.
 * @note for mk1 devices only.  
 * @note does not work.
 */
int blink1_serialnumwrite(blink1_device *dev, uint8_t* serialnumstr);

/** 
 * Tickle blink1 serverdown functionality.
 * @note 'st' param for mk2 firmware only
 * @param on  enable or disable: enable=1, disable=0
 * @param millis milliseconds to wait until triggering 
 * @param stay lit (st=1) or set off() (st=0)
 */
int blink1_serverdown(blink1_device *dev, uint8_t on, uint16_t millis, 
                      uint8_t st);

/**
 * Play color pattern stored in blink1.
 * @param dev blink1 device to command
 * @param play boolean: 1=play, 0=stop
 * @param pos position to start playing from
 * @return -1 on error, 0 on success
 */
int blink1_play(blink1_device *dev, uint8_t play, uint8_t pos);

/**
 * Play color pattern stored in blink1mk2.
 * @note For mk2 devices only.
 * @param dev blink1 device to command
 * @param play boolean: 1=play, 0=stop
 * @param startpos position to start playing from
 * @param endpos position to end playing
 * @param count number of times to play (0=forever)
 * @return -1 on error, 0 on success
 */
int blink1_playloop(blink1_device *dev, uint8_t play, uint8_t startpos, uint8_t endpos, uint8_t count);

/**
 * Read the current state of a playing pattern.
 * @note For mk2 devices only.
 * @param dev blink1 device to command
 * @param playing pointer to play/stop boolean
 * @param playstart pointer to start position
 * @param playend pointer to end position
 * @param playcount pointer to count left
 * @param playpos pointer to play position
 * @return -1 on error, 0 on success
 */
int blink1_readPlayState(blink1_device *dev, uint8_t* playing, 
                         uint8_t* playstart, uint8_t* playend,
                         uint8_t* playcount, uint8_t* playpos);

/**
 * Write a color pattern line to blink1.
 * @note on mk1 devices, this saves the pattern line to nonvolatile storage.
 * @note on mk2 devices, this only saves to RAM (see savePattern() for nonvol)
 * @param dev blink1 device to command
 * @param r red part of RGB color
 * @param g green part of RGB color
 * @param b blue part of RGB color
 * @param pos pattern line number 0-max_patt (FIXME: put note about this)
 * @return -1 on error, 0 on success
 */
int blink1_writePatternLine(blink1_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
/**
 * Read a color pattern line to blink1.
 * @param dev blink1 device to command
 * @param fadeMillis pointer to milliseconds to fade to RGB color
 * @param r pointer to store red color component
 * @param g pointer to store green color component
 * @param b pointer to store blue color component
 * @return -1 on error, 0 on success
 */
int blink1_readPatternLine(blink1_device *dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos);
/**
 * Read a color pattern line to blink1.
 * @note ledn param only works on unreleased mk2a devices
 * @param dev blink1 device to command
 * @param fadeMillis pointer to milliseconds to fade to RGB color
 * @return -1 on error, 0 on success
 */
int blink1_readPatternLineN(blink1_device *dev, uint16_t* fadeMillis, 
                            uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* ledn,
                            uint8_t pos);
/**
 * Save color pattern in RAM to nonvolatile storage.
 * @note For mk2 devices only.
 * @note this doesn't actually return a proper return value, as the
 *       time it takes to write to flash actually exceeds USB timeout
 * @param dev blink1 device to command
 * @return -1 on error, 0 on success
 */
int blink1_savePattern(blink1_device *dev);

/**
 * Sets 'ledn' parameter for blink1_savePatternLine()
 * @note only for unreleased mk2a devices
 */
int blink1_setLEDN( blink1_device* dev, uint8_t ledn);

int blink1_testtest(blink1_device *dev);

char *blink1_error_msg(int errCode);

/*
 * Enable blink1-lib gamma curve.
 */
void blink1_enableDegamma();

/*
 * Disable blink1-lib gamma curve.
 * @note should probably always have it disabled
 */
void blink1_disableDegamma();
int blink1_degamma(int n);

/**
 * Simple wrapper for cross-platform millisecond delay.
 * @param delayMillis number of milliseconds to wait
 */
void blink1_sleep(uint16_t delayMillis);

/**
 * Vendor ID for blink1 devices.
 * @return blink1 VID
 */
int blink1_vid(void);  // return VID for blink(1)
/**
 * Product ID for blink1 devices.
 * @return blink1 PID
 */
int blink1_pid(void);  // return PID for blink(1)


/**
 * Return platform-specific USB path for given cache index.
 * @param i cache index
 * @return path string
 */
const char*  blink1_getCachedPath(int i);
/**
 * Return bilnk1 serial number for given cache index.
 * @param i cache index
 * @return 8-hexdigit serial number as string
 */
const char*  blink1_getCachedSerial(int i);
/**
 * Return cache index for a given platform-specific USB path.
 * @param path platform-specific path string
 * @return cache index or -1 if not found
 */
int          blink1_getCacheIndexByPath( const char* path );
/**
 * Return cache index for a given blink1 serial number.
 * @param path platform-specific path string
 * @return cache index or -1 if not found
 */
int          blink1_getCacheIndexBySerial( const char* serial );
/**
 * Return cache index for a given blink1_device object.
 * @param dev blink1 device to lookup
 * @return cache index or -1 if not found
 */
int          blink1_getCacheIndexByDev( blink1_device* dev );
/**
 * Clear the blink1 device cache for a given device.
 * @param dev blink1 device 
 * @return cache index that was cleared, or -1 if not found
 */
int          blink1_clearCacheDev( blink1_device* dev );

/**
 * Return serial number string for give blink1 device.
 * @param dev blink device to lookup
 * @return 8-hexdigit serial number string
 */
const char*  blink1_getSerialForDev(blink1_device* dev);
/**
 * Return number of entries in blink1 device cache.
 * @note This is the number of devices found with blink1_enumerate()
 * @return number of cache entries
 */
int          blink1_getCachedCount(void);

/**
 * Returns if device at cache index i is a mk2
 *
 * @return mk2=1, mk1=0
 */
int          blink1_isMk2ById(int i);

/**
 * Returns if given blink1_device is a mk2 or not
 * @param dev blink1 device to check
 * @return mk2=1, mk1=0
 */
int          blink1_isMk2(blink1_device* dev);


#ifdef __cplusplus
}
#endif

#endif
