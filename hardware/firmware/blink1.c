/* 
 * blink(1) - BlinkM-like device with USB interface
 *  2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 * 
 * Based on much code from:
 *   Project: hid-custom-rq example by  Christian Starkjohann
 *   LinkM : http://linkm.thingm.com/
 *
 * 
 * Note: blink(1) contains no code from BlinkM.  The circuit is different,
 * the PWM is hardware, not software, and the fading and pattern engine work
 * differently. 
 *
 * 
 */


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  // for sei() 
#include <avr/eeprom.h>
#include <util/delay.h>     // for _delay_ms() 
#include <string.h>         // for memcpy()
#include <inttypes.h>
#include <ctype.h>          // for toupper()

#include "usbdrv.h"

#define blink1_ver_major  '1'
#define blink1_ver_minor  '1'

static uint8_t usbHasBeenSetup;

#include "osccal.h"         // oscillator calibration via USB 

// pins for RGB LED
#define PIN_RED	 PB4   // OCR1B == red    == PB4 == pin3
#define PIN_GRN	 PB0   // OCR0A == green  == PB0 == pin5
#define PIN_BLU	 PB1   // OCR0B == blue   == PB1 == pin6
// pins for USB
#define PIN_USBP PB2   // pin7 == USB D+ must be PB2 / INT0
#define PIN_USBM PB3   // pin2 == USB D-

#define setRedOut(x) ( OCR1B = (x) )
#define setGrnOut(x) ( OCR0A = 255 - (x) )
#define setBluOut(x) ( OCR0B = 255 - (x) )
#define setRGBOut(r,g,b) { setRedOut(r); setGrnOut(g); setBluOut(b); }

#include "color_funcs.h"  // needs "setRGBOut()" defined before inclusion

// number of entries a color pattern can contain
#define patt_max 12

// possible values for boot_mode
#define BOOT_NORMAL      0
#define BOOT_NIGHTLIGHT  1
#define BOOT_MODE_SPACER 0x55

uint8_t ee_osccal          EEMEM = 0;   // used by "osccal.h"
uint8_t ee_bootmode        EEMEM = BOOT_MODE_SPACER; 
uint8_t ee_serialnum[4]    EEMEM = { 0x01, 0xAA, 0x1A, 0x23 };  
// serial num is represented as hex ascii on USB, ==> 8 bytes 
patternline_t ee_pattern[patt_max]  EEMEM = {
    { { 0xff, 0x00, 0x00 },  50 }, // 0
    { { 0x00, 0x00, 0x00 },  50 }, // 1
    { { 0x00, 0xff, 0x00 },  50 }, // 2
    { { 0x00, 0x00, 0x00 },  50 }, // 3
    { { 0x00, 0x00, 0xff },  50 }, // 4
    { { 0x00, 0x00, 0x00 },  50 }, // 5
    { { 0xff, 0xff, 0xff }, 100 }, // 6
    { { 0x00, 0x00, 0x00 }, 100 }, // 7
    { { 0xff, 0x00, 0xff },  50 }, // 8
    { { 0xff, 0xff, 0x00 },  50 }, // 9
    { { 0x00, 0xff, 0xff },  50 }, // 10
    { { 0x00, 0x00, 0x00 }, 100 }, // 11
};



// next time 
const uint32_t led_update_millis = 10;  // tick msec
static uint32_t led_update_next;
static uint32_t pattern_update_next;
static uint16_t serverdown_millis;
static uint32_t serverdown_update_next;

rgb_t cplay;     // holder for currently playing color
uint16_t tplay;
uint8_t playpos;
uint8_t playing; // boolean
// in-memory copy of EEPROM pattern 
patternline_t pattern[patt_max];
/* = {
    { { 0x11, 0x11, 0x11 }, 100 }, // 0
    { { 0x44, 0x44, 0x44 }, 100 }, // 1
    { { 0x88, 0x88, 0x88 }, 100 }, // 2
    { { 0xff, 0xff, 0xff }, 100 }, // 3
    { { 0xff, 0xff, 0xff }, 100 }, // 4
    { { 0xff, 0x00, 0xff },  50 }, // 5
    { { 0xff, 0xff, 0x00 },  50 }, // 6
    { { 0x00, 0xff, 0xff },  50 }, // 7
    { { 0x00, 0x00, 0x00 },  50 }, // 8
    { { 0x00, 0x00, 0x00 }, 100 }, // 9
    { { 0x00, 0x00, 0x00 }, 100 }, // 10
    { { 0x00, 0x00, 0x00 }, 100 }, // 11
    };*/



