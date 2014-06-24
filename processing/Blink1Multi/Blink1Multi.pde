/*
 * Blink1Multi -- Show off how to use multiple blink(1)s
 *
 * 2014, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 *
 */

import thingm.blink1.*;
import java.util.Random;

Blink1 blink1;
Random rand;

int num_blink1s;

int blink1i = 0;
int r,g,b;
color c;

int fadeTime = 1000;

//
void setup()
{
  size(300,300);
  frameRate(3);

  Blink1.enumerate(); 
  num_blink1s = Blink1.getCount();

 if( num_blink1s == 0 ) {
    println("uh oh, no Blink1 device found");
    num_blink1s = 1; // hack so div-by-zero doesn't bite
  }
  else { 
    println("found "+ num_blink1s + " blink1 devices");
  }
  rand = new Random();
}

//
void draw()
{
  if( blink1i==0 ) { // time to get a new color 
    r = rand.nextInt() & 0xFF;
    g = rand.nextInt() & 0xFF;
    b = rand.nextInt() & 0xFF;
    c = color(r, g, b);
  }

  println("blink1:"+blink1i+" color:"+hex(c));

  // draw color on screen
  background(0, 0);
  fill(c);
  rect(10, 10, width-20, height-20);

  // open up a blink1, send it a color, close it 
  blink1 = Blink1.openById( blink1i );
  blink1.fadeToRGB( fadeTime, r, g, b );
  blink1.close();

  // go to next blink1 (or loop)
  blink1i++;
  blink1i %= num_blink1s;
}

