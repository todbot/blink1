//
// Two Blink(1) Dance Party 
//
// Sends a random color to alternating Blink1s
// 
// Processing 2.0 Compatible
//
// Created December 12 2012
// by Carlyn Maw for ThingM
//

//import blink1 library
import thingm.blink1.*;

//name your blink1
Blink1 myBlink1A;
Blink1 myBlink1B;

//timer variables for how long it takes for the color to change
//the higher the speedvar the slower the change rate
long t = 0;
int framerateVar = 12;
int speedVar = framerateVar;

//The background color set as global so I only have one place
//to remember if I change my mind
int bgVar = 249;

//characteristics of the swatches
int swatchH = 100;
int swatchW = 100;
int swatchOffSet = 10;
//color variable for most recent random color
color c = color(0, 0, 0);
//holder for the previous random color
color pc = color(0, 0, 0);

//the holder for which blink1 I'm talking to
int myID = 0;
//how many blink ones are plugged in?
int blink1Count = 0;
//there is at least 1 blink1 plugged in
boolean blink1FoundFlag = false;

//strings to pull a path and a serial number from the initial
//test for ease of use in testing BySerial and ByPath methods. 
String pathTest = "";
String serialTest = "";


//----------------------------------------------------------------- START SETUP
void setup()
{

  //stage characteristics
  size(300, 150 );
  frameRate(framerateVar);
  //textFont(createFont("helvetica", 11));
  background(bgVar);

  // see who's there
  blink1Count = Blink1.getCount();
  printBlink1List();

  if( blink1Count > 0 ) {
    blink1FoundFlag = true;
  }
  
  if (blink1Count < 2) {
    println("too few! too few!");
  } 
  else if (blink1Count > 2) {
    println("goodness, what a party!!");
  } 
  else {
    println("just the right number");
  }

  // make a blink1,
  myBlink1A = Blink1.openById(0);
  myBlink1B = Blink1.openById(1);

  //this is the function that turns off the Blink1s on Quit
  prepareExitHandler();

}
//------------------------------------------------------------------- END SETUP
//------------------------------------------------------------------ START DRAW
void draw()
{


  //incrementing global for setting clock, ticks up every frame
  //cannot just use millis since millis will not increment smoothly
  t = t + 1;


  if ((t % speedVar) == 0) {
    if (myID == 0) {
      myID = 1;
    } 
    else {
      myID = 0;
    }
    //extra little check in case in the time elapsed blink1 has been unplugged.
    background(bgVar);


    pc = c;
    c = generateRandomColor();

    if (blink1FoundFlag) {
      sendColorToBlink1ById(c,myID);
      //sendColorToBothBlink1s(c);
      //sendColor2Blink1ByID(c, myID);
      //OTHER OPTIONS
      //sendColor2Blink1BySerial(c, serialTest);
      //sendColor2Blink1ByPath(c, pathTest);
      //sendColor2Blink1(c);
    } else {
      println("plug in Blink1s and restart");
    }
  }


  //draw the swatches
  rectMode(CENTER);
  noStroke();
  if (myID == 0) {
    fill(c);
    rect(width/4, (height/2)-swatchOffSet, swatchW, swatchH);
    fill(pc);
    rect(3*width/4, (height/2)-swatchOffSet, swatchW, swatchH);
  } 
  else {
    fill(c);
    rect(3*width/4, (height/2)-swatchOffSet, swatchW, swatchH);
    fill(pc);
    rect(width/4, (height/2)-swatchOffSet, swatchW, swatchH);
  }
}
//-------------------------------------------------------------------- END DRAW

//---------------------------------------------------------- START EXIT HANDLER
private void prepareExitHandler () {
  Runtime.getRuntime().addShutdownHook(new Thread(new Runnable() {
    public void run () {
      //System.out.println("SHUTDOWN HOOK");
      //What to do on closing here
      //TURN OFF & RELEASE myBlink1
      myBlink1A.setRGB(0, 0, 0);
      myBlink1B.setRGB(0, 0, 0);
      myBlink1A.close();
      myBlink1B.close();
    }
  }
  ));
}
//------------------------------------------------------------- END EXIT HANDLER


//------------------------------------------------------------------------------
//------------------------------------------------------- START CUSTOM FUNCTIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------ printBlink1List()
//TELL ME WHO'S HERE 
void printBlink1List() {
  int howMany = Blink1.getCount();
  println( "There are " + howMany + " Blink(1)s detected");
  if( howMany > 0 ) { 
    String paths[] = Blink1.getDevicePaths();
    String serials[] = Blink1.getDeviceSerials();
    pathTest = paths[0];
    serialTest = serials[0];
    println( "ID" + ": "+ "serials" + " : " + "paths");
    for ( int i=0; i<paths.length; i++ ) { 
      println( i + ": "+ serials[i] + " : " + paths[i]);
    }
    
    //int myFirmware = myBlink1.getFirmwareVersion();
    //println("A Firmware Number: " + myFirmware);
  }
}

void sendColorToBothBlink1s(color y) {
  myBlink1A.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  myBlink1B.setRGB(int(red(y)), int(green(y)), int(blue(y)));
}

//------------------------------------------------------------ sendColor2Blink1()
//UNPACKS A COLOR OBJECT AND SENDS IT TO BLINK1
void sendColor2Blink1(color y) {
  //myBlink1.open();
  myBlink1A.setRGB(int(red(y)), int(green(y)), int(blue(y)));
}

//-------------------------------------------------------- sendColor2Blink1ByID()
//UNPACKS A COLOR OBJECT AND SENDS IT TO BLINK1
void sendColorToBlink1ById(color y, int i) {
  Blink1 myBlink1 = Blink1.openById(i);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  myBlink1.close();
}

//---------------------------------------------------- sendColor2Blink1BySerial()
void sendColorToBlink1BySerial(color y, String mySerial) {
  println(mySerial);
  Blink1 myBlink1 = Blink1.openBySerial(mySerial);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  myBlink1.close();
}

//------------------------------------------------------ sendColor2Blink1ByPath()
void sendColorToBlink1ByPath(color y, String myPath) {
  Blink1 myBlink1 = Blink1.openByPath(myPath);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  myBlink1.close();
}

//--------------------------------------------------------- generateRandomColor()
color generateRandomColor() {      //generate the colors
  //random creates a float, it needs to be cast as an int. 
  int r = int(random(255));
  int g = int(random(255));     
  int b = int(random(255));

  //set a color property
  color rColor = color(r, g, b);
  return rColor;
}

