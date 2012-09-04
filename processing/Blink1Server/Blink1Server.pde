//
//
//
import thingm.blink1.*;

int serverport = 8080;

PatternPlayer player;

int sketchWidth  = 400;
int sketchHeight = 240;

int colorPickerX = 10;
int colorPickerY = 20;
int colorPickerWidth  = 255; // this is harccoded in createColorPicker
int colorPickerHeight = 207; // this is hardcoded in createColorPicker
int previewWidth  = 40;
int previewHeight = 40;
int pickX, pickY;
int previewX, previewY;

color backColor = color(226);
color previewColor = color(255,0,255); // start off with a default
PGraphics colorPickerImage;
PGraphics alphaImage;
PFont font;

boolean doRefresh = true;

//
void updateBlink1()
{
  int r = int(red(previewColor));
  int g = int(green(previewColor));
  int b = int(blue(previewColor));
  
  println("r,g,b: "+r+","+g+","+b );
  java.awt.Color c = new java.awt.Color( r,g,b );
  player.updateBlink1( 100, c );
}

//
void setup()
{
  size(sketchWidth, sketchHeight);
    
  frameRate(15);
  smooth();
  font = loadFont("LucidaSans-12.vlw");

  File docroot = new File(dataPath("html"));

  player = new PatternPlayer();

  // http://elonen.iki.fi/code/nanohttpd/
  Blink1NanoHTTPD server;
  try {
      server = new Blink1NanoHTTPD( serverport, docroot);
      server.setPatternPlayer(player);
  } catch(Exception e) { 
      println("Couldn't start server on port "+serverport);
  }

  pickX = colorPickerX + (colorPickerWidth/8)*7; // hack
  pickY = colorPickerY + colorPickerHeight/2;

  previewX = colorPickerX + colorPickerWidth + 30;
  previewY = colorPickerY - 5 ;

  colorPickerImage = createColorPickerImage();
  alphaImage = createGradient( previewWidth, previewHeight, previewWidth);

  updateBlink1();
}

long lastMillis;
//
void draw()
{
  background( backColor );
  drawColorPicker();
  
  drawPreview( previewX, previewY, previewWidth, previewHeight);
  
  drawStatus();

  if( (millis() - lastMillis) > 1000 ) { 
      lastMillis = millis();
      //println("getting version");
      player.pingBlink1();
  }
}

//
void mousePressed() {
  // if in colorpicker, we're updating previewColor
  if( mouseX > colorPickerX && mouseX < (colorPickerX+colorPickerWidth) &&
      mouseY > colorPickerY && mouseY < (colorPickerY+colorPickerHeight) ) {
    //previewColor = get(mouseX,mouseY);
    previewColor =colorPickerImage.get(mouseX-colorPickerX,mouseY-colorPickerY);
    pickX = mouseX; pickY = mouseY;

    updateBlink1();
  }
}
// handle drag the same way as a press, kinda lame, yeah
void mouseDragged() {
  mousePressed();
}

void drawStatus() 
{
  textFont(font, 12);
  String statusText = player.getStatus();
  text( statusText, colorPickerX+colorPickerWidth+5, height-13);
}


//
void drawPreview(int x, int y, int w, int h) {
  color c =  previewColor;
  pushMatrix();
  translate(x,y);
  textFont(font, 12);
  text("color", 0,0);
  text("selected", 0,12);
  strokeWeight(1.0);
  stroke(180);
  fill(c);
  rect( 0,15, w,h);
  //blend(alphaImage, x+0,y+15, previewWidth,previewHeight,x+0,y+15,previewWidth,previewHeight, OVERLAY);
  blend(alphaImage, 0,0, previewWidth,previewHeight,previewX,previewY+15,previewWidth,previewHeight, BLEND);
  fill(10);
  textFont(font,8);
  text( int(red(c)),   w+2,25);
  text( int(green(c)), w+2,35);
  text( int(blue(c)),  w+2,45);
  popMatrix();
}

// really just render the pre-computed image, created w/ createColorPickerImage
void drawColorPicker() {
  fill(10);
  textFont(font, 12);
  text("pick a color", colorPickerX, colorPickerY-5);
  image(colorPickerImage, colorPickerX, colorPickerY);
  noFill();
  stroke( 200 );
  rect( pickX-4,pickY-4, 8,8 );
  stroke( 40 );
  rect( pickX-3,pickY-3, 8,8 );
}

//
PGraphics createGradient( int w, int h, int radius ) {
    PGraphics pg;
    pg = createGraphics( w,h, JAVA2D );
    pg.smooth();
    pg.strokeWeight(3);
    pg.beginDraw();
    pg.pushMatrix();
    pg.translate(w/2,h/2);
    pg.ellipseMode(CENTER);
    pg.noStroke();
    for( int i=radius; i>0; i--) {
        int v = 255-i*7;
        pg.fill(255,v);
        pg.ellipse( 0,0, i,i );
    }
    pg.popMatrix();
    pg.endDraw();
    return pg;
}

// just do this plotting once because it's static and 
// sucks up cycles if computed every draw()
PGraphics createColorPickerImage() {
  PGraphics pg;
  pg = createGraphics(255,200+4+4,JAVA2D);
  pg.beginDraw();
  pg.colorMode(HSB,255,100,100);
  pg.stroke(255,0,100);
  pg.fill(255,0,100);
  pg.rect(0,0, 255,4);

  for (int i = 0; i < 255; i++) {
    for (int j = 0; j < 100; j++) {
      pg.stroke( i, j, 100);
      pg.point( i, 4+j);
      pg.stroke( i, 100, 100-j);
      pg.point( i, 4+j+100 );
    }
  }
  pg.stroke(255,0,0);
  pg.fill(255,0,0);
  pg.rect(0,204, 255,4);
  pg.endDraw();
  return pg;
}


/*
 from : ~/projects/blinkm/firmware-new/test/generate_lut.c

uint8_t log2lin_algo(uint8_t n) {
  return (((1<<(n/32))-1) + ((1<<(n/32))*((n%32)+1)+15)/32);
}
uint16_t log2lin_first(uint8_t n) {
  return (1<<(n/16))-1;
}
uint16_t log2lin_9bit(uint8_t n) {
  return (((1<<(n/31))-1) + ((1<<(n/31))*((n%31)+1)+15)/31);
}
uint8_t log2lin_float1(uint8_t n) {
    return  1.0* (n * 0.707 );  // 1/sqrt(2)
}
uint8_t log2lin_float2(uint8_t n) {
    return  1.0* (n * 0.707 );  // 1/sqrt(2)
}
uint8_t log2lin_avrfreaks(uint8_t n) { 
    return pow(2, round((n+1)/32) ) - 1 ;
}
uint8_t log2lin_gamma22(uint8_t n) { 
    return ( 255 * pow((n+0)/255.0, 2.2));
}

*/
