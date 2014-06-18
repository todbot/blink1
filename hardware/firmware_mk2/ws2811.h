 /* 
 * File:   ws2811.h
 * Author: tod
 *
 * ws2812 timings from datasheet
 * TH+TL = 1.25us +/-600ns
 * T0H  = 0.35 us = 350 ns +/-150ns 
 * T1H  = 0.70 us = 700 ns +/-150ns
 * T0L  = 0.80 us = 800 ns +/-150ns  => T0H+T0L = 350 + 800 = 1150ns
 * T1L  = 0.60 us = 600 ns +/-150ns  => T1H+T1L = 700 + 600 = 1300ns
 * TRES = >=50us
 *
 * ws2811 timings
 * T0H  =  250 ns +/-150ns
 * T1H  =  600 ns +/-150ns
 * T0L  = 1000 ns +/-150ns
 * T1L  =  650 ns +/-150ns
 * TRES = >=50us
 *
 *
 */

#ifndef WS2811_H
#define	WS2811_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <xc.h>

// fix stupid IDE warnings on __delay_us
#ifndef _delay_us(x)
#define _delay_us(x) __delay_us(x)
#endif

// prototypes
void ws2811_init(void);
void ws2811_showRGB(void);

void ws2811_init(void)
{
    TRISC2 = 0;  // make output
}

// The pin to write LED data out on
// FIXME: how to map this to  "LATCbits.LATC2"
#ifndef WSDIN
//#define WSDIN LATC2
#define WSDIN LATC,2
#endif

// NOTE: must set compiler to "require address qualifiers" to make sure
// these variables are put in the same bank as the port pins
bank0 volatile unsigned char wscurbyte;
bank0 volatile unsigned char wsbitCounter;

// with this code, timing is:
// T0H = 340ns, T0L = 910ns, T0H+T0L = 1250ns  (w/o NOP befor nextbit: 1170ns)
// T1H = 670ns, T1L = 580ns, T1H+T1L = 1250ns  (w/o NOP after nextbit: 1170ns)
// with a 2.7usec gap between bytes
void ws2812_sendarray_tod1(unsigned char *data, unsigned int datlen)
{
    di();
    while (datlen--)
    {
        wscurbyte = *data++;
        wsbitCounter = 8; // this must come right before asm so bank0 is selected
#asm
wsstart:
        //BANKSEL _wscurbyte
        BSF WSDIN       // set pin high
        RLF BANKMASK(_wscurbyte)  // rotate left into carry
        BTFSC STATUS,0  // skip if carry bit set aka 'BC wsone'
        GOTO wsone
        BCF WSDIN       // set pin low for zero
        NOP
        NOP
        NOP
        GOTO wsnextbit
wsone:
        NOP
        NOP
        NOP
        BCF WSDIN       // set pin low for one
        NOP
wsnextbit:
        NOP
        MOVLW 0
        // BANKSEL _wsbitCounter
        DECFSZ BANKMASK(_wsbitCounter)
        GOTO wsstart
#endasm
    }
    ei();
}

#define ws2811_showRGB()  \
{ \
    ws2812_sendarray_tod1( (uint8_t *)leds, nLEDs*3);   \
    __delay_us(50);  \
}



#ifdef	__cplusplus
}
#endif

#endif	/* WS2811_H */

