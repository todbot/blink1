/*
 * blink(1) C library -- 
 *
 * 2012, Tod E. Kurt, http://todbot.com/blog/ , http://thingm.com/
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


int blink1_vid(void);  // return VID for blink(1)
int blink1_pid(void);  // return PID for blink(1)


const char*  blink1_getCachedPath(int i);
const char*  blink1_getCachedSerial(int i);
int          blink1_getCacheIndexByPath( const char* path );
int          blink1_getCacheIndexBySerial( const char* serial );
int          blink1_getCacheIndexByDev( blink1_device* dev );
int          blink1_clearCacheDev( blink1_device* dev );

const char*  blink1_getSerialForDev(blink1_device* dev);
int          blink1_getCachedCount(void);

int          blink1_isMk2ById(int i);
int          blink1_isMk2(blink1_device* dev);

// scan USB for blink(1) devices
int          blink1_enumerate();
// scan USB for devices by given VID,PID
int          blink1_enumerateByVidPid(int vid, int pid);

// open first found blink(1) device
blink1_device* blink1_open(void);

// open blink(1) by USB path  note: this is platform-specific, and port-specific
blink1_device* blink1_openByPath(const char* path);

// open blink(1) by 8-digit serial number
blink1_device* blink1_openBySerial(const char* serial);

// open by "id", which if from 0-blink1_max_devices is index
// or if >blink1_max_devices, is numerical representation of serial number
blink1_device* blink1_openById( uint32_t i );

void blink1_close( blink1_device* dev );

int blink1_write( blink1_device* dev, void* buf, int len);
int blink1_read( blink1_device* dev, void* buf, int len);

int blink1_getVersion(blink1_device *dev);

int blink1_fadeToRGB(blink1_device *dev, uint16_t fadeMillis,
                     uint8_t r, uint8_t g, uint8_t b );
int blink1_fadeToRGBN(blink1_device *dev, uint16_t fadeMillis,
                      uint8_t r, uint8_t g, uint8_t b, uint8_t n );

int blink1_setRGB(blink1_device *dev, uint8_t r, uint8_t g, uint8_t b );

// mk2 devices only
int blink1_readRGB(blink1_device *dev, uint16_t* fadeMillis, 
                   uint8_t* r, uint8_t* g, uint8_t* b, 
                   uint8_t ledn);
int blink1_readRGB_mk1(blink1_device *dev, uint16_t* fadeMillis,
                       uint8_t* r, uint8_t* g, uint8_t* b);

int blink1_eeread(blink1_device *dev, uint16_t addr, uint8_t* val);
int blink1_eewrite(blink1_device *dev, uint16_t addr, uint8_t val);

int blink1_serialnumread(blink1_device *dev, uint8_t** serialnumstr);
int blink1_serialnumwrite(blink1_device *dev, uint8_t* serialnumstr);

//int blink1_nightlight(blink1_device *dev, uint8_t on);
//int blink1_serverdown(blink1_device *dev, uint8_t on, uint16_t millis);
int blink1_serverdown(blink1_device *dev, uint8_t on, uint16_t millis, uint8_t st);

int blink1_play(blink1_device *dev, uint8_t play, uint8_t pos);
// mk2 devices only
int blink1_playloop(blink1_device *dev, uint8_t play, uint8_t startpos, uint8_t endpos, uint8_t count);
// mk2 devices only
int blink1_readPlayState(blink1_device *dev, uint8_t* playing, 
                         uint8_t* playstart, uint8_t* playend,
                         uint8_t* playcount, uint8_t* playpos);

int blink1_writePatternLine(blink1_device *dev, uint16_t fadeMillis, 
                            uint8_t r, uint8_t g, uint8_t b, 
                            uint8_t pos);
int blink1_readPatternLine(blink1_device *dev, uint16_t* fadeMillis, 
                           uint8_t* r, uint8_t* g, uint8_t* b, 
                           uint8_t pos);
// mk2 devices only
int blink1_savePattern(blink1_device *dev);

int blink1_testtest(blink1_device *dev);

char *blink1_error_msg(int errCode);

void blink1_enableDegamma();
void blink1_disableDegamma();
int blink1_degamma(int n);

void blink1_sleep(uint16_t delayMillis);

#ifdef __cplusplus
}
#endif

#endif
