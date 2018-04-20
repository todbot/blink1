
/********************************************************************
 * blink(1) mk2 firmware
 *
 * For Microchip PIC16F1455 
 * - http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en556969
 * Uses Microchip's USB Framework (aka "MCHPFSUSB Library")
 * - http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=2680&dDocName=en547784
 *
 * Note: to compile, must have the Microchip Application Library frameworks
 * symlinked in the same directory level as the "blink1mk2" directory.
 * e.g. if you installed the framework in ~/microchip_solutions_v2013-06-15/Microchip
 * then do: ln -s ~/microchip_solutions_v2013-06-15/Microchip Microchip
 *
 * The blink(1) firmware is entirely open source, but the Microchip USB library
 * is not fully open.
 *
 * A modified version of "usb_device.c" from MCHPFSUSB is needed for 
 * RAM-based serial numbers and is provided.
 *
 * PIC16F1455 pins (MLF/QFN pkg) used in blink(1) mk2:
 *
 * pin 1  - RA5            - n/c
 * pin 2  - RA4            - n/c
 * pin 3  - RA3/MCLR/VPP   - VPP testpad, 10k pullup to VDD
 * pin 4  - RC5            - n/c
 * pin 5  - RC4            - n/c
 * pin 6  - RC3            - n/c
 * pin 7  - RC2            - WS2811 serial out (to DI pin on LED1)
 * pin 8  - RC1            - n/c
 * pin 9  - RC0            - n/c
 * pin 10 - VUSB3V3        - 100nF cap to Gnd
 * pin 11 - RA1/D-/ICSPC   - D-  on USB
 * pin 12 - RA0/D+/ICSPD   - D+  on USB
 * pin 13 - VSS            - Gnd on USB
 * pin 14 - n/c            - n/c
 * pin 15 - n/c            - n/c
 * pin 16 - VDD            - +5V on USB, 10uF & 100nF cap to Gnd
 *
 *
 * 2013, Tod E. Kurt, http://thingm.com/
 *
 ********************************************************************/

#ifndef MAIN_C
#define MAIN_C

#include <xc.h>
#include "./USB/usb.h"
#include "HardwareProfile.h"
#include "./USB/usb_function_hid.h"

#include <stdint.h>
#include "flashprog.h"

#if 1 // to fix stupid IDE error issues with __delay_ms
#ifndef _delay_ms(x)
#define _delay_ms(x) __delay_ms(x)
#endif
#ifndef _delay_us(x)
#define _delay_us(x) __delay_us(x)
#endif
#endif

//------------ chip configuration ------------------------------------

//#warning Using Internal Oscillator
#pragma config FOSC     = INTOSC
#pragma config WDTE     = OFF
#pragma config PWRTE    = ON
#pragma config MCLRE    = OFF
#pragma config CP       = OFF
#pragma config BOREN    = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO     = OFF
#pragma config FCMEN    = OFF

#pragma config WRT      = OFF
#pragma config CPUDIV   = NOCLKDIV
#pragma config USBLSCLK = 48MHz
#pragma config PLLMULT  = 3x
#pragma config PLLEN    = ENABLED
#pragma config STVREN   = ON
#pragma config BORV     = LO
#pragma config LPBOR    = OFF
#pragma config LVP      = ON  // keep that on when using LVP programmer

// -------------------------------------------------------------------


#define blink1_ver_major  '2'
#define blink1_ver_minor  '5'

#define blink1_report_id 0x01


// number of ws2811-style LEDs we support
#define nLEDs 18

void setLED(uint8_t r, uint8_t g, uint8_t b, uint8_t n);
void displayLEDs(void);
#define setLEDsAll(r,g,b) { setLED(r,g,b, 255); } // 255 means all


// RGB triplet of 8-bit vals for input/output use
typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} rgb_t;

rgb_t leds[nLEDs];  // NOTE: rgb_t is G,R,B formatted

// note: ws2811.h requires the following be defined:
// "nLEDs" -- number LEDs
// "leds" -- array of led data rgb_t format

#include "ws2811.h"

// note: color_funcs.h requires the following be defined:
// "nLEDs" -- number of LEDs
// "setLED()" -- set the color of a single LED
// "displayLEDs()" -- push LED data to LED strip
//
#include "color_funcs.h"  // provides color struct & slewing