// ------------------------------------------------------------------------- 
// ----------------------- millis time keeping -----------------------------
// -------------------------------------------------------------------------

volatile uint32_t tick;  // for "millis()" function, a count of 1.024msec units

//
static inline uint32_t millis(void)
{
    return tick;
}

// 8MHz w/clk/8: overflows 1000/(8e6/8/256) = 0.256msec -> "millis" = ~tick/4
// 16.5MHz w/clk/64: overflows 1000/(16.5e6/64/256) = ~1 msec
ISR(SIG_OVERFLOW1,ISR_NOBLOCK)  // NOBLOCK allows USB ISR to run
{
    tick++;  // in this case, tick == millis, to <1%
}

// ------------------------------------------------------------------------- 
// ----------------------------- USB interface ----------------------------- 
// ------------------------------------------------------------------------- 

#define REPORT_COUNT 8

// The following variables store the status of the current data transfer 
static uchar    currentAddress;
static uchar    bytesRemaining;

static uint8_t msgbuf[REPORT_COUNT+1];
//static uint8_t msgbufout[8];

// HID descriptor, 1 report, 8 bytes long
PROGMEM char usbHidReportDescriptor[24] = {
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x95, REPORT_COUNT,            //   REPORT_COUNT (8)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};

// USB serial number is unique per device, stored in EEPROM
int usbDescriptorStringSerialNumber[] = {
    USB_STRING_DESCRIPTOR_HEADER( USB_CFG_SERIAL_NUMBER_LEN ),
    USB_CFG_SERIAL_NUMBER
};

void handleMessage(void);

/* usbFunctionRead() is called when the host requests a chunk of data from
 * the device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar   usbFunctionRead(uchar *data, uchar len)
{
    if(len > bytesRemaining)
        len = bytesRemaining;
    memcpy( data, msgbuf + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return len;
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{
    if(bytesRemaining == 0) {
        handleMessage();
        return 1;            // end of transfer 
    }
    if(len > bytesRemaining) 
        len = bytesRemaining;
    memcpy( msgbuf+currentAddress, data, len );
    currentAddress += len;
    bytesRemaining -= len;

    if(bytesRemaining == 0) {  // FIXME: inelegant
        handleMessage();
        return 1;            // end of transfer 
    }
    return bytesRemaining == 0;  // return 1 if this was the last chunk 
}

//
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t    *rq = (void *)data;
    // HID class request 
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
        // wValue: ReportType (highbyte), ReportID (lowbyte)
        //uint8_t rid = rq->wValue.bytes[0];  // report Id
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  
            // since we have only one report type, we can ignore the report-ID
            bytesRemaining = REPORT_COUNT;
            currentAddress = 0;
            return USB_NO_MSG; // use usbFunctionRead() to obtain data 
        } else if(rq->bRequest == USBRQ_HID_SET_REPORT) {
            // since we have only one report type, we can ignore the report-ID 
            bytesRemaining = REPORT_COUNT;
            currentAddress = 0;
            return USB_NO_MSG; // use usbFunctionWrite() to recv data from host 
        }
    }else{
        // ignore vendor type requests, we don't use any 
    }
    return 0;
}


// ------------------------------------------------------------------------- 

// set blink1 to not playing, and no LEDs lit
void off(void)
{
    playing = 0;
    setRGBt(cplay, 0,0,0);      // starting color
    rgb_setCurr( &cplay );
}

// start playing the light pattern
// playing values: 0 = off, 1 = normal, 2 == playing from powerup
void startPlaying( void )
{
    //playing = playtype; 
    //playpos = 0;
    pattern_update_next = millis(); //now;
    eeprom_read_block( &pattern, &ee_pattern, sizeof(patternline_t)*patt_max);
}

// 
static char tohex(uint8_t num)
{
    num &= 0x0f;
    if( num<= 9 ) return num + '0';
    return num - 10 + 'A';
}

//
// msgbuf[] is 8 bytes long
//  byte0 = command
//  byte1..byte7 = args for command
//
// Available commands: ('x' == implemented)
// x Fade to RGB color       format: {'c', r,g,b,      th,tl, 0,0 }
// x Set RGB color now       format: {'n', r,g,b,        0,0, 0,0 }
// x Serverdown tickle/off   format: {'D', {1/0},th,tl,  0,0, 0,0 }
// x Play/Pause              format: {'p', {1/0},pos,0,  0,0, 0,0 }
// x Set pattern entry       format: {'P', r,g,b, th,tl, i,0 }
// - Read playback loc n
// - Set log2lin vals        format: {'M', i, v0, v1,v2,v3,v4, 0,0 } 
// - Get log2lin val 
// x Read EEPROM location    format: {'e' addr, }
// x Write EEPROM location   format: {'E', addr, val, }
// x Get version             format: {'v',} 
//
// // Save last N cmds for playback : 
//

//
void handleMessage(void)
{
    uint8_t* msgbufp = msgbuf+1;  // skip over report id

    uint8_t cmd = msgbufp[0];
    
    // fade to RGB color
    // command {'c', r,g,b, th,tl, 0,0 } = fade to RGB color over time t
    // where time 't' is a number of 10msec ticks
    //
    if(      cmd == 'c' ) { 
        rgb_t* c = (rgb_t*)(msgbufp+1); // msgbuf[1],msgbuf[2],msgbuf[3]
        uint16_t t = (msgbufp[4] << 8) | msgbufp[5]; // msgbuf[4],[5]
        playing = 0;
        rgb_setDest( c, t );
    }
    // set RGB color immediately  - {'n', r,g,b, 0,0,0,0 } 
    //
    else if( cmd == 'n' ) { 
        rgb_t* c = (rgb_t*)(msgbufp+1);
        rgb_setDest( c, 0 );
        rgb_setCurr( c );
    }
    // play/pause, with position  - {'p', p, 0,0, 0,0,0,0}
    //
    else if( cmd == 'p' ) { 
        playing = msgbufp[1]; 
        playpos = msgbufp[2];
        startPlaying();
    }
    // write color pattern entry - {'P', r,g,b, th,tl, p, 0}
    //
    else if( cmd == 'P' ) { 
        // was doing this copy with a cast, but broke it out for clarity
        patternline_t ptmp;
        ptmp.color.r = msgbufp[1];
        ptmp.color.g = msgbufp[2];
        ptmp.color.b = msgbufp[3];
        ptmp.dmillis = ((uint16_t)msgbufp[4] << 8) | msgbufp[5]; 
        uint8_t p = msgbufp[6];
        if( p >= patt_max ) p = 0;
        // save pattern line to RAM 
        memcpy( &pattern[p], &ptmp, sizeof(patternline_t) );
        eeprom_write_block( &pattern[p], &ee_pattern[p], sizeof(patternline_t));
    }
    // read color pattern entry - {'R', 0,0,0, 0,0, p,0}
    //
    else if( cmd == 'R' ) {
        uint8_t p = msgbufp[6];
        if( p >= patt_max ) p = 0;
        patternline_t ptmp ;
        eeprom_read_block( &ptmp, &ee_pattern[p], sizeof(patternline_t));
        msgbuf[2] = ptmp.color.r;
        msgbuf[3] = ptmp.color.g;
        msgbuf[4] = ptmp.color.b;
        msgbuf[5] = (ptmp.dmillis >> 8);
        msgbuf[6] = (ptmp.dmillis & 0xff);
    }
    // read eeprom byte - { 'e', addr, 0,0, 0,0,0,0}
    //
    else if( cmd == 'e' ) { 
        uint8_t addr = msgbufp[1];
        uint8_t val = eeprom_read_byte( (uint8_t*)(uint16_t)addr ); // dumb
        msgbufp[2] = val;  // put read byte in output buff
    }
    // write eeprom byte - { 'E', addr,val, 0, 0,0,0,0}
    //
    else if( cmd == 'E' ) { 
        uint8_t addr = msgbufp[1];
        uint8_t val  = msgbufp[2];
        if( addr > 0 ) {  // don't let overwrite osccal value
            eeprom_write_byte( (uint8_t*)(uint16_t)addr, val ); // dumb
        }
    }
    // servermode tickle - {'D', {1/0},th,tl,  {1/0},0, 0,0 }
    //
    else if( cmd == 'D' ) {
        uint8_t serverdown_on = msgbufp[1];
        uint16_t t = ((uint16_t)msgbufp[2] << 8) | msgbufp[3]; 
        uint8_t st = msgbuf[4];
        if( serverdown_on ) { 
            serverdown_millis = t;
            serverdown_update_next = millis() + (t*10);
        } else {
            serverdown_millis = 0; // turn off serverdown mode
        }
        if( st == 0 ) {  // reset blink(1) state 
            off();
        }
    }
    // version info
    else if( cmd == 'v' ) { 
        msgbufp[2] = blink1_ver_major;
        msgbufp[3] = blink1_ver_minor;
    }
    else if( cmd == '!' ) { // testing testing
        msgbufp[0] = 0x55;
        msgbufp[1] = 0xAA;
        msgbufp[2] = usbHasBeenSetup;
    }
    else {
        
    }
}

// ------------------------------------------------------------------------- 
// ------------------------ chip setup -------------------------------------
// -------------------------------------------------------------------------

//
// timerInit -- initialize the various PWM & timekeeping functions
//  there are 3 PWMs to be setup and one timer for "millis" counting 
//
static void timerInit(void)
{
    // configure PWM outputs 
    DDRB = _BV( PIN_RED ) | _BV( PIN_GRN ) | _BV( PIN_BLU );

    //WGM00 & WGM01 = FastPWM 
    TCCR0A = _BV(WGM01)  | _BV(WGM00) |
        _BV(COM0A1) | _BV(COM0A0) | 
        _BV(COM0B1) | _BV(COM0B0);
    
    TCCR0B = _BV(CS01)|_BV(CS00); //Timer0 prescaler /64 (1kHz freq @ 16.5MHz)

    TCCR1 = _BV(CS12)|_BV(CS11)|_BV(CS10);  //Timer1 prescaler /64 (1kHz)
    //PWM1B = enable PWM on OCR1B 
    //COM1B1 = clear on compare match, set when 
    GTCCR = _BV(PWM1B) | _BV(COM1B1); 
    //GTCCR = _BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0); // why doesn't this work?

    // create a "millis" by using Timer1 overflow interrupt
    TIFR  |= _BV( TOV1 );
    TIMSK |= _BV( TOIE1 );
}


// ------------------------------------------------------------------------- 
// -------------------------- main logic -----------------------------------
// -------------------------------------------------------------------------

//
static void updateLEDs(void)
{
    uint32_t now = millis();

    // update LED for lading every led_update_millis
    if( (long)(now - led_update_next) > 0 ) { 
        led_update_next += led_update_millis;
        rgb_updateCurrent();
        
        // check for non-computer power up
        if( !usbHasBeenSetup ) { 
            if( !playing && now > 500 ) {  // 500 msec wait
                playing = 2;
                playpos = 0;
                startPlaying();
            }
        }
        else {  // else usb is setup...
            if( playing == 2 ) { // ...but we started a powerup play, so reset
                off();
            }

        }
    }

    // serverdown logic
    if( serverdown_millis != 0 ) {  // i.e. servermode has been turned on
        if( (long)(now - serverdown_update_next) > 0 ) { 
            serverdown_millis = 0;  // disable this check
            playing = 1;
            playpos = 0;
            startPlaying();
        }
    }

    // playing light pattern
    if( playing ) {
        if( (long)(now - pattern_update_next) > 0  ) {
            cplay = pattern[playpos].color;
            tplay = pattern[playpos].dmillis;
            rgb_setDest( &cplay, tplay );
            playpos++;
            if( playpos == patt_max ) playpos = 0; // loop the pattern
            pattern_update_next += tplay*10;
        }
    }
    
}


//
int main(void)
{
    wdt_enable(WDTO_1S);  // watchdog status is preserved on reset

    calibrationLoad();

    timerInit();
    
    // load the serial number from EEPROM into RAM
    for( uint8_t i=0; i< 4; i++ )  {
        uint8_t v = eeprom_read_byte( ee_serialnum + i );
        uint8_t c0 = tohex( v>>4 );
        uint8_t c1 = tohex( v );
        uint8_t p = 1 + (2*i);
        usbDescriptorStringSerialNumber[p+0] = c0;
        usbDescriptorStringSerialNumber[p+1] = c1;
    }

    usbInit();
    usbDeviceDisconnect();

    // fake USB disconnect for > 250 ms 
    for( uint8_t i=255; i>0; i-- ) {
        wdt_reset();
        _delay_ms(1);
        uint8_t j = i>>4;      // not so bright, please
        setRGBOut(j,j,j);      // fade down for fun, let's us see disconnect
    }
    usbDeviceConnect();

    sei();

    off();

    for(;;){                // main event loop 
        wdt_reset();
        usbPoll();
        updateLEDs();
    }
    return 0;
}




// -eof-
