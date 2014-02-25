/*
 * Blink1Test0 -- Simplest test of the blink(1) Processing library 
 *
 * 2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 *
 */

import thingm.blink1.*;
import java.util.Random;

Blink1 blink1;
Random rand;

//
void setup()
{
  frameRate(1);

  blink1 = Blink1.open();

  if( blink1.error() ) { 
      println("uh oh, no Blink1 device found");
  }

  rand = new Random();
}

//
void draw()
{
  background(0, 0);

  int r = rand.nextInt() & 0xFF;
  int g = rand.nextInt() & 0xFF;
  int b = rand.nextInt() & 0xFF;

  color c = color(r, g, b);
  fill(c);
  rect(0, 0, width, height);

  blink1.setRGB( r, g, b );
  
}