// next time
const uint32_t led_update_millis = 10;  // tick msec
uint32_t led_update_next;
uint32_t pattern_update_next;
uint16_t serverdown_millis;
uint32_t serverdown_update_next;

rgb_t ctmp;      // temp color holder
uint16_t ttmp;   // temp time holder
uint8_t ledn;    // temp ledn holder


// number of entries a color pattern can contain
#define patt_max 32
#define patt_maxflash 16

uint8_t playstart_serverdown = 0;        // start play position for serverdown
uint8_t playend_serverdown   = patt_max; // end play position for serverdown

uint8_t playpos   = 0; // current play position
uint8_t playstart = 0; // start play position
uint8_t playend   = patt_max; // end play position
uint8_t playcount = 0; // number of times to play loop, or 0=infinite
uint8_t playing; // playing values: 0 = off, 1 = normal, 2 == playing from powerup playing=3 direct led addressing FIXME: this is dumb
uint8_t do_pattern_write = 0;

patternline_t ptmp;  // temp pattern holder

// in-memory copy of non-volatile pattern
patternline_t pattern[patt_max]; 

// non-volatile color pattern
// high-endurance flash starts at 0x1F80 - 0x1FFF = 128 bytes
// bank write size is 32-bytes => 128 / 32 = 4 banks
// serial number lives in upper 32-byte bank, so 3 banks (96 bytes) easily usable
//
// old: 128 bytes / 5 bytes/patternline = 25.6 pattern lines
// new: 128 bytes / 6 bytes/patternline = 21.3 pattern lines
// old: at patt_max=12, this is ((3+2)*12) = 60 bytes, so takes two 32-byte flash writes (=>64 bytes)
// new: at patt_max=12, this is ((3+2+1)*12) = 72 bytes, so takes three 32-byte flash writes (= 96 bytes)
// new: at patt_max=16, this is ((3+2+1)*16) = 96 bytes, so takes three 32-byte flash writes (= 96 bytes)
const patternline_t patternflash[patt_maxflash] @ 0x1F80 =
{
    //    G     R     B    fade ledn
    { { 0x00, 0xff, 0x00 },  50, 1 }, // 0  red A
    { { 0x00, 0xff, 0x00 },  50, 2 }, // 1  red B
    { { 0x00, 0x00, 0x00 },  50, 0 }, // 2  off both
    { { 0xff, 0x00, 0x00 },  50, 1 }, // 3  grn A
    { { 0xff, 0x00, 0x00 },  50, 2 }, // 4  grn B
    { { 0x00, 0x00, 0x00 },  50, 0 }, // 5  off both
    { { 0x00, 0x00, 0xff },  50, 1 }, // 6  blu A
    { { 0x00, 0x00, 0xff },  50, 2 }, // 7  blu B
    { { 0x00, 0x00, 0x00 },  50, 0 }, // 8  off both
    { { 0x80, 0x80, 0x80 }, 100, 0 }, // 9  half-bright, both LEDs
    { { 0x00, 0x00, 0x00 }, 100, 0 }, // 10 off both
    { { 0xff, 0xff, 0xff },  50, 1 }, // 11 white A
    { { 0x00, 0x00, 0x00 },  50, 1 }, // 12 off A
    { { 0xff, 0xff, 0xff },  50, 2 }, // 13 white B
    { { 0x00, 0x00, 0x00 }, 100, 2 }, // 14 off B
    { { 0x00, 0x00, 0x00 }, 100, 0 }, // 15 off everyone
};

// this is just for fun, can delete it
// we have 4 more bytes we can write in this 2x32-byte block
//const uint16_t todfoo @ 0x1F80 + (sizeof(patternline_t)*patt_max) = 0x6969;
//#define patternline_size 6  // can't use sizeof(patternline_t)
//const uint16_t todfoo @ 0x1F80 + (patternline_size*patt_max) = 0x6969;

// serial number of this blink(1), 
// stored in a packed format at address 0x1FF8
const uint8_t serialnum_packed[4] @ 0x1FF8 = {0x21, 0x43, 0xba, 0xdc };


#if defined( RAM_BASED_SERIALNUMBER )
RAMSNt my_RAMSN;
#endif



/** VARIABLES ******************************************************/

