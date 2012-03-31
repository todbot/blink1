//
//
//
import thingm.blinkm.*;

BlinkMUSB blinkmusb;
Random rand;

//
void setup()
{
  frameRate(1);

  blinkmusb = new BlinkMUSB();

  int rc = blinkmusb.open();

  if( rc != 0 ) { 
      println("uh oh, no BlinkMUSB device found");
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

  blinkmusb.setRGB( r, g, b );
  
}

