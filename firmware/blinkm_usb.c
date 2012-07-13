/* 
 * BlinkM USB - BlinkM-like device with USB interface
 *  2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 * 
 * Based on much code from:
 *   Project: hid-custom-rq example by  Christian Starkjohann
 *   LinkM : http://linkm.thingm.com/
 *
 *
 * Firmware TODOs: (x=done)
 * x detect plugged in to power supply vs computer, 
 * - play pattern if not on computer
 * - log2lin() function, maybe map in memory (256 RAM bytes, compute at boot)
 * - upload new log2lin table
 * - upload of pattern
 * - 
 */


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  // for sei() 
#include <util/delay.h>     // for _delay_ms() 
#include <string.h>         // for memcpy()
#include <inttypes.h>

#include "usbdrv.h"

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

// needs "setRGBOut()" defined
#include "color_funcs.h"

static uint32_t ledUpdateTimeNext;
const uint32_t ledUpdateMillis = 10;  // tick msec


rgb_t cplay;     // holder for currently playing color
uint16_t tplay;
uint8_t playpos;
uint8_t playing; // boolean
scriptline_t pattern[8] = {
    { { 0xff, 0x00, 0x00 }, 100 },
    { { 0x00, 0xff, 0x00 }, 100 },
    { { 0x00, 0x00, 0xff }, 100 },
    { { 0xff, 0xff, 0xff }, 100 },
    { { 0xff, 0xff, 0xff }, 100 },
    { { 0xff, 0x00, 0xff }, 100 },
    { { 0xff, 0xff, 0x00 }, 100 },
    { { 0x00, 0xff, 0xff }, 200 },
};


// ------------------------------------------------------------------------- 
// ----------------------- millis time keeping -----------------------------
// -------------------------------------------------------------------------

// for "millis()" function, a count of 0.256ms units
volatile uint32_t tick;

//
static inline uint32_t millis(void)
{
    return tick;
}

// 8MHz w/clk/8: overflows 1000/(8e6/8/256) = 0.256msec -> "millis" = ~tick/4
// 16.5MHz w/clk/64: overflows 1000/(16.5e6/64/256) = ~1 msec
ISR(SIG_OVERFLOW1,ISR_NOBLOCK)  // NOBLOCK allows USB ISR to run
{
    tick++;  //
}

// ------------------------------------------------------------------------- 
// ----------------------------- USB interface ----------------------------- 
// ------------------------------------------------------------------------- 

// The following variables store the status of the current data transfer 
static uchar    currentAddress;
static uchar    bytesRemaining;

static uint8_t msgbuf[8];

//
PROGMEM char usbHidReportDescriptor[22] = {    // USB report descriptor 
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};

// ------------------------------------------------------------------------- 

//
// msgbuf[] is 8 bytes long
//  byte0 = command
//  byte1..byte7 = args for command
//
// Available commands:
// x Fade to RGB color       format: {'c', r,g,b,      th,tl, 0,0 }
// x Set RGB color now       format: {'n', r,g,b,        0,0, 0,0 }
// - Nightlight mode on/off  format: {'N', {1/0},th,tl,  0,0, 0,0 }
// - Serverdown mode on/off  format: {'D', {1/0},th,tl,  0,0, 0,0 }
//// - Set attern col & t at pos i   : {'P', r,g,b, th,tl, i,0 }
// // Save last N cmds for playback : 
// - Playback
// - Read playback loc n
// - Set log2lin vals        format: {'M', i, v0, v1,v2,v3,v4, 0,0 } 
// - Get log2lin val 
//
void handleMessage(void)
{
    uint8_t cmd = msgbuf[0];
    
    // command {'c', r,g,b, th,tl, 0,0 } = fade to RGB color over time t
    // where time 't' is a number of 10msec ticks
    if(      cmd == 'c' ) { 
        rgb_t* c = (rgb_t*)(msgbuf+1); // msgbuf[1],msgbuf[2],msgbuf[3]
        int t = (msgbuf[4] << 8) | msgbuf[5]; // msgbuf[4],[5]
        uint8_t i = msgbuf[6];
        memcpy( pattern+i, (scriptline_t*)(msgbuf+1), sizeof(scriptline_t) );
        playing = 0;
        rgb_setDest( c, t );
    }
    // command {'n', r,g,b, 0,0,0,0 } == set RGB color immediately
    else if( cmd == 'n' ) { 
        rgb_t* c = (rgb_t*)(msgbuf+1);
        rgb_setDest( c, 0 );
        rgb_setCurr( c );
    }
    else if( cmd == 'e' ) {  // FIXME: not complete
        msgbuf[0] = eeprom_read_byte(0);
    }
    else if( cmd == 'p' ) { 
        playing = msgbuf[1];
        playpos = msgbuf[2];
    }
    else if( cmd == '!' ) { // testing testing
        msgbuf[0] = 0x55;
        msgbuf[1] = 0xAA;
        msgbuf[2] = usbHasBeenSetup;
    }
    else {
        
    }
}

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
            bytesRemaining = 8;
            currentAddress = 0;
            return USB_NO_MSG; // use usbFunctionRead() to obtain data 
        } else if(rq->bRequest == USBRQ_HID_SET_REPORT) {
            // since we have only one report type, we can ignore the report-ID 
            bytesRemaining = 8;
            currentAddress = 0;
            return USB_NO_MSG; // use usbFunctionWrite() to recv data from host 
        }
    }else{
        // ignore vendor type requests, we don't use any 
    }
    return 0;
}


