//
// This code creates a random color on the screen and displays it on
// on a blink(1). The user can pause the action with a mouse click. The
// software checks for a blink one startup and mouse-release.
// 
//
// Created December 11 2012
// by Carlyn Maw for ThingM
//

//import blink1 library
import thingm.blink1.*;

//name your blink1
Blink1 myBlink1;

//global that holds the information whether or not blink1 is plugged in
//after checking function is called
boolean blink1PluggedInFlag = false;

//global that knows if random generator is playing or paused
boolean playFlag = true;

//is the mouse in the button
boolean buttonFlag = false;

//timer variables for how long it takes for the color to change
//the higher the speedvar the slower the change rate
long t = 0;
int framerateVar = 12;
int speedVar = framerateVar;

//The background color set as global so I only have one place
//to remember if I change my mind
int bgVar = 249;

//characteristics of the swatch
int swatchH = 100;
int swatchW = 100;
int swatchOffSet = 10;
color c = color(0, 0, 0);


//----------------------------------------------------------------- START SETUP
void setup()
{
  //stage characteristics
  size(150, 150);
  frameRate(framerateVar);
  textFont(createFont("helvetica", 11));
  background(bgVar);

  // intantiate your blink1
  myBlink1 = Blink1.open();

  //is it there - see function, also runs on mouse release
  checkForBlink1();

}
//------------------------------------------------------------------- END SETUP
//------------------------------------------------------------------ START DRAW
void draw()
{

  //incrementing global for setting clock, ticks up every frame
  //cannot just use millis since millis will not increment smoothly
  t = t + 1;
  //every time the speedVar goes evenly into t it is time to change the 
  //color of the blink1 if it playFlag is set to true, otherwise the color
  //is left alone.
  //(playFlag is in by checkForBlink1())
  if (((t % speedVar) == 0) && playFlag) {
    //extra little check in case in the time elapsed blink1 has been unplugged.

    background(bgVar);
    //generate the colors
    //random creates a float, it needs to be cast as an int. 
    int r = int(random(255));
    int g = int(random(255));     
    int b = int(random(255));
    
    //write these colors to the screen
    textAlign(LEFT, CENTER);
    fill(102, 51, 51);
    text(r, width/2-50, (height/2)+swatchH/2);
    textAlign(CENTER, CENTER);
    fill(51, 102, 51);
    text(g, width/2, (height/2)+swatchH/2);
    textAlign(RIGHT, CENTER);
    fill(51, 51, 102);
    text(b, width/2+50, (height/2)+swatchH/2);
    
    //set a color property
    c = color(r, g, b);
    
    //update the blink1 .setRGB seems pretty obvious. 
    myBlink1.setRGB( r, g, b );

    if( myBlink1.error() ) {
      println("error setting color, blink(1) unplugged?");
    }
    
    //draw the swatch
    rectMode(CENTER);
    noStroke();
    rect(width/2, (height/2)-swatchOffSet, swatchW, swatchH);
  }

  //if there is no blink1 plugged in, set the swatch color to black
  //and no text.
  else if (!blink1PluggedInFlag) {
    background(bgVar);
    c = 0;
  }

  //draw the swatch
  fill(c);
  rectMode(CENTER);
  noStroke();
  rect(width/2, (height/2)-swatchOffSet, swatchW, swatchH);

  //are you in the button area?
  buttonHandler();
}
//-------------------------------------------------------------------- END DRAW
//---------------------------------------------------------- START MOUSERELEASE
//on mouse release, look to see if the blink one is still there.
void mouseReleased() {
  if (buttonFlag) {
    checkForBlink1();
  }
}

//------------------------------------------------------------ END MOUSERELEASE

//------------------------------------------------------------------------------
//------------------------------------------------------- START CUSTOM FUNCTIONS
//------------------------------------------------------------------------------

//--------------------------------------------------------- START checkForBlink1
void checkForBlink1() {
  //the .open command looks to see if what blink one is available and 
  //prepares it to recieve commands.
  if( myBlink1.error() ) {
    myBlink1.close();
    myBlink1 = Blink1.open();
  }
  boolean b1error = myBlink1.error();
  
  //println("b1error: "+b1error + ", blink1PluggedInFlag: "+blink1PluggedInFlag );

  if( b1error ) {
    if( blink1PluggedInFlag ) {
      println("uh oh, you took out the blink(1) device");
    }
    else { 
      println("uh oh, no blink(1) device found");
    }
    blink1PluggedInFlag = false;
    playFlag = false;
  }
  else { // else no error
    if( blink1PluggedInFlag == true ) {
      playFlag = !playFlag; // toggle play/pause
    } else {
      println("blink(1) device found");
      playFlag = true;
    }
    blink1PluggedInFlag = true;
  }
  
}

//--------------------------------------------------------- END checkForBlink1

//----------------------------------------------------- START Button Functions
void buttonHandler() {

  if ((mouseX < (width/2 + swatchW/2)) && (mouseX > (width/2 - swatchW/2))) {
    if ((mouseY < (height/2 + swatchH/2-swatchOffSet)) && (mouseY > (height/2 - swatchH/2-swatchOffSet))) {
      buttonFlag = true;
      if (blink1PluggedInFlag) {
        if (playFlag) {
          drawPause();
        } 
        else {
          drawPlay();
        }
      }
    }
  } else {
    buttonFlag = false;
  }
}
//---------------------------------------------------------- END buttonHandler

//------------------------------------------------------------ START drawPause
void drawPause() {
  fill(bgVar, 50);
  rect(width/2-13, (height/2)-swatchOffSet, swatchW/7, swatchH/1.8);
  rect(width/2+13, (height/2)-swatchOffSet, swatchW/7, swatchH/1.8);
}
//-------------------------------------------------------------- END drawPause

//------------------------------------------------------------- START drawPlay
void drawPlay() {
  fill(bgVar, 50);
  float x1 = (width/2-swatchW/4+5);
  float x2 = x1;
  float x3 = (width/2+swatchW/4+5);
  float y3 =  height/2 - swatchOffSet;
  float y2 = y3 + 30;
  float y1 =  y3 - 30;
  triangle(x1, y1, x2, y2, x3, y3);
}
//--------------------------------------------------------------- END drawPlay
