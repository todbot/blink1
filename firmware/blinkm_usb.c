/* 
 * BlinkM USB
 * 
 * Originally from:
 * Project: hid-custom-rq example by  Christian Starkjohann
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>
#include <string.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
//#include "requests.h"       /* The custom request numbers we use */



// this is the new world order
#define PIN_RED	 PB4   // OCR1B == red    == PB4 == pin3
#define PIN_GRN	 PB0   // OCR0A == green  == PB0 == pin5
#define PIN_BLU	 PB1   // OCR0B == blue   == PB1 == pin6
// and i guess
#define PIN_USBP PB2   // pin7 == USB D+ must be PB2 / INT0
#define PIN_USBM PB3   // pin2 == USB D-


#define setRed(x) ( OCR1B = (x) )
#define setGrn(x) ( OCR0A = 255 - (x) )
#define setBlu(x) ( OCR0B = 255 - (x) )

#define setRGB(x,y,z) { setRed(x); setGrn(y); setBlu(z); }


/* The following variables store the status of the current data transfer */
static uchar    currentAddress;
static uchar    bytesRemaining;

static uint8_t msgbuf[8];

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    // 0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};
/* The descriptor above is a dummy only, it silences the drivers. The report
 * it describes consists of one byte of undefined data.
 * We don't transfer our data through HID reports, we use custom requests
 * instead.
 */

/* ------------------------------------------------------------------------- */

//
void handleMessage(void)
{
    setRed( msgbuf[0] );
    setGrn( msgbuf[1] );
    setBlu( msgbuf[2] );
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


/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
static void calibrateOscillator(void)
{
    uchar       step = 128;
    uchar       trialValue = 0, optimumValue;
    int         x, optimumDev;
    int         targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();  // proportional to current real frequency 
        if(x < targetValue)           // frequency still too low 
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/

void usbEventResetReady(void)
{
    calibrateOscillator();
    eeprom_write_byte(0, OSCCAL);   // store the calibrated value in EEPROM 
}


// ------------------------------------------------------------------------- 

void pwmInit(void)
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

}

int main(void)
{
    uchar   i;

    uchar   calibrationValue;
    calibrationValue = eeprom_read_byte(0); // calibration value from last time 
    if(calibrationValue != 0xff){
        OSCCAL = calibrationValue;
    }

    pwmInit();

    odDebugInit();
    usbDeviceDisconnect();
    for(i=0;i<30;i++){  // 300 ms disconnect 
        _delay_ms(10);
    }

    wdt_enable(WDTO_1S);
    odDebugInit();
    usbInit();
    usbDeviceDisconnect();  // enforce re-enumeration

    i = 0;
    while(--i) {             // fake USB disconnect for > 250 ms 
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();


    sei();
    for(;;){                // main event loop 
        wdt_reset();
        usbPoll();
    }
    return 0;
}

// -eof-