#if defined(__XC8)
#if defined(_16F1459) || defined(_16F1455)
#define RX_DATA_BUFFER_ADDRESS @0x2050
#define TX_DATA_BUFFER_ADDRESS @0x20A0
// FIXME: this is ugly
#define IN_DATA_BUFFER_ADDRESS 0x2050
#define OUT_DATA_BUFFER_ADDRESS (IN_DATA_BUFFER_ADDRESS + HID_INT_IN_EP_SIZE)
#define FEATURE_DATA_BUFFER_ADDRESS (OUT_DATA_BUFFER_ADDRESS + HID_INT_OUT_EP_SIZE)
#define FEATURE_DATA_BUFFER_ADDRESS_TAG @FEATURE_DATA_BUFFER_ADDRESS
//
#endif
#else
#define RX_DATA_BUFFER_ADDRESS
#define TX_DATA_BUFFER_ADDRESS
#endif

uint8_t hid_send_buf[USB_EP0_BUFF_SIZE] FEATURE_DATA_BUFFER_ADDRESS_TAG;

uint8_t usbHasBeenSetup = 0;  // set in USBCBInitEP()
#define usbIsSetup (USBGetDeviceState() == CONFIGURED_STATE)
// previous ideas on how to handle this
//#define usbHasBeenSetup (!(USBGetDeviceState() < CONFIGURED_STATE))
//#define usbHasBeenSetup ((USBGetDeviceState() != CONFIGURED_STATE) || (USBIsDeviceSuspended() == TRUE))
//#define usbHasBeenSetup ((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))

//
static void InitializeSystem(void);
//void ProcessIO(void);
void USBCBSendResume(void);
void USBHIDCBSetReportComplete(void);


// ----------------------- millis time keeping -----------------------------

volatile uint32_t tick;  // for "millis()" function, a count of 1.024msec units

//These are your actual interrupt handling routines.
// PIC16F has only one interrupt vector,
// so must check all possible interrupts when interrupt occurs
void interrupt ISRCode()
{
    //Check which interrupt flag caused the interrupt.
    //Service the interrupt
    //Clear the interrupt flag
    //Etc.
#if defined(USB_INTERRUPT)
    USBDeviceTasks();
#endif

    // Timer2 Interrupt- Freq = 1045.75 Hz - Period = 0.000956 seconds
    if( TMR2IF ) { // timer 2 interrupt flag
        tick++;
        TMR2IF = 0; // clears TMR2IF       bit 1 TMR2IF: TMR2 to PR2 Match Interrupt Flag bit
    }
}

// millis() returns current time-since-power-up in milliseconds
#if 0
static inline uint32_t millis(void)
{
    return tick;
}
#else
// hack because this compiler has no optimizations
#define millis() (tick)
#endif

// delays for a specified time in milliseconds, using our millis functionality
void delaymillis(uint16_t t)
{
    uint32_t now = tick;
    while( (uint16_t)(tick-now) <= t );
    //uint32_t future = tick+t;
    //while( tick < future );
}

#define delay(x) delaymillis(x)


// ------------------- utility functions -----------------------------------
//
static char tohex(uint8_t num)
{
    num &= 0x0f;
    if( num<= 9 ) return num + '0';
    return num - 10 + 'A';
}

#if defined( RAM_BASED_SERIALNUMBER )
 // load the serial number from packed flash into RAM
inline void loadSerialNumber(void)
{
    for( uint8_t i=0; i< 4; i++ )  {
        uint8_t v = serialnum_packed[i];
        uint8_t c0 = tohex( v>>4 );
        uint8_t c1 = tohex( v );
        uint8_t p = 0 + (2*i);
        my_RAMSN.SerialNumber[p+0] = c0;
        my_RAMSN.SerialNumber[p+1] = c1;
    }
}
#else
#define loadSerialNumber()
#endif


// Can't write flash ("PFM") with early revisions of this chip
// see: http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en556969
// so these two functions are in support of flash writing
// Slow the clock down to 4MHz with PLL off
#define clock_slow() { OSCCONbits.IRCF = 0b1101; }
// Put speed up to 48MHz (16MHz, PLL on)
#define clock_fast() { OSCCONbits.IRCF = 0b1111; }


