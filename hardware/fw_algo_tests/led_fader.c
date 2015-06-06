
#include "led_fader.h"

#include <stdio.h> // debug

const uint8_t led_update_millis = 1;
//const uint8_t led_update_millis = 10;

extern int NUM_LEDS;
extern rgb_t leds[];
extern fader_t faders[];


//
inline uint8_t scale8( uint8_t i, fract8 scale)
{
    return ((uint16_t)i * (uint16_t)(scale) ) >> 8;
}

// return true when at top of fader
bool blend( rgb_t* curr, rgb_t* start, rgb_t* dest, fract8 blend_amount )
{
    if( blend_amount == 0) {
        curr->r = start->r;
        curr->g = start->g;
        curr->b = start->b;
    }
    else if( blend_amount == 255) {
        curr->r = dest->r;
        curr->g = dest->g;
        curr->b = dest->b;
        return true;
    }
    else {
        fract8 keep_amount = 256 - blend_amount;
        
        curr->r = scale8( curr->r,   keep_amount) +
                  scale8( dest->r,   blend_amount);

        curr->g = scale8( curr->g,   keep_amount) +
                  scale8( dest->g,   blend_amount);

        curr->b = scale8( curr->b,   keep_amount) +
                  scale8( dest->b,   blend_amount);
    }
    return false;
}

// given a duration,
// compute how much a fader from 0-65535 will travel per msec tick
// to complete range in fade_millis
// compute the fractional amount the fader will move for each (1 or 10) msec tick
//
inline uint16_t compute_blendfract( uint16_t fade_millis )
{
    uint16_t bf = ((uint16_t)65535 * led_update_millis ) / fade_millis;
    printf("compute: fm:%d  bf:%d\n", fade_millis, bf);
    return bf;
}

//
void led_set_dest( rgb_t* new, int dmillis, uint8_t ledn )
{
    uint8_t st = ledn;
    uint8_t end = ledn+1;
    // ledn==0 means all leds
    if( ledn==0 ) {  st = 0; end = NUM_LEDS; }

    for( uint8_t i = st; i< end; i++ ) {
        rgb_t* cur = &leds[i];
        fader_t* f = &faders[i];
        // reset fader position & inc amount
        f->faderpos = 0;
        f->blendfract = compute_blendfract( dmillis );
        // make current color the new start color
        f->last.r = cur->r;
        f->last.g = cur->g;
        f->last.b = cur->b;
        // make new color the new destination
        f->dest.r = new->r;
        f->dest.g = new->g;
        f->dest.b = new->b;
    }
}

//
void led_update_faders()
{
    for( int i=0; i< NUM_LEDS; i++ ) {
        rgb_t* cur   = &leds[i];
        rgb_t* start = &(faders[i].last);
        rgb_t* end   = &(faders[i].dest);
        uint8_t faderpos = faders[i].faderpos/256;
        //faderpos = faderpos/256;
        
        bool donefading = blend( cur, start, end, faderpos );  // do next increment of fade
        if( !donefading ) 
            faders[i].faderpos += faders[i].blendfract;
            // faders[i].faderpos = ((uint16_t)faderpos * 256 + faders[i].blendfract) / 256;
        else {
            //printf(" done fading!\n");
            cur->r = end->r;
            cur->g = end->g;
            cur->b = end->b;
        }
    }
}
