/**
 * Blink 1 Tea Timer 
 * by Carlyn Maw / carlynorama
 *
 * Created December 12, 2012 
 * Updated December 13, 2012
 *
 * Helps you time your tea. Example shows blink1 being updated from an
 * array at increments based on the passage of time. 
 *
 *
 * 
 */

//IMPORTS
import thingm.blink1.*;


//DEBUGGING
//millis/second - to change speed for testing
final int mps = 1000;

//PROCESSING SETTINGS (CONSTANTS)
final float framerateVar = 30; 
final float myHeight = 600;
final float myWidth = 600;

//TEA TIME CONSTANTS
//mintutes * millis/second(variable for debugging) * 60 seconds/minute
final long teaBreakLength = 10 * mps * 60;
final long minSteepTime = 2 * mps * 60;
final long maxSteepTime = 5 * mps * 60;
//how many tea stages (I do 1 per 30 seconds of Min brew time)
final int stageNum = 4; 

//TEA CUP CONSTANTS
//cupSize/30 seconds  100 px
final float cupSize =  3*min(myHeight,myWidth)/4;
final int cupColor = 180;
final int teaAlpha = 255/(stageNum-1);
final color teaColor = color(158, 80, 0, teaAlpha);
//the cup gets bigger at the rate that the first circle reaches the 
//edge at minSteepTime
final float increment = ((cupSize) / (minSteepTime)) ;

//GENERAL ARTBOARD (LAYOUT) CONSTANTS
final int bgColor = 250;
final float artCenterH = myHeight/2-(myHeight*0.075);
final float artCenterW = myWidth/2;
final float cupBottom = artCenterH + cupSize/2;
final float textTop = cupBottom + cupSize*0.03;
final float textMargin = myWidth/2; //- ((myWidth - cupSize)/2);
final float lineHeight = min(myHeight/20, 14);

final float lineSpacing = 1.2;

//TEA TIME VARIABLES
//when the user clicked, i.e. started the tea timer
long clickMillis = 0;
//is the timer running
boolean timerGoFlag = false;
//size biggest tea "puddle" would be if unfettered by cup.
float s = 0;
//what color from the array to use
int cNum = 0; 
//last color sent
int pcNum = 0;
//color array, number is stageNum + 4 
//(timer off, timer past last segment flow, timer past reccomended maximum, timer past tea time)
color[] btColors = new color[8];
//what message from the array to use
int mNum = 0;
//message array, number is stageNum + 4 
//(timer off, timer past last segment, timer past reccomended maximum, timer past tea time)
String[] messages = new String[8];
String timeString = "";


//THE BLINK(1)
Blink1 myBlink1;



//----------------------------------------------------------------- START SETUP

void setup() {
  println(cupSize);
  //SETUP ENVIRONMENT
  size(int(myHeight), int(myWidth));
  frameRate(framerateVar);
  textFont(createFont("helvetica", lineHeight));
  textAlign(CENTER, TOP);
  

  //DRAW BACKGROUND
  drawBackground();

  //INITIATE BLINK 1
  myBlink1 = Blink1.open();
  myBlink1.setRGB(0, 0, 0);

  //COLOR AND MESSAGE ARRAYS
  btColors[7] = color(0, 0, 0);
  btColors[6] = color(130, 10, 5);
  btColors[5] = color(190, 80, 0);
  btColors[4] = color(220, 115, 0);
  btColors[3] = color(245, 140, 0);
  btColors[2] = color(255, 180, 2);
  btColors[1] = color(255, 209, 168);
  btColors[0] = color(0, 0, 0);  

  messages[0] = "";
  messages[1] = "steeping";
  messages[2] = "ready";
  messages[3] = "very ready";
  messages[4] = "very very ready";
  messages[5] = "last call";
  messages[6] = "past due";
  messages[7] = "tea time over.";

  //ON-EXIT CLEAN UP
  prepareExitHandler();
}
//------------------------------------------------------------------- END SETUP


