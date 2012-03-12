
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>
#include <string.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */

// this is for normal BlinkM, using software PWM
#define PIN_RED  PB3
#define PIN_GRN  PB4
#define PIN_BLU  PB1

#define PIN_SDA  PB0
#define PIN_SCL  PB2

// this is the new world order
#define PIN_PWM0		PB4
#define PIN_PWM1		PB1



int main(void)
{
    //wdt_enable(WDTO_1S);
    wdt_disable();
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */

    DDRB  |= 1<<PIN_PWM0; //
    DDRB  |= 1<<PIN_PWM1; 

    for( int i=0; i<5; i++ ) {
        PORTB |=  _BV( PIN_PWM1 );
        _delay_ms(80);
        PORTB &=~ _BV( PIN_PWM1 );
        _delay_ms(80);
    }
    for( int i=0; i<5; i++ ) {
        PORTB |=  _BV( PIN_PWM0 );
        _delay_ms(80);
        PORTB &=~ _BV( PIN_PWM0 );
        _delay_ms(80);
    }

	TCNT1 = OCR1A = OCR1B = OCR1C = 0;
	TCCR1 = 0<<CTC1 | 0<<PWM1A | 0<<COM1A1 | 0<<COM1A0 | 0<<CS13 | 1<<CS12 | 1<<CS11 | 0<<CS10;
	GTCCR = 0<<TSM  | 0<<PWM1B | 0<<COM1B1 | 0<<COM1B0 | 0<<FOC1B | 0<<FOC1A | 0<<PSR1 | 0<<PSR0;
	OCR1C = 249;		// for 1 kHz


    GTCCR |= 1<<PWM1B | 1<<COM1B1;
    TCCR1 |= 1<<PWM1A | 1<<COM1A1;

    OCR1A = 0x80; // PWM1 == PB1 == pin6
    OCR1B = 0x80; // PWM0 == PB4 == pin3
    
    uint8_t val = 0;

    for(;;) {                // main event loop 
        //wdt_reset();

        val++;

        OCR1A = val;     // PWM1 == PB1 = pin6
        OCR1B = val*2;   // PWM0 == PB4 = pin3
        
        _delay_ms(4);
    }

    return 0;
}

 

/* ------------------------------------------------------------------------- */
