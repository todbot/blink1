/* 
 * BlinkM USB - BlinkM-like device with USB interface
 *  2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 * 
 * Originally from:
 * Project: hid-custom-rq example by  Christian Starkjohann
 *
 *
 * Firmware TODOs:
 * - detect plugged in to power supply vs computer, play pattern if former?
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

#include "osccal.h"         // oscialltor calibration via USB 

// pins for RGB LED
#define PIN_RED	 PB4   // OCR1B == red    == PB4 == pin3
#define PIN_GRN	 PB0   // OCR0A == green  == PB0 == pin5
#define PIN_BLU	 PB1   // OCR0B == blue   == PB1 == pin6
// pins for USB
#define PIN_USBP PB2   // pin7 == USB D+ must be PB2 / INT0
#define PIN_USBM PB3   // pin2 == USB D-

#define setRed(x) ( OCR1B = (x) )
#define setGrn(x) ( OCR0A = 255 - (x) )
#define setBlu(x) ( OCR0B = 255 - (x) )

#define setRGB(r,g,b) { setRed(r); setGrn(g); setBlu(b); }

// needs "setRGB()" defined
#include "color_funcs.h"


uint32_t ledLastMillis;
uint32_t ledUpdateMillis = 10;  // tick msec

// ------------------------------------------------------------------------- 

// for "millis()" function, a count of 0.256ms units
volatile uint32_t tick;
//
static uint32_t millis(void)
{
    return tick;
}
// 8MHz w/clk/8: overflows 1000/(8e6/8/256) = 0.256msec -> "millis" = ~tick/4
// 16.5MHz w/clk/64: overflows 1000/(16.5e6/64/256) = ~1 msec
ISR(SIG_OVERFLOW1,ISR_NOBLOCK)  // NOBLOCK allows USB ISR to run still
{
    tick++;  //
}

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

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
// Available commands:
// x Fade to RGB color  format: {'c', r,g,b, th, tl, 0, 0}
// x Set RGB color now  format: {'n', r,g,b,  0,0, 0,0}
// - Save last N commands for playback
// - Playback
// - Read playback loc n
// 
void handleMessage(void)
{
    uint8_t cmd = msgbuf[0];
    
    // command {'c', r,g,b, th,tl, 0,0 } = fade to RGB color over time t
    // where time 't' is a number of 10msec ticks
    if(      cmd == 'c' ) { 
        rgb_t* c = (rgb_t*)(msgbuf+1); //msgbuf[1],msgbuf[2],msgbuf[3]
        int t = (msgbuf[4] << 8) | msgbuf[5];
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

//
void timerInit(void)
{
    //configure outputs on PB0, PB1, PB4 
    //DDRB = _BV(PB0) | _BV(PB1) | _BV(PB4); 
    DDRB = _BV( PIN_RED ) | _BV( PIN_GRN ) | _BV( PIN_BLU );

    //WGM00 & WGM01 = FastPWM 
    TCCR0A = _BV(WGM01)  | _BV(WGM00) |
        _BV(COM0A1) | _BV(COM0A0) | 
        _BV(COM0B1) | _BV(COM0B0);
    
    //TCCR0B = _BV(CS01); //Timer0 prescaler /8  (8kHz freq at 16.5MHz)
    TCCR0B = _BV(CS01)|_BV(CS00); //Timer0 prescaler /64 (1kHz freq @ 16.5MHz)

    //TCCR1 = _BV(CS12);  //Timer1 prescaler /8 (8kHz freq at 16.5MHz)
    TCCR1 = _BV(CS12)|_BV(CS11)|_BV(CS10);  //Timer1 prescaler /8 (1kHz)
    //PWM1B = enable PWM on OCR1B 
    //COM1B1 = clear on compare match, set when 
    GTCCR = _BV(PWM1B) | _BV(COM1B1); 
    //GTCCR = _BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0); // why doesn't this work?


    // create a "millis" by using Timer1 overflow interrupt
    TIFR  |= _BV( TOV1 );
    TIMSK |= _BV( TOIE1 );
}

//
static void updateLEDs(void)
{
    uint32_t now = millis();
    if( (now - ledLastMillis) >= ledUpdateMillis ) { 
        ledLastMillis = now;
        rgb_updateCurrent();
    }
}

// ------------------------------------------------------------------------- 

//
int main(void)
{
    wdt_enable(WDTO_1S);  // watchdog status is preserved on reset

    calibrationLoad();

    timerInit();

    usbInit();
    usbDeviceDisconnect();

    uint8_t i = 0;
    while(--i){             // fake USB disconnect for > 250 ms 
        wdt_reset();
        _delay_ms(1);
        setRGB(i,i,i);      // fade down for funx
    }
    usbDeviceConnect();

    sei();

    rgb_t cBlack = {0x00,0x00,0x00};
    rgb_setCurr( &cBlack );

    for(;;){                // main event loop 
        wdt_reset();
        usbPoll();
        updateLEDs();
    }
    return 0;
}



    /*
    rgb_t cBlack = {0x00,0x00,0x00};
    rgb_t cWhite = {0xff,0xff,0xff};
    rgb_t cRed =   {0xff,0x00,0x00};
    rgb_t cBlue =  {0x00,0x00,0xff};

    while( 1 ) { 

        rgb_setCurr(&cBlue);

        while( (millis() - ledLastMillis) < 1000 ) { wdt_reset(); }
        ledLastMillis = millis();

        rgb_setCurr(&cBlack);

        while( (millis() - ledLastMillis) < 1000 ) { wdt_reset(); }
        ledLastMillis = millis();
    }
    
    */
    


// -eof-