// Write the RAM color pattern to flash
// NOTE: this slows clock down to 4MHz, might cause issues
// see errata on http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en556969
// for description of why we must slow to 4MHz and turn off PLL
// NOTE:
// FIXME: this is hard-coded for the number of 32-byte banks to be written
inline void writePatternFlash(void)
{
    di();
    clock_slow();  // NOTE: must do this because of chip errata
    flash_modify( ((uint8_t*)patternflash +  0), (((uint8_t*)pattern)+ 0) );
    flash_modify( ((uint8_t*)patternflash + 32), (((uint8_t*)pattern)+32) );
    flash_modify( ((uint8_t*)patternflash + 64), (((uint8_t*)pattern)+64) );
    //flash_modify( ((uint8_t*)0x1f80) + 32 , (((uint8_t*)pattern)+32) );
    clock_fast();
    ei();
}


// -------- LED & color pattern handling -------------------------------------

// actually set the color of a particular LED, or all of them
void setLED(uint8_t r, uint8_t g, uint8_t b, uint8_t n)
{
    if (n == 255) { // all of them
        for (int i = 0; i < nLEDs; i++) {
            leds[i].r = r;
            leds[i].g = g;
            leds[i].b = b;
        }
    }
    else {    // else just one LED, not all of them
        leds[n].r = r; leds[n].g = g; leds[n].b = b;
    }
}

// make sure only called in main code, no interrupts
// otherwise compiler will copy it and you will be sad
void displayLEDs(void)
{
    ws2811_showRGB();
}

// set blink1 to not playing, and no LEDs lit
void off(void)
{
    playing = 0;
    setRGBt(ctmp, 0,0,0);      // starting color
    rgb_setCurr( &ctmp );  // FIXME: better way to do this?
}

// start playing the light pattern
// playing values: 0 = off, 1 = normal, 2 == playing from powerup
void startPlaying( void )
{
    playpos = playstart;
    pattern_update_next = millis(); // now;
    //pattern_update_next = 0; // invalidate it so plays immediately
    //memcpy( pattern, patternflash, sizeof(patternline_t)*patt_max);
}

//
// updateLEDs() is the main user-land function that:
// - periodically calls the rgb fader code to fade any actively moving colors
// - controls sequencing of a light pattern, if playing
// - triggers pattern playing on USB disconnect
//
void updateLEDs(void)
{
    uint32_t now = millis();

    // update LEDs every led_update_millis
    if( (long)(now - led_update_next) > 0 ) {
        led_update_next += led_update_millis;

        rgb_updateCurrent();  // playing=3 => direct LED addressing (not anymore)
        displayLEDs();

        // check for non-computer power up
        if( !usbHasBeenSetup ) {
            if( !playing && now > 500 ) {  // 500 msec wait
                playing = 2;
                startPlaying();
            }
        }
        else {  // else usb is setup...
            if( playing == 2 ) { // ...but we started a powerup play, so reset
                off();
            }
        }
    } // led_update_next

    // serverdown logic
    if( serverdown_millis != 0 ) {  // i.e. servermode has been turned on
        if( (long)(now - serverdown_update_next) > 0 ) {
            serverdown_millis = 0;  // disable this check
            playing = 1;
            playstart = playstart_serverdown;
            playend   = playend_serverdown;
            startPlaying();
        }
    }

    // playing light pattern
    if( playing ) {
        if( (long)(now - pattern_update_next) > 0  ) { // time to get next line
            ctmp = pattern[playpos].color;
            ttmp = pattern[playpos].dmillis;
            ledn = pattern[playpos].ledn;
            if( ttmp == 0 && ctmp.r == 0 & ctmp.g == 0 && ctmp.b == 0) {
                // skip lines set to zero
            } else {
                rgb_setDest( &ctmp, ttmp, ledn );
            }
            playpos++;
            if( playpos == playend ) {
                playpos = playstart; // loop the pattern
                playcount--;
                if( playcount == 0 ) {
                    playing=0; // done!
                }
                else if(playcount==255) {
                    playcount = 0; // infinite playing
                }
            }
            pattern_update_next += ttmp*10;
        }
    } // playing

}

//
// user-land function to perform tasks asked for my interrupt-level code
//
void updateMisc(void)
{
    if( do_pattern_write ) { 
        do_pattern_write = 0;
        writePatternFlash();
    }
}


// ------------- USB command handling ----------------------------------------