//------------------------------------------------------------------ START DRAW
void draw() {

  background(bgColor);
  fill(teaColor);
  noStroke();

  //is the timer running?
  if (timerGoFlag) {
    //then make the size variable bigger by the increment
    //s = s + increment;
    s = (millis()-clickMillis) * increment;
    //and conver the time elapsed into a string
    timeString = makeTimeDisplayString(clickMillis, millis());
  } 
  else {
    //if the timer isn't running text field should be empty.
    //I could have made this be a conditional display of the
    //text field itself but I wanted to keep the two text fields
    //together and that one has to be later. 
    timeString = "";
  }



  //DRAW THE TEA PUDDLES & DETERMINE WHAT STAGE COLOR & MESSAGE TO USE
  for (int i = 0; i < stageNum; i = i+1) {
    if (s >= i*(cupSize/stageNum)) {
      float teaRingSize = s - (i*cupSize/stageNum);
      if (teaRingSize >= cupSize) {
        //what color and message to use.
        //the first array item is off-status, hence +1
        //and then the last i value that returns true is
        //actually lagging behind the actual stage number, 
        //hence the +2
        cNum = i+2;
        mNum = i+2;
      }
      //snap to cup size
      teaRingSize = min(teaRingSize, cupSize);
      //draw puddle
      ellipse(artCenterW, artCenterH, teaRingSize, teaRingSize);
    }
  }


  //IF PAST MAX STEEP TIME, BUMP COLOR AND MESSAGE TO PAST DUE
  if (millis() > (clickMillis+maxSteepTime)) {
    cNum = stageNum + 2;
    mNum = stageNum + 2;
  }

  //FURTHER IF PAST TEA BREAK TIME, BUMP UP COLOR AND MESSAGE VALUE AGAIN
  //AND TURN OFF TIMER (BUT DON'T RESET TEA RINGS)
  if (millis() > (clickMillis+teaBreakLength)) {
    timerGoFlag = false;
    cNum = stageNum + 3;
    mNum = stageNum + 3;
  }

  //IF THE STAGE HAS CHANGED (AS INDICATED BY THE cNum) UPDATE THE BLINK1
  if (pcNum != cNum) {
    sendColor2Blink1(btColors[cNum]);
  }
  //put current cNum into variable for the previous
  pcNum = cNum;

  //DRAW THE TEXT FIELDS

  text(messages[mNum], textMargin, (textTop));
  text(timeString, textMargin, (textTop + lineHeight*lineSpacing));

  //DRAW IN THE "BACKGROUND" (i.e. the teacup)
  drawBackground();
}

//-------------------------------------------------------------------- END DRAW

//------------------------------------------------------------------------------
//--------------------------------------------------------- START EVENT HANDLERS
//------------------------------------------------------------------------------

//---------------------------------------------------------- START MOUSERELEASE
//on mouse release, either clear the timer or start it, depending
//on status of timerGoFlag
void mouseReleased() {
  if (timerGoFlag) {
    //TURN OFF TIMER
    //empty messages
    cNum = 0;
    mNum = 0;
    //teaRings goodbye
    s = 0;
    //timer off
    timerGoFlag = false;
    //blink1 off
    sendColor2Blink1(btColors[cNum]);
  } 
  else {
    //TURN ON TIMER
    //starting messages
    cNum = 1;
    mNum = 1;
    //teaRings goodbye
    s = 0;
    //timer on
    clickMillis = millis();
    timerGoFlag = true;
    //light up blink1 with first color. 
    sendColor2Blink1(btColors[cNum]);
  }
}


//------------------------------------------------------------ END MOUSERELEASE

//---------------------------------------------------------- START EXIT HANDLER
private void prepareExitHandler () {
  Runtime.getRuntime().addShutdownHook(new Thread(new Runnable() {
    public void run () {
      //System.out.println("SHUTDOWN HOOK");
      //What to do on closing here
      //TURN OFF & RELEASE myBlink1
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

//--------------------------------------------------------------- drawBackground
void drawBackground() {
  //DRAW TEA CUP
  stroke(cupColor);
  noFill();
  ellipse(artCenterW, artCenterH, cupSize, cupSize);
}

//-------------------------------------------------------------- sendColor2Blink1
//UNPACKS A COLOR OBJECT AND SENDS IT TO BLINK1
void sendColor2Blink1(color y) {
  //sharp
  myBlink1.setRGB(int(red(y)), int(green(y)), int(blue(y)));
  //fade
  //myBlink1.fadeToRGB(mps, int(red(y)), int(green(y)), int(blue(y)));
}

//--------------------------------------------------------- makeTimeDisplayString
//RETURNS A STRING WITH FORMAT m:ss OF THE DIFFERENCE BETWEEN TWO TIMES GIVEN
//IN MILLISECONDS
String makeTimeDisplayString(long startTime, long endTime) {
  //get the difference
  long ellapsedMillis = endTime-startTime;
  //how many seconds total
  float tempTime = ellapsedMillis/(mps);
  //how many minutes
  int ellapsedMinutes = int(tempTime/60);
  //how many seconds remaining
  int ellapsedSeconds = int( (tempTime % 60));
  //create string with minutes and remaining seconds (forced zero)
  String rString = str(ellapsedMinutes) + " : " + intToStringWithTensDigit(ellapsedSeconds);
  //return string
  return rString;
}

//---------------------------------------------------------- intToStringWithTensD
//FORCES THE TENS DIGIT
String intToStringWithTensDigit(int n) {
  String rString;
  if (n < 10) {
    rString = "0" + str(n);
  } 
  else {
    rString = str(n);
  }
  return rString;
}
