/*
 * Blink1ColorOrgan --
 *
 * Just a heavily modified version of:
 * http://projects.mathfarmer.com/home/12-band-color-organ-rgb-led
 *
 * Requires Minim: http://code.compartmental.net/tools/minim/
 *
 * 2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 *
 */

import thingm.blink1.*;

import ddf.minim.analysis.*;
import ddf.minim.*;

Blink1 blink1;

// Sound Input and processing objects
Minim minim;
AudioInput myInput;
AudioOutput myOutput;
BeatDetect beat;
FFT fftL;
FFT fftR;
int bufferSize = 2048;
int minBeatPeriod = 300; // if new "beat" is < 300 ms after last beat, ignore it

int updateMillis = 50;

// Frequency analysis
float decay = 0.99f;
float thresBot = 0.3;
float thresTop = 0.9;
float[] peaks;
float[] peakSinceUpdate;
float[] noiseLvl;
float minPeak = 0.1; // Used to stop lights flickering at start due inaudible noise
boolean trackNoiseLvl = false;
float maxPeak = 0;
int maxPeakIdx = 0;
int bandNumber;

// Color state
int posOffset = 0;
byte rr;
byte gg;
byte bb;

// Communications
boolean inputReady=false;
long lastUpdate;
byte[] drawState; // I'm assuming 15-bit light data
byte lowByte;
byte highByte;

// Constants to configure
int ledCount = 1;  //How many LEDs in your string.
//int[] colorIndex = { 
//  0xff0000, 0xff0000, 0xff0000, 0xffff00, 0x00ff00, 0x00ff00, 
//  0x00ff00, 0x0000ff, 0x0000ff, 0x0000ff, 0xff00ff, 0xff00ff
//}; // Standard HTML 24-bit RGB hex color notation.
int[] colorIndex = { 
  0xff0000, 0xff0000, 0xff0000, 0xff0000, 
  0xff0000, 0x00ff00, 0x00ff00, 0x0000ff, 
  0x0000ff, 0x0000ff, 0x0000ff, 0x0000ff, 
}; // Standard HTML 24-bit RGB hex color notation.
int bandLimit = 12;
int startingQ = 55;
int octaveDivisions = 2;

// ********** BEGIN ***********
void setup() {  
  size(300,300);

  blink1 = Blink1.open();
  if( blink1.error() ) { 
      println("uh oh, no Blink1 device found");
  }

  // Init all the sound objects
  minim = new Minim(this);
  myInput = minim.getLineIn(Minim.STEREO, bufferSize);
  fftL = new FFT(myInput.bufferSize(), myInput.sampleRate());
  fftL.logAverages(startingQ,octaveDivisions);
  fftL.window(FFT.HAMMING);
  fftR = new FFT(myInput.bufferSize(), myInput.sampleRate());
  fftR.logAverages(startingQ,octaveDivisions);
  fftR.window(FFT.HAMMING);
  beat = new BeatDetect(myInput.bufferSize(), myInput.sampleRate());
  beat.setSensitivity(minBeatPeriod);

  // Init tracking data
  drawState = new byte[ledCount*2 + 2];
  drawState[ledCount*2] = 0; drawState[ledCount*2 + 1] = 0; // Terminating bytes
  bandNumber = min(bandLimit, fftL.avgSize());
  peaks = new float[bandNumber];
  peakSinceUpdate = new float[bandNumber];
  noiseLvl = new float[bandNumber];
  for (int i = 0; i < bandNumber; ++i) peaks[i] = minPeak;

  lastUpdate = millis();
}

void draw() {
  beat.detect(myInput.mix);
  fftL.forward(myInput.left);
  fftR.forward(myInput.right);

  checkPeaks();

  colorOrgan();

  updateScreen();
}

void checkPeaks() {
  boolean newPeak = false;
  boolean newMaxPeak = false;

  // Grab the new level data. Check to see if it represents a new peak.
  //   Also check to see if there is a new max peak.
  //   If there are no new peaks, decay the levels of the current peaks.
  //     (this acts as a primitive auto-level control, and helps emphasize 
  //      changes in volume)
  for (int i=0; i < bandNumber; i++) {
    if (fftL.getAvg(i) + fftR.getAvg(i) > peaks[i]) {
      peaks[i] = fftL.getAvg(i) + fftR.getAvg(i);
      
      if (peaks[i] > maxPeak) { 
        newMaxPeak = true; 
        maxPeak = peaks[i];
        maxPeakIdx = i;
      }
    }
    if (!newPeak) {
      peaks[i] *= decay;
      if (peaks[i] < minPeak) peaks[i] = minPeak;
    }
  }
  if (!newMaxPeak) {
    maxPeak *= decay;
    if (maxPeak < minPeak) maxPeak = minPeak;
  }

  // Raise the other peaks based on the max peak. This allows a few
  //   fequency bands to dominate the display when those frequencies also
  //   dominate the sound spectrum. The power function makes more distant
  //   frequency bands less affected by this shaping. The value of 0.8 
  //   (and heck, the function) was the result of crude experimentation.
  //   There are probably better methods for this, but it seems to do
  //   about what I want.
  for (int i = 0; i < bandNumber; i++) {
    float peakTop = maxPeak*(pow(0.8,abs(i-maxPeakIdx)));
    if (peaks[i] < peakTop) peaks[i] = peakTop;
  }

  if (trackNoiseLvl) setNoiseFloor();
  
  // I'm not sure I'm totally sold on this. It seems a little busy.
  if (beat.isKick()) posOffset++;
  if (posOffset >= bandNumber) posOffset = 0;
}

