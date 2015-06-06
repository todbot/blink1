#ifndef _LED_FADER_H_
#define _LED_FADER_H_

#include <stdint.h>
#include <stdbool.h>

//  fract8   range is 0 to 0.99609375
//                 in steps of 0.00390625
typedef uint8_t   fract8;   // ANSI: unsigned short _Fract

typedef struct {
    uint8_t g;  // order is important: bytes are sent G,R,B to WS2812
    uint8_t r;
    uint8_t b;
} rgb_t;

// current blink1 fader struct size is 3 * (2+2+2) + 2 = 3*6 + 2 = 20 bytes per LED
// below is 3 + 3 + 2 + 2 = 10 bytes per LED
// can maybe shave another byte off that
// blink0.h has (2+1)*3 + 2 = 9+2 = 11 bytes
typedef struct {
    rgb_t last;          // last / starting color
    rgb_t dest;          // destination/ending color
    uint16_t faderpos;  // ranges from 0-65535 
    uint16_t blendfract; // amount of to increment fader_val by each tick
} fader_t;


#define rgb_set( c, ar, ag, ab ) {c.r=ar; c.g=ag; c.b=ab; }

void led_set_dest( rgb_t* new, int dmillis, uint8_t ledn );

void led_update_faders();


uint8_t scale8( uint8_t i, fract8 scale);

bool blend( rgb_t* curr, rgb_t* start, rgb_t* dest, fract8 blend_amount );

uint16_t compute_blendfract( uint16_t dmillis );


#endif
