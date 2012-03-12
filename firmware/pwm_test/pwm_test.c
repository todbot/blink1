
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
// also USBM & USBP
#define PIN_SDA  PB0
#define PIN_SCL  PB2

// this is the new world order
#define PIN_RED	 PB4   // OCR1B
#define PIN_GRN	 PB0   // OCR0A
#define PIN_BLU	 PB1   // OCR0B
// and i guess
#define PIN_USBP PB2
#define PIN_USBM PB3

#define setRed(x) ( OCR1B = (x) )
#define setGrn(x) ( OCR0A = 255 - (x) )
#define setBlu(x) ( OCR0B = 255 - (x) )

#define setRGB(x,y,z) { setRed(x); setGrn(y); setBlu(z); }

int main(void)
{
    //wdt_enable(WDTO_1S);
    wdt_disable();

    // from: 
    // http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=848902

    //configure outputs on PB0, PB1, PB4 
    DDRB = _BV(PB0) | _BV(PB1) | _BV(PB4); 
    //PORTB = _BV(DDB2); //enable pullup on PB2 

    //WGM00 & WGM01 = FastPWM 
    //COM0A1 & COM0B1 = Set OC0A/OC0B on compare match, clear at bottom (inverting mode) 
    TCCR0A = _BV(WGM01)  | _BV(WGM00) |
        _BV(COM0A1) | _BV(COM0A0) | 
        _BV(COM0B1) | _BV(COM0B0);
    
    TCCR0B = _BV(CS01); //Timer0 prescaler /8 

    TCCR1 = _BV(CS12);  //Timer1 prescaler /8 
    //PWM1B = enable PWM on OCR1B 
    //COM1B1 = clear on compare match, set when 
    GTCCR = _BV(PWM1B) | _BV(COM1B1); 
    //GTCCR = _BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0); // why doesn't this work?


    for( int i=0; i< 5; i++ ) { 
        setRed( 127 );
        setGrn( 127 );
        setBlu( 127 );

        _delay_ms(2000);

        setRed( 0 );
        setGrn( 0 );
        setBlu( 0 );
        _delay_ms(2000);
    }

    while( 1 ) { 
        ;;
    }

}

    /*
    // full off
    OCR1B = 255; //red    == PB4 == pin3
    OCR0A = 255; //green  == PB0 == pin5
    OCR0B = 255; //blue   == PB1 == pin6
    _delay_ms(1000);

    // full on
    OCR1B = 0; //red    == PB4 == pin3
    OCR0A = 0; //green  == PB0 == pin5
    OCR0B = 0; //blue   == PB1 == pin6
    _delay_ms(1000);

    */
/*
    OCR1B = 127; //red    == PB4 == pin3
    OCR0A = 127; //green  == PB0 == pin5
    OCR0B = 127; //blue   == PB1 == pin6

    uint8_t val=0;
    while(1) { 

        for( int r=0; r<255; r++ ) { 
            OCR1B = 255 - r;
            _delay_ms(15);
        }
        OCR1B = 255; // off

        for( int g=0; g<255; g++ ) { 
            OCR0A = 255 - g;
            _delay_ms(15);
        }
        OCR0A = 255; // off

        for( int b=0; b<255; b++ ) { 
            OCR0B = 255 - b;
            _delay_ms(15);
        }
        OCR0B = 255; // off

        _delay_ms(300); 
    } 

    return 1;                                                
} 
*/