int tr = 0;
int tg = 0;
int tb = 0;

void colorOrgan() {

  for (int i=0; i < bandNumber; i++) {
    int col = colorIndex[i%colorIndex.length];
    float amp = fftL.getAvg(i) + fftR.getAvg(i);
    print(nf(int(10*amp),2,0) + ", ");

    // Check noise threshold. If above, normalize amp to [0-1].
    if (amp > noiseLvl[i]) amp = (amp)/peaks[i];
    else amp = 0;

    // Shape the band levels. Peg values above or below the upper and lower
    //   bounds. Remap the middle so that it covers the full range. Less space
    //   between the bounds makes things blinkier.
    if (amp < thresBot) amp = 0;
    else if (amp > thresTop) amp = 1;
    else amp = amp/(thresTop - thresBot) - thresBot;
    if (amp < 0) amp = 0;
    else if (amp > 1) amp = 1;

    // Hold on the biggest amplitudes we've seen since the last update. This
    //   is so that we don't lose transients if it takes too long to communicate
    //   with the lights. I'm not sure how much of a difference this makes 
    //   though.
    if (amp > peakSinceUpdate[i]) peakSinceUpdate[i]=amp; else amp=peakSinceUpdate[i];

    // Set the colors from the amplitudes
    int rrr = (int)( ((col&0xff0000) >> 16)*amp );
    int ggg = (int)( ((col&0x00ff00) >>  8)*amp ); 
    int bbb = (int)( ((col&0x0000ff)      )*amp );
    
    tr += rrr;
    tg += ggg;
    tb += bbb;

  }
  println();

  int div = bandNumber/3;  // such a hack
  tr /= div;
  tg /= div;
  tb /= div;

}


void updateScreen() {

  if( (millis() - lastUpdate) > updateMillis ) { 
    lastUpdate = millis();

    background( color(tr,tg,tb) );
    // println("r,g,b: "+ hex(tr,2) +","+ hex(tg,2) +","+ hex(tb,2));

    blink1.setRGB( tr, tg, tb );

    clearPSU();
    tr = 0;
    tg = 0;
    tb = 0;
  }

}

public void stop() {
  // always close Minim audio classes when you are done with them
  myInput.close();
  myOutput.close();
  minim.stop();

  super.stop();
}


void clearPSU() {
  for (int i = 0; i<bandNumber; ++i) {
    peakSinceUpdate[i] = 0;
  } 
}

// This is used primarily when taking audio from an external input. Since
//   I automatically reset levels based on recent input volume, even a 
//   small amount of noise from the external source will eventually light 
//   up some of the lights, which can ruin the effect of quiet passages 
//   in the music. The somewhat crude solution is to set a noise threshold 
//   when no music is playing. Sound must exceed the volume of the noise in 
//   order to be recognized. This check is done on a per-band basis, so a 
//   lot of noise in one band (e.g. a 60Hz hum) won't interfere with the 
//   sensitivity of other bands.
//
//   Anyways, to set the noise threshold, hold down 'n' when no music is
//   playing to sample the noise.
void keyPressed() {
  if ( key == 'n' ) {
    if (!trackNoiseLvl) {
      //for (int i=0; i < fftL.avgSize(); i++) {
      for (int i=0; i < bandNumber; i++) {

        noiseLvl[i] = 0;
      }
      trackNoiseLvl = true;
    }
  }
}

void keyReleased() {
  if ( key == 'n' ) {
    trackNoiseLvl = false;
  }
}

void setNoiseFloor() {
  for (int i=0; i < bandNumber; i++) {
    if (fftL.getAvg(i)+fftR.getAvg(i) > noiseLvl[i]) {
      noiseLvl[i] = fftL.getAvg(i)+fftR.getAvg(i);
    }
  }
}