// handleMessage(char* msgbuf) -- main command router
//
// msgbuf[] is 8 bytes long
//  byte0 = report-id
//  byte1 = command
//  byte2..byte7 = args for command
//
// Available commands:
//    - Fade to RGB color       format: { 1, 'c', r,g,b,     th,tl, n }
//    - Set RGB color now       format: { 1, 'n', r,g,b,       0,0, n } (*)
//    - Read current RGB color  format: { 1, 'r', n,0,0,       0,0, n } (2)
//    - Serverdown tickle/off   format: { 1, 'D', on,th,tl,  st,sp,ep } (*)
//    - PlayLoop                format: { 1, 'p', on,sp,ep,c,    0, 0 } (2)
//    - Playstate readback      format: { 1, 'S', 0,0,0,       0,0, 0 } (2)
//    - Set color pattern line  format: { 1, 'P', r,g,b,     th,tl, p }
//    - Save color patterns     format: { 1, 'W', 0,0,0,       0,0, 0 } (2)
//    - read color pattern line format: { 1, 'R', 0,0,0,       0,0, p }
///// - Set ledn                format: { 1, 'l', n,0,0,       0,0, 0 } (2+)
//    - Read EEPROM location    format: { 1, 'e', ad,0,0,      0,0, 0 } (1)
//    - Write EEPROM location   format: { 1, 'E', ad,v,0,      0,0, 0 } (1)
//    - Get version             format: { 1, 'v', 0,0,0,       0,0, 0 }
//    - Test command            format: { 1, '!', 0,0,0,       0,0, 0 }
//
//  Fade to RGB color        format: { 1, 'c', r,g,b,      th,tl, ledn }
//  Set RGB color now        format: { 1, 'n', r,g,b,        0,0, ledn }
//  Play/Pause, with pos     format: { 1, 'p', {1/0},pos,0,  0,0,    0 }
//  Play/Pause, with pos     format: { 1, 'p', {1/0},pos,endpos, 0,0,0 }
//  Write color pattern line format: { 1, 'P', r,g,b,      th,tl,  pos }
//  Read color pattern line  format: { 1, 'R', 0,0,0,        0,0, pos }
//  Server mode tickle       format: { 1, 'D', {1/0},th,tl, {1,0},0, 0 }
//  Get version              format: { 1, 'v', 0,0,0,        0,0, 0 }
//
void handleMessage(const char* msgbuf)
{
    //if( msgbuf[0] != blink1_report_id ) return;
    // pre-load response with request, contains report id
    memcpy( hid_send_buf, msgbuf, 8 );

    uint8_t cmd;
    rgb_t c;  // we're going to need a color for many commands, so pre-parse it
    //rid= msgbuf[0];
    cmd  = msgbuf[1];
    c.r  = msgbuf[2];
    c.g  = msgbuf[3];
    c.b  = msgbuf[4];

    //  Fade to RGB color         format: { 1, 'c', r,g,b,      th,tl, ledn }
    //   where time 't' is a number of 10msec ticks
    //
    if(      cmd == 'c' ) {
        uint16_t dmillis = (msgbuf[5] << 8) | msgbuf[6];
        uint8_t ledn = msgbuf[7];          // which LED to address
        playing = 0;
        rgb_setDest(&c, dmillis, ledn);
    }
    //  Set RGB color now         format: { 1, 'n', r,g,b,        0,0, ledn }
    //
    else if( cmd == 'n' ) {
        uint8_t ledn = msgbuf[7];          // which LED to address
        playing = 0;
        if( ledn > 0 ) {
            playing = 3;                   // FIXME: wtf non-semantic 3
            setLED( c.r, c.g, c.b, ledn ); // FIXME: no fading
            //displayLEDs();
        }
        else {
            rgb_setDest( &c, 0, 0 );
            rgb_setCurr( &c );  // FIXME: no LED arg
        }
    }
    //  Read current color        format: { 1, 'r', 0,0,0,        0,0, 0 }
    //
    else if( cmd == 'r' ) {
        uint8_t ledn = msgbuf[7];          // which LED to address
        if( ledn > 0 ) ledn--;
        hid_send_buf[2] = leds[ledn].r;
        hid_send_buf[3] = leds[ledn].g;
        hid_send_buf[4] = leds[ledn].b;
        hid_send_buf[5] = 0;
        hid_send_buf[6] = 0;
        hid_send_buf[7] = ledn;
    }
    //  Play/Pause, with pos      format: { 1, 'p', {1/0},startpos,endpos,  0,0, 0 }
    //
    else if( cmd == 'p' ) {
        playing   = msgbuf[2];
        playstart = msgbuf[3];
        playend   = msgbuf[4];
        playcount = msgbuf[5];
        if( playend == 0 || playend > patt_max )
            playend = patt_max;
        else playend++;  // so that it's equivalent to patt_max, if you know what i mean
        startPlaying();
    }
    // Play state readback        format: { 1, 'S', 0,0,0, 0,0,0 }
    // resonse format:  {
    else if( cmd == 'S' ) { 
        hid_send_buf[2] = playing;
        hid_send_buf[3] = playstart;
        hid_send_buf[4] = playend;
        hid_send_buf[5] = playcount;
        hid_send_buf[6] = playpos;
        hid_send_buf[7] = 0;
    }
    // Write color pattern line   format: { 1, 'P', r,g,b,      th,tl, pos }
    //
    else if( cmd == 'P' ) {
        // was doing this copy with a cast, but broke it out for clarity
        ptmp.color.r = msgbuf[2];
        ptmp.color.g = msgbuf[3];
        ptmp.color.b = msgbuf[4];
        ptmp.dmillis = ((uint16_t)msgbuf[5] << 8) | msgbuf[6];
        ptmp.ledn    = ledn;
        uint8_t pos  = msgbuf[7];
        if( pos >= patt_max ) pos = 0;  // just in case
        // save pattern line to RAM
        memcpy( &pattern[pos], &ptmp, sizeof(patternline_t) );
        //if( pos == (patt_max-1) ) {  // NOTE: writing last position causes write to flash
            //do_pattern_write = 1;
            //writePatternFlash();
        //}
    }
    //  Read color pattern line   format: { 1, 'R', 0,0,0,        0,0, pos }
    //
    else if( cmd == 'R' ) {
        uint8_t pos = msgbuf[7];
        if( pos >= patt_max ) pos = 0;
        patternline_t patt = pattern[pos];
        hid_send_buf[2] = patt.color.r;
        hid_send_buf[3] = patt.color.g;
        hid_send_buf[4] = patt.color.b;
        hid_send_buf[5] = (patt.dmillis >> 8);
        hid_send_buf[6] = (patt.dmillis & 0xff);
        hid_send_buf[7] = patt.ledn;
    }
    // Write color pattern to flash memory: { 1, 'W', 0x55,0xAA, 0xCA,0xFE, 0,0}
    //
    else if( cmd == 'W' ) {
        // FIXME: why doesn't this extra check work?
        // because it was off by One?
        if( msgbuf[2] == 0xBE &&
            msgbuf[3] == 0xEF &&
            msgbuf[4] == 0xCA &&
            msgbuf[5] == 0xFE ) {
            do_pattern_write = 1;
        }
    }
    // Set ledn : { 1, 'l', n, 0...}
    //
    else if( cmd == 'l' ) { 
        ledn = msgbuf[2];
    }

    // read eeprom byte           format: { 1, 'e', addr, 0,0, 0,0,0,0}
    //
    // ...
    // not impelemented in mk2
    // ...

    //  Server mode tickle        format: { 1, 'D', {1/0},th,tl, {1,0}, sp, ep }
    //
    else if( cmd == 'D' ) {
        uint8_t serverdown_on = msgbuf[2];
        uint32_t t = ((uint16_t)msgbuf[3] << 8) | msgbuf[4];
        uint8_t st = msgbuf[5];
        playstart_serverdown  = msgbuf[6];
        playend_serverdown    = msgbuf[7];
        playend_serverdown++;  // to make 'p' play command
        if( playend_serverdown == 0 || playend_serverdown > patt_max )
            playend_serverdown = patt_max;

        if( serverdown_on ) {
            serverdown_millis = t;
            serverdown_update_next = millis() + (t*10);
        } else {
            serverdown_millis = 0; // turn off serverdown mode
        }
        if( st == 0 ) {   // agreed, confusing
            off();
        }
    }
    //  Get version               format: { 1, 'v', 0,0,0,        0,0, 0 }
    //
    else if( cmd == 'v' ) {
        hid_send_buf[3] = blink1_ver_major;
        hid_send_buf[4] = blink1_ver_minor;
    }
    else if( cmd == '!' ) { // testing testing
        //uint16_t w1 = config_read(0x8007); // config word 1
        //uint16_t w2 = config_read(0x8008); // config word 2 (must & 0x3fff )
        uint16_t w1 = config_read(0x8005); // rev id
        uint16_t w2 = config_read(0x8006); // dev id
        hid_send_buf[1] = 0x55;
        hid_send_buf[2] = 0xAA;
        hid_send_buf[3] = usbHasBeenSetup;
        hid_send_buf[4] = w1 >> 8;
        hid_send_buf[5] = w1 & 0xff;
        hid_send_buf[6] = w2 >> 8;
        hid_send_buf[7] = w2 & 0xff;
    }
    else {

    }
}