// ------------------------------------------------------------------------- 
// ------------------------ chip setup -------------------------------------
// -------------------------------------------------------------------------

//
// timerInit -- initialize the various PWM & timekeeping functions
//  there are 3 PWMs to be setup and one timer for "millis" counting 
//
void timerInit(void)
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

uint32_t patternUpdateNext;
//
static void updateLEDs(void)
{
    // update LED for lading every ledUpdateMillis
    if( (long)(millis() - ledUpdateTimeNext) > 0 ) { 
        ledUpdateTimeNext += ledUpdateMillis;
        rgb_updateCurrent();
        
        // check for not on computer power up
        if( !playing && millis() > 500 && !usbHasBeenSetup ) { 
            playing = 1;
            patternUpdateNext = millis();
        }
    }
    
    if( playing ) {
        if( (long)(millis() - patternUpdateNext) > 0  ) {
            cplay = pattern[playpos].color;
            tplay = pattern[playpos].dmillis;
            rgb_setDest( &cplay, tplay );
            playpos++;
            if( playpos == 8 ) playpos = 0; // wrap around
            patternUpdateNext += tplay*10;
        }
    }

}

//
int main(void)
{
    wdt_enable(WDTO_1S);  // watchdog status is preserved on reset

    calibrationLoad();

    timerInit();
    
    usbInit();
    usbDeviceDisconnect();

    // fake USB disconnect for > 250 ms 
    for( uint8_t i=255; i>0; i-- ) {
        wdt_reset();
        _delay_ms(1);
        uint8_t j = i>>4;      // not so bright, please
        setRGBOut(j,j,j);      // fade down for fun
    }
    usbDeviceConnect();

    sei();

    setRGBt(cplay, 0,0,0);      // starting color
    rgb_setCurr( &cplay );

    for(;;){                // main event loop 
        wdt_reset();
        usbPoll();
        updateLEDs();
    }
    return 0;
}




/*
static void updateLEDs(void)
{
    uint32_t now = millis();
    if( (now - ledLastMillis) >= ledUpdateMillis ) { 
        ledLastMillis = now;
        rgb_updateCurrent();
    }
}
*/

/*

// scrapped idea for using pinchange to detect bus activity
// (and thus use it for plug/unplug state)
//

//volatile uint8_t pintick;

//
ISR(SIG_PIN_CHANGE,ISR_NOBLOCK)  // NOBLOCK allows USB ISR to run
{
    pintick++;
}

//
void pinChangeInit(void)
{
    PCMSK |= _BV(PCINT3);
    GIMSK |= _BV(PCIE);
}

//
void connectedTest(void)
{
    if( ((long)millis() - connectedTestTimeNext) > 0 ) {
        connectedTestTimeNext += 1000;
        if( pintick < 20 ) { // disconnected
            rgb_t cDisconn = {0xff,0xdd,0x33}; 
            rgb_setCurr( &cDisconn );
        }
        pintick = 0;
    }
}
*/

// -eof-
