/*
 * Blink1ColorPatterns - make some pretty patterns with a mk2 with 18 LEDs.
 *
 * 2013, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 *
 */

import thingm.blink1.*;
import java.util.Random;
import java.awt.Color;

// number of LEDs connected to your blink1, 18 is the max
int ledn = 18;

Blink1 blink1;
Random rand;

long nextUpdate;
long runningUpdate;
int runningMillis = 50;

Color[] ledcolors = new Color[ledn];
color cscreen;

//
void setup()
{
  //frameRate(60);
  size(300,300);
  
  blink1 = Blink1.open();
  
  if( blink1.error() ) { 
    println("uh oh, no Blink1 device found");
  }
  
  rand = new Random();
  for( int i=0; i<ledn; i++ ) { 
    ledcolors[i] = new Color( 0,0,0);
  }
}

//
void draw()
{

  cscreen = ledcolors[0].getRGB();
  background(0, 0);
  fill(cscreen);
  rect(0, 0, width, height);
  

  if( (millis() - runningUpdate) > 0 ) { 
    runningUpdate += runningMillis;

    // shift the positions
    Color ctmp = ledcolors[ledcolors.length-1];
    System.arraycopy(ledcolors, 0, ledcolors, 1, ledcolors.length-1);
    ledcolors[ 0 ] = ctmp;
    
    for( int n=0; n < ledn; n++ ) {
      blink1.fadeToRGB( 100, ledcolors[n], n+1 ); // addresing starts at 1
    }
    
    if( (millis() - nextUpdate) > 0 ) {
      nextUpdate += 4*(runningMillis*(ledn));
      
      //int r = 200; 
      //int g = 0; 
      //int b = 100;
      
      int r = rand.nextInt() & 0xFF;
      int g = rand.nextInt() & 0xFF;
      int b = rand.nextInt() & 0xFF;
      
      println("color="+ new Color(r,g,b));
      
      // make gradient
      for( int i=0; i<ledn; i++ ) { 
        ledcolors[i] = new Color( r * i/ledn, g * i/ledn, b * i/ledn );
      }

    }

  }

}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
        runningMillis += 5;
    } else if (keyCode == DOWN) {
        runningMillis -= 5;
    } 
  } else {
      //fillVal = 126;
  }
  if( runningMillis < 5 )  runningMillis = 5;
  if( runningMillis > 500 ) runningMillis = 500;
}

