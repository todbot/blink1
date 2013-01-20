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
Blink1 myBlink1;

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

  //make a blink1, see whose there
  myBlink1 = new Blink1();
  int b1check = myBlink1.open();
  if (b1check == 0) {
    printBlink1List();
    blink1Count = myBlink1.getCount();
    if (blink1Count < 2) {
      println("too few! too few!");
    } 
    else if (blink1Count > 2) {
      println("goodness, what a party!!");
    } 
    else {
      println("just the right number");
    }
    blink1FoundFlag = true;

    //this is the function that turns off the Blink1s on Quit
    prepareExitHandler();
  }
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
      sendColor2Blink1ByID(c, myID);
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
      myBlink1.openById(0);
      myBlink1.setRGB(0, 0, 0);
      myBlink1.openById(1);
      myBlink1.setRGB(0, 0, 0);
      myBlink1.close();
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
  int howMany = myBlink1.getCount();
  println( "There are " + howMany + " Blink(1)s detected");
  String paths[] = myBlink1.getDevicePaths();
  String serials[] = myBlink1.getDeviceSerials();
  pathTest = paths[0];
  serialTest = serials[0];
  println( "ID" + ": "+ "serials" + " : " + "paths");
  for ( int i=0; i<paths.length; i++ ) { 
    println( i + ": "+ serials[i] + " : " + paths[i]);
  }
  int myFirmware = myBlink1.getFirmwareVersion();
  println("A Firmware Number: " + myFirmware);
}

//------------------------------------------------------------ sendColor2Blink1()
//UNPACKS A COLOR OBJECT AND SENDS IT TO BLINK1
void sendColor2Blink1(color y) {
  myBlink1.open();
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
}

//-------------------------------------------------------- sendColor2Blink1ByID()
//UNPACKS A COLOR OBJECT AND SENDS IT TO BLINK1
void sendColor2Blink1ByID(color y, int i) {
  myBlink1.openById(i);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  myBlink1.close();
}

//---------------------------------------------------- sendColor2Blink1BySerial()
void sendColor2Blink1BySerial(color y, String mySerial) {
  println(mySerial);
  myBlink1.openBySerial(mySerial);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
}

//------------------------------------------------------ sendColor2Blink1ByPath()
void sendColor2Blink1ByPath(color y, String myPath) {
  myBlink1.openByPath(myPath);
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
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