/*
void checkUSB(void)
{

    //Check if any data was sent from the PC to the keyboard device.  Report descriptor allows
    //host to send 1 byte of data.  Bits 0-4 are LED states, bits 5-7 are unused pad bits.
    //The host can potentially send this OUT report data through the HID OUT endpoint (EP1 OUT),
    //or, alternatively, the host may try to send LED state information by sending a
    //SET_REPORT control transfer on EP0.  See the USBHIDCBSetReportHandler() function.
    if(!HIDRxHandleBusy(lastOUTTransmission)) {

        lastOUTTransmission = HIDRxPacket(HID_EP,(BYTE*)&hid_report_out, 8);
    }
}
*/

// ----------------------------------------------------------------------------
// main
//
int main(void)
{
    InitializeSystem();

#if defined(USB_INTERRUPT)
    USBDeviceAttach();
#endif

    // load pattern from flash to RAM
    memset( pattern, 0, sizeof(patternline_t)*patt_max);
    memcpy( pattern, patternflash, sizeof(patternline_t)*patt_maxflash);

    ws2811_init();

    for( uint8_t i=255; i>0; i-- ) {
        _delay_ms(1);
        uint8_t j = i>>4;      // not so bright, please
        setLEDsAll(j,j,j);
        displayLEDs();
    }

    startPlaying();  // to load pattern up
    off();
    displayLEDs();
    
    while (1) {
        //checkUSB();
        updateLEDs();
        updateMisc();
    }

} //end main


