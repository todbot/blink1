//
// This code creates a random color on the screen and displays it on
// on a blink(1). It updates on initiation and mouse-release. Only works
// well when one blink1 is plugged in. 
// 
// Processing 2.0 Compatible
//
// Created December 10 2012
// by ThingM
//

//import blink1 library
import thingm.blink1.*;

//name your blink1
Blink1 myBlink1;

//global that knows whether or not blink1 is plugged it after
//a check is run
boolean blink1PluggedInFlag = false;

//
void setup()
{
  frameRate(1);
  //intantiate your blink1
  myBlink1 = new Blink1();

  //is it there - see function, also runs on mouse release
  checkForBlink1();

  //set background dark by default
  background(0, 0);
}

//
void draw()
{

  //if a blink1 is plugged in as determined by out checkForBlink1() function
  //then generate a random color and push it out to the blink1
  if (blink1PluggedInFlag) {
    //generate the colors
    //random creates a float, it needs to be cast as an int. 
    int r = int(random(255));
    int g = int(random(255));
    int b = int(random(255));
    //set a color property
    color c = color(r, g, b);
    //set the fill
    fill(c);
    //draw the rectangle
    rect(0, 0, width, height);
    //update the blink1 .setRGB seems pretty obvious. 
    myBlink1.setRGB( r, g, b );
  } 
  else {
    //otherwise just draw a black rectangle
    fill(0);
    rect(0, 0, width, height);
  }
}

//on mouse release, look to see if the blink one is still there.
void mouseReleased() {
  checkForBlink1();
}

void checkForBlink1() {
  //the .open command looks to see if what blink one is available and 
  //prepares it to recieve commands. Only one blink1 can be open at a time
  //anything but 0 is an error code and therefore a fail
  int b = myBlink1.open();
  if ( b != 0 ) { 
    println("uh oh, no Blink1 device found");
    blink1PluggedInFlag = false;
  } 
  else {
    blink1PluggedInFlag = true;
    println("Blink1 device found");
  }
}
