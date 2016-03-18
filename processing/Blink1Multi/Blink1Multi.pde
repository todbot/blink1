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
int blink1inc = 1;

color c = color(255,0,0);
int ci;

int fadeTime;

int nextMillis;
int mode;

//
void setup()
{
  size(300,300);
  frameRate(15);

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
void randDisco()
{
  if( (long)(millis() - nextMillis) < 0 ) { 
    return;
  }
  nextMillis = millis() + 100;
  fadeTime = 300;

  colorMode(HSB,255);
  c = color( random(255), 255,255);

  // draw color on screen
  background(0, 0);
  fill(c);
  rect(10, 10, width-20, height-20);

  blink1i = int(random(num_blink1s));
  // open up a blink1, send it a color, close it 
  blink1 = Blink1.openById( blink1i );
  blink1.fadeToRGB( fadeTime, int(red(c)), int(green(c)), int(blue(c)) );
  blink1.close();

}

//
void randColorFades()
{
  if( (long)(millis() - nextMillis) < 0 ) { 
    return;
  }
  nextMillis = millis() + 1000;

  fadeTime = 2000;
  colorMode(RGB,255);
  if( blink1i==0 ) { // time to get a new color 
    int r = rand.nextInt() & 0xFF;
    int g = rand.nextInt() & 0xFF;
    int b = rand.nextInt() & 0xFF;
    c = color(r, g, b);
  }

  println("blink1:"+blink1i+" color:"+hex(c));

  // draw color on screen
  background(0, 0);
  fill(c);
  rect(10, 10, width-20, height-20);

  // open up a blink1, send it a color, close it 
  blink1 = Blink1.openById( blink1i );
  blink1.fadeToRGB( fadeTime, int(red(c)), int(green(c)), int(blue(c)) );
  blink1.close();

  // go to next blink1 (or loop)
  blink1i++;
  blink1i %= num_blink1s;

}

//
void cylonFades()
{
  colorMode(HSB,255);

  if( (long)(millis() - nextMillis) < 0 ) { 
    return;
  }
  nextMillis = millis() + 300;
    
  // draw color on screen
  background(0, 0);
  fill(c);
  rect(10, 10, width-20, height-20);
  
  fadeTime = 800;

  // open up a blink1, send it a color, close it 
  blink1 = Blink1.openById( blink1i );
  blink1.fadeToRGB( fadeTime, 0,0,0 );
  blink1.close();

  // go to next blink1 (or loop)
  blink1i += blink1inc;
  if( blink1i == num_blink1s-1 ) {
    blink1inc=-1;
    nextMillis += 200;
  }
  if( blink1i == 0 ) {
    blink1inc = 1;
    nextMillis += 200;
  }

  c = color( ci, 200,200);
  ci+=1;
  ci %= 255;

  println("blink1i:"+blink1i+" ci:"+ci);

  // open up a blink1, send it a color, close it 
  blink1 = Blink1.openById( blink1i );
  blink1.fadeToRGB( fadeTime, int(red(c)), int(green(c)), int(blue(c)));
  blink1.close();

}

//
void draw()
{
  if( mode == 0 ) { 
    randDisco();
  }
  else if( mode == 1 ) { 
    cylonFades();
  } else if( mode == 2 ) { 
    randColorFades();
  }
  else { 
    mode = 0; // hack
  }
}

void keyPressed()
{
  mode++;
  mode %= 3;  // number of modes
  println("change modes! mode="+mode);
}