//
static void InitializeSystem(void)
{
    // set IO pin state
    ANSELA = 0x00;
    ANSELC = 0x00;
    TRISA = 0x00;
    TRISC = 0x00;

    // setup oscillator
    OSCTUNE = 0;
    OSCCON = 0xFC;          //16MHz HFINTOSC with 3x PLL enabled (48MHz operation)
    ACTCON = 0x90;          //Enable active clock tuning with USB

    // setup timer2 for tick functionality
    T2CONbits.T2CKPS = 0b01;    // 1:4 prescaler
    T2CONbits.T2OUTPS = 0b1011; //1:12 Postscaler
    PR2 = 242;  // at delay(10), PR2=250 => 1.15msec (w/overhead), PR2=245 => 1.083, PR2=240 => 1.060
    T2CONbits.TMR2ON = 1;       // bit 2 turn timer2 on;
    PIE1bits.TMR2IE  = 1;       // enable Timer2 interrupts
    INTCONbits.PEIE = 1;        // bit6 Peripheral Interrupt Enable bit...1 = Enables all unmasked peripheral interrupts

    loadSerialNumber();

    ei(); // enable global interrupts

    USBDeviceInit(); //usb_device.c.  Initializes USB module SFRs and firmware
    //variables to known states.
}//end InitializeSystem



// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA* each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

// Note *: The "usb_20.pdf" specs indicate 500uA or 2.5mA, depending upon device classification. However,
// the USB-IF has officially issued an ECN (engineering change notice) changing this to 2.5mA for all 
// devices.  Make sure to re-download the latest specifications to get all of the newest ECNs.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Call back that is invoked when a USB suspend is detected
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
    //Example power saving code.  Insert appropriate code here for the desired
    //application behavior.  If the microcontroller will be put to sleep, a
    //process similar to that shown below may be used:

    //ConfigureIOPinsForLowPower();
    //SaveStateOfAllInterruptEnableBits();
    //DisableAllInterruptEnableBits();
    //EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
    //Sleep();
    //RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
    //RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

    //IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
    //cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
    //things to not work as intended.

}

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
    // If clock switching or other power savings measures were taken when
    // executing the USBCBSuspend() function, now would be a good time to
    // switch back to normal full power run mode conditions.  The host allows
    // 10+ milliseconds of wakeup time, after which the device must be
    // fully back to normal, and capable of receiving and processing USB
    // packets.  In order to do this, the USB module must receive proper
    // clocking (IE: 48MHz clock must be available to SIE for full speed USB
    // operation).
    // Make sure the selected oscillator settings are consistent with USB
    // operation before returning from this function.
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

    // Typically, user firmware does not need to do anything special
    // if a USB error occurs.  For example, if the host sends an OUT
    // packet to your device, but the packet gets corrupted (ex:
    // because of a bad connection, or the user unplugs the
    // USB cable during the transmission) this will typically set
    // one or more USB error interrupt flags.  Nothing specific
    // needs to be done however, since the SIE will automatically
    // send a "NAK" packet to the host.  In response to this, the
    // host will normally retry to send the packet again, and no
    // data loss occurs.  The system will typically recover
    // automatically, without the need for application firmware
    // intervention.

    // Nevertheless, this callback function is provided, such as
    // for debugging purposes.
}

