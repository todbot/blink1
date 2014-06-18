//
// color_funcs.h -- color sliding
//
// 2012, Tod E. Kurt, http://todbot.com/blog/
//
//
// also see:
//   http://meyerweb.com/eric/tools/color-blend/ 
//
// current memory usage for one LED is:
// 6 bytes - curr100x
// 6 bytes - dest100x
// 6 bytes - step100x
// 1 byte  - stepcnt
// === 19 bytes
// => 8 LEDs = 8*19 = 152 bytes
//
//

#ifndef RGB_FUNCS_H
#define RGB_FUNCS_H

#include <stdint.h>


// RGB triplet unsigned ints for internal use of 100x scale
// used instead of floating point
typedef struct {
    int g;
    int r;
    int b;
} rgbint_t;

//
typedef struct {
    rgb_t color;
    uint16_t dmillis; // hundreths of a sec
} patternlineold_t;

typedef struct {
    rgb_t color;
    uint16_t dmillis; // hundreths of a sec
    uint8_t ledn;     // number of led, or 0 for all
} patternline_t;

// note: this is for doing ROM size address calcs that can't use sizeof()
#define patternline_size (3 + 2 + 1)

// what is this for exactly? only used in off()?
#define setRGBt(rgbt,x,y,z) { rgbt.r=x; rgbt.g=y; rgbt.b=z; }

typedef struct {
    rgbint_t dest100x;  // the eventual destination color we want to hit
    rgbint_t step100x;  // the amount of to move each tick
    rgbint_t curr100x;  // the current color, times 10 (to lessen int trunc issue)
    int stepcnt;
} rgbfader_t;

rgbfader_t fader[nLEDs];

//#ifndef setLEDs
//#error "setLEDs(r,g,b,n) not defined"
//#endif


// set the current color OF ALL LEDs
void rgb_setCurr( rgb_t* newcolor )
{
    for( uint8_t i=0; i<nLEDs; i++ ) { 
        rgbfader_t* f = &fader[i];
        //rgbfader_t f = fader[i];
        f->curr100x.r = newcolor->r * 100;
        f->curr100x.g = newcolor->g * 100;
        f->curr100x.b = newcolor->b * 100;

        f->dest100x.r = f->curr100x.r;
        f->dest100x.g = f->curr100x.g;
        f->dest100x.b = f->curr100x.b;
        f->stepcnt = 0;

        //setRGBOutN( newcolor->r, newcolor->g, newcolor->b, i );
        setLED( newcolor->r, newcolor->g, newcolor->b, i );
    }
    //displayLEDs();
}

void rgb_setDestN( rgb_t* newcolor, int steps, int ledn )
{
    rgbfader_t* f = &fader[ledn];
    f->dest100x.r = newcolor->r * 100;
    f->dest100x.g = newcolor->g * 100;
    f->dest100x.b = newcolor->b * 100;

    f->stepcnt = steps + 1;

    f->step100x.r = (f->dest100x.r - f->curr100x.r) / steps;
    f->step100x.g = (f->dest100x.g - f->curr100x.g) / steps;
    f->step100x.b = (f->dest100x.b - f->curr100x.b) / steps;
}

// set a new destination color
void rgb_setDest( rgb_t* newcolor, int steps, int ledn  )
{
    if (ledn > 0) {
        rgb_setDestN(newcolor, steps, ledn - 1);
    } else {
        for (uint8_t i = 0; i < nLEDs; i++) {
            rgb_setDestN( newcolor, steps, i);
        }
    }
}

// call at every tick
void rgb_updateCurrent(void)
{
    for( uint8_t i=0; i<nLEDs; i++ ) {
        //rgbfader_t f = fader[i];
        rgbfader_t* f = &fader[i];
        if( !f->stepcnt ) {
            continue;
        }
        f->stepcnt--;
        if( f->stepcnt ) {
            f->curr100x.r += f->step100x.r;
            f->curr100x.g += f->step100x.g;
            f->curr100x.b += f->step100x.b;
        } else {
            f->curr100x.r = f->dest100x.r;
            f->curr100x.g = f->dest100x.g;
            f->curr100x.b = f->dest100x.b;
        }
        
        //setRGBOutN( f->curr100x.r/100, f->curr100x.g/100, f->curr100x.b/100, i );
        setLED( f->curr100x.r/100, f->curr100x.g/100, f->curr100x.b/100, i );
    }
    //displayLEDs();
}


#endif


