/* 
 * BlinkM USB
 * 
 * Originally from:
 * Project: hid-custom-rq example by  Christian Starkjohann
 *
 *
 *
 */


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  // for sei() 
#include <util/delay.h>     // for _delay_ms() 
#include <string.h>         // for memcpy()
#include <inttypes.h>

#include "usbdrv.h"

#include "osccal.h"         // oscialltor calibration via USB 

// this is the new world order
#define PIN_RED	 PB4   // OCR1B == red    == PB4 == pin3
#define PIN_GRN	 PB0   // OCR0A == green  == PB0 == pin5
#define PIN_BLU	 PB1   // OCR0B == blue   == PB1 == pin6
// and i guess
#define PIN_USBP PB2   // pin7 == USB D+ must be PB2 / INT0
#define PIN_USBM PB3   // pin2 == USB D-

// ease of use functions 
#define setRed(x) ( OCR1B = (x) )
#define setGrn(x) ( OCR0A = 255 - (x) )
#define setBlu(x) ( OCR0B = 255 - (x) )

#define setRGB(r,g,b) { setRed(r); setGrn(g); setBlu(b); }

// needs "setRGB()" defined
#include "color_funcs.h"


// ------------------------------------------------------------------------- 

// for "millis()" function, a count of 0.256ms units
static volatile uint32_t tick;
//
uint32_t millis(void)
{
    return tick/4;
}
// overflows every 0.256msec -> "millis" = ~tick/4
ISR(SIG_OVERFLOW1,ISR_NOBLOCK)
{
    tick++;
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
void handleMessage(void)
{
    uint8_t cmd = msgbuf[0];
    
    // command {'c', r,g,b, t, 0,0 } = fade to RGB color over time t
    if( cmd == 'c' ) { 
        rgb_t* c = (rgb_t*)(msgbuf+1);
        int t = msgbuf[3];
        rgb_setDest( c, t );
    }
    // command {'n', r,g,b, 0,0,0,0 } == set RGB color immediately
    else if( cmd == 'n' ) { 
        rgb_t* c = (rgb_t*)(msgbuf+1);
        rgb_setDest( c, 0 );
        rgb_setCurr( c );
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
    
    TCCR0B = _BV(CS01); //Timer0 prescaler /8 

    TCCR1 = _BV(CS12);  //Timer1 prescaler /8 
    //PWM1B = enable PWM on OCR1B 
    //COM1B1 = clear on compare match, set when 
    GTCCR = _BV(PWM1B) | _BV(COM1B1); 
    //GTCCR = _BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0); // why doesn't this work?


    // create a "millis" by using Timer1 overflow interrupt
    TIFR  |= _BV( TOV1 );
    TIMSK |= _BV( TOIE1 );
}

uint32_t ledLastMillis;
const uint32_t ledUpdateMillis = 10;
//
void updateLEDs(void)
{
    if( (millis() - ledLastMillis) > ledUpdateMillis ) { 
        ledLastMillis = millis();
        rgb_updateCurrent();
    }
}

// ------------------------------------------------------------------------- 

//
int main(void)
{
    // Even if you don't use the watchdog, turn it off here. 
    // On newer devices, watchdog status (on/off, period) is PRESERVED ON RESET!
    wdt_enable(WDTO_1S);

    calibrationLoad();

    timerInit();

    usbInit();
    usbDeviceDisconnect();

    sei();  // done already for us I think, but just in case.

    uint8_t i = 0;
    while(--i){             // fake USB disconnect for > 250 ms 
        wdt_reset();
        _delay_ms(1);
        setRGB(i,i,i);      // fade down for funx
    }
    usbDeviceConnect();

    setRGB(0,0,0);          // start at black

    sei();
    for(;;){                // main event loop 
        wdt_reset();
        usbPoll();
        updateLEDs();
    }
    return 0;
}

// -eof-