/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}//end

/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end

/*******************************************************************
 * Function:        void USBCBInitEP(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
    //USBEnableEndpoint(HID_EP, USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    //USBOutHandle = HIDRxPacket(HID_EP, (BYTE*) & ReceivedDataBuffer, USB_EP0_BUFF_SIZE);
    usbHasBeenSetup++;
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;

    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE) 
        {
            USBMaskInterrupts();

            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0;
            USBBusIsSuspended = FALSE;  //So we don't execute this code again,
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;
            do {
                delay_count--;
            } while (delay_count);

            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1; // Start RESUME signaling
            delay_count = 1800U; // Set RESUME line for 1-13 ms
            do {
                delay_count--;
            } while (delay_count);
            USBResumeControl = 0; //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 * PreCondition:    None
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 * Output:          None
 * Side Effects:    None
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch (event) {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED:
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was 
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was 
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //      endpoints).
            break;
        default:
            break;
    }
    return TRUE;
}


//Secondary callback function that gets called when the below
//control transfer completes for the USBHIDCBSetReportHandler()
void USBHIDCBSetReportComplete(void)
{
    handleMessage((uint8_t*)&CtrlTrfData);
    //memcpy( msgbuf, &CtrlTrfData, sizeof(msgbuf));
    //handleMessage();
}

/********************************************************************
 * Function:        void USBHIDCBSetReportHandler(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        USBHIDCBSetReportHandler() is used to respond to
 *					the HID device class specific SET_REPORT control
 *					transfer request (starts with SETUP packet on EP0 OUT).
 *
 * Note:            This function is called from the USB stack in
 *                  response to a class specific control transfer requests
 *                  arriving over EP0.  Therefore, this function executes in the
 *                  same context as the USBDeviceTasks() function executes (which
 *                  may be an interrupt handler, or a main loop context function,
 *                  depending upon usb_config.h options).
 *******************************************************************/
//void USBHIDCBSetReportHandler(void)
void UserSetReportHandler(void)
{
	//Prepare to receive the command data through a SET_REPORT
	//control transfer on endpoint 0. 
	//USBEP0Receive((BYTE*)&CtrlTrfData, USB_EP0_BUFF_SIZE, USBHIDCBSetReportComplete);
    USBEP0Receive((BYTE*)CtrlTrfData,SetupPkt.wLength, USBHIDCBSetReportComplete);
}


/********************************************************************
 * Function:        void UserGetReportHandler(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    If either the USBEP0SendRAMPtr() or USBEP0Transmit()
 *                  functions are not called in this function then the
 *                  requesting GET_REPORT will be STALLed
 *
 * Overview:        This function is called by the HID function driver
 *                  in response to a GET_REPORT command.
 *
 * Note:            This function is called from the USB stack in
 *                  response to a class specific control transfer request
 *                  arriving over EP0.  Therefore, this function executes in the
 *                  same context as the USBDeviceTasks() function executes (which
 *                  may be an interrupt handler, or a main loop context function,
 *                  depending upon usb_config.h options).
 *                  If the firmware needs more time to process the request,
 *                  here would be a good place to use the USBDeferStatusStage()
 *                  USB stack API function.
 *******************************************************************/
void UserGetReportHandler(void)
{
    USBEP0SendRAMPtr((BYTE*) & hid_send_buf, USB_EP0_BUFF_SIZE, USB_EP0_NO_OPTIONS);
}


/** EOF main.c *************************************************/
#endif
