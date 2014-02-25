/*
 * BlinkATweet -- Flash blink(1) when a particular tweet comes in
 *
 * Note: This uses official ThingM OAuth credentials. 
 *       You will want to change the credentials to your own.
 *
 * 2012, Tod E. Kurt, http://thingm.com/ , http://todbot.com/blog/
 * 
 */

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.plaf.metal.*;
import java.util.regex.*;
import java.util.*;

import thingm.blink1.*;


static final boolean debug = true;

// your Oauth info
static String OAuthConsumerKey = "RFYIxhaoqkXJgfnbqUv8PQ";
static String OAuthConsumerSecret = "pXSVrAvFhFMXbuKe0RVaL7bJ42eQYGjNkK6oqNNvlk";
// your Access Token info
static String AccessToken = "23659748-gMN9Qf6wy9C2F9HNPcHQ10vcNeBJK5MCoZV0CWM";
static String AccessTokenSecret = "m93E2BIdCmwKSycmEyjR2EpLfKCkQGFSAU9278LygU";


Blink1 blink1;

TwitterStream twitter = new TwitterStreamFactory().getInstance();
boolean twitterSetupDone = false;

long lastMillis;
Color lastColor = Color.gray;
Color screenColor = Color.gray;
int mentionCount=255;

String colorfile = "rgb.txt";
HashMap colormap;  // stores String -> Color mappings of rgb.txt file
PFont font;
String lastMsg = "BlinkATweet!";

String watchString = "Getting ready to watch Twitter";
String mentionString1 = "kickstarter";
String mentionString2 = "oshw";
String[] keywords = new String[] { mentionString1, mentionString2 }; 

//
void setup() {
  size(600,400);
  frameRate(10);

  font = loadFont("HelveticaNeue-CondensedBold-18.vlw"); 
  textFont( font  );
  colormap = parseColorFile(colorfile);

  blink1 = Blink1.open();
  if( blink1.error() ) { 
    updateMsg("uh oh, no Blink1 device found");
  }
  blink1.fadeToRGB(1000, 0,0,0 );

  lastMillis = millis();
}

//
void draw() {
  background(1);
  color lc = screenColor.getRGB();

  // make circle last tweeted color, as nice radial alpha gradient
  createGradient(width/2, width/2, width*3/4,  lc, color(0,0,0,10)  ); 

  noStroke();
  fill(0,0,0,50);
  roundrect( 30,height-80, width-80,60, 30); // draw text background

  textAlign(CENTER);
  fill(100,100,100);
  text(watchString, 9,20, width-40,70 );
  int r = (int)(screenColor.getRed() * 0.90);
  int g = (int)(screenColor.getGreen() * 0.90);
  int b = (int)(screenColor.getBlue() * 0.90);
  screenColor = new Color( r,g,b );    

  if( mentionCount>0 ) { 
    fill(255,255,255,mentionCount);
    mentionCount -= 5; // fade out
    text(lastMsg, 9, height-80, width-40,70 );  // draw text
  }
  
  long t = millis();
  if( (t-lastMillis) > 10000 ) {  // just a heartbeat
    lastMillis = t;
    println("listening to twitter for "+keywords); //mentionString1+" & "+mentionString2); 
  }

  //
  if( frameCount==1 && !twitterSetupDone ) {
    showTwitterSetupDialog();
  }

}

// update the status message at bottom of screen
void updateMsg(String s) {
  mentionCount = 255;
  lastMsg = s;
}

// let you trigger random colors just to see what's what
void keyPressed() {
  int r = int(random(255));
  int g = int(random(255));
  int b = int(random(255));
  lastColor = new Color(r,g,b); 
  // better way of doing this?
  screenColor = new Color(lastColor.getRed(),lastColor.getGreen(),lastColor.getBlue());

  println("keyPressed: "+lastColor);
  blink1.fadeToRGB( 200, r,g,b );
  delay(100);
  blink1.fadeToRGB( 2000, 0,0,0 );
}


//
void setupTwitter() { 
  connectTwitter();
  twitter.addListener(listener);
  if (keywords.length==0) twitter.sample();
  else twitter.filter(new FilterQuery().track(keywords));
}

// Initial connection
void connectTwitter() {
  twitter.setOAuthConsumer(OAuthConsumerKey, OAuthConsumerSecret);
  AccessToken accessToken = loadAccessToken();
  twitter.setOAuthAccessToken(accessToken);
}

// Loading up the access token
private static AccessToken loadAccessToken() {
  return new AccessToken(AccessToken, AccessTokenSecret);
}

// This listens for new tweet
StatusListener listener = new StatusListener() {
  public void onStatus(Status status) {

    println("@" + status.getUser().getScreenName() + " - " + status.getText());

    dbg(status.getUser().getName() + " : " + status.getText());
    String text = status.getText();
    String lctext = text.toLowerCase();
    
    updateMsg( "@"+status.getUser().getScreenName()+": "+text );

    int r = lastColor.getRed();
    int g = lastColor.getGreen();
    int b = lastColor.getBlue();

    screenColor = new Color(r,g,b); // better way of doing this?

    blink1.fadeToRGB( 100, r,g,b );
    delay( 100 );
    blink1.fadeToRGB( 2000, 0,0,0 );
    delay( 100 );

    // turn first blinkm color of tweet (if applicable)
      boolean rc = parseColors( lctext );
      if( rc ) {
        //blink1.fadeToRGB( 100, lastColor );
        blink1.fadeToRGB( 100, r,g,b );
      }
  }

  public void onDeletionNotice(StatusDeletionNotice statusDeletionNotice) {
    //System.out.println("Got a status deletion notice id:" + statusDeletionNotice.getStatusId());
  }
  public void onTrackLimitationNotice(int numberOfLimitedStatuses) {
    //  System.out.println("Got track limitation notice:" + numberOfLimitedStatuses);
  }
  public void onScrubGeo(long userId, long upToStatusId) {
    System.out.println("Got scrub_geo event userId:"+userId+" upToStatusId:"+upToStatusId);
  }
  public void onException(Exception ex) {
    ex.printStackTrace();
  }
};



//
// stolen from TwitterBlinkM in linkm projects
//

/**
 * Attempt to determine what color has been tweeted to us
 */
boolean parseColors(String text) {
  //println("text='"+text+"'");
  text = text.replaceAll("#",""); // in case they do #ff00ff
  Color c = null;
  String linepat = mentionString1 + "\\s+(.+?)\\b";
  Pattern p = Pattern.compile(linepat);
  Matcher m = p.matcher( text );
  if(  m.find() && m.groupCount() == 1 ) { // matched 
    String colorstr = m.group(1);
    dbg(" match: "+colorstr );
    c = (Color) colormap.get( colorstr );
    if( c !=null ) { 
      dbg("  color! "+c);
      lastColor = c;
      return true;
    }

    //colorstr = colorstr.replaceAll("#","");
    try {
      int hexint = Integer.parseInt( colorstr, 16 ); // try hex
      c = new Color( hexint );
      dbg("  color! "+c);
      lastColor = c;
      return true;
    } catch( NumberFormatException nfe ) { 
    }
  }

  return false;
}

/**
 * Parse the standard X11 rgb.txt file into a hashmap of name String -> Color
 * This is called only once on setup()
 */
HashMap parseColorFile(String filename) {
  HashMap colormap = new HashMap();
  String lines[] = loadStrings(filename);

  String linepat = "^\\s*(.+?)\\s+(.+?)\\s+(.+?)\\s+(.+)$";
  Pattern p = Pattern.compile(linepat);
  for( int i=0; i< lines.length; i++) { 
    String l = lines[i];
    Matcher m = p.matcher( l );
    if(  m.find() && m.groupCount() == 4 ) { // matched everything
      int r = Integer.parseInt( m.group(1) );
      int g = Integer.parseInt( m.group(2) );
      int b = Integer.parseInt( m.group(3) );
      String name = m.group(4);
      name = name.replaceAll("\\s+","").toLowerCase();
      Color c = new Color(r,g,b);
      colormap.put( name, c );
    }
  }

  if( debug ) {
    Set keys = colormap.keySet();
    Iterator it = keys.iterator();
    while (it.hasNext()) {
      String cname = (String)(it.next());
      dbg(cname + " - " + colormap.get(cname));
    }
  }

  return colormap;
}

//
void dbg(String s) {
  dbg( s,null);
}
//
void dbg(String s1, Object s2) {
  String s = s1;
  if( s2!=null ) s = s1 + " : " + s2;
  if(debug) println(s);
  //lastMsg = s1;
}

//
// stolen from: http://processing.org/learning/basics/radialgradient.html
//
void createGradient (float x, float y, float radius, color c1, color c2) {
  float px = 0, py = 0, angle = 0;

  // calculate differences between color components 
  float deltaR = red(c2)-red(c1);
  float deltaG = green(c2)-green(c1);
  float deltaB = blue(c2)-blue(c1);
  // hack to ensure there are no holes in gradient
  // needs to be increased, as radius increases
  float gapFiller = 8.0;

  for (int i=0; i< radius; i++){
    for (float j=0; j<360; j+=1.0/gapFiller){
      px = x+cos(radians(angle))*i;
      py = y+sin(radians(angle))*i;
      angle+=1.0/gapFiller;
      color c = color(
      (red(c1)+(i)*(deltaR/radius)),
      (green(c1)+(i)*(deltaG/radius)),
      (blue(c1)+(i)*(deltaB/radius)) 
        );
      set(int(px), int(py), c);      
    }
  }
  // adds smooth edge 
  // hack anti-aliasing
  noFill();
  strokeWeight(3);
  ellipse(x, y, radius*2, radius*2);
}

//
// stolen from: http://processing.org/discourse/yabb2/YaBB.pl?num=1213696787/1
//
void roundrect(int x, int y, int  w, int h, int r) {
  noStroke();
  rectMode(CORNER);
  
  int  ax, ay, hr;
  
  ax=x+w-1;
  ay=y+h-1;
  hr = r/2;
  
  rect(x, y, w, h);
  arc(x, y, r, r, radians(180.0), radians(270.0));
  arc(ax, y, r,r, radians(270.0), radians(360.0));
  arc(x, ay, r,r, radians(90.0), radians(180.0));
  arc(ax, ay, r,r, radians(0.0), radians(90.0));
  rect(x, y-hr, w, hr);
  rect(x-hr, y, hr, h);
  rect(x, y+h, w, hr);
  rect(x+w,y,hr, h);
  
}

//
void showTwitterSetupDialog()
{
  println("showTwitterSetupDialog");
  javax.swing.SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        try{ Thread.sleep(500); } catch(Exception e){}  // wait to avoid assert
        new TwitterSetupDialog();
      }
    } );

}

//
public class TwitterSetupDialog extends JDialog { //implements ActionListener {

  JTextField userfield,passfield;
  JTextField mention1field,mention2field;

  public TwitterSetupDialog() {
    super();

    try {  // use a Swing look-and-feel that's the same across all OSs
      MetalLookAndFeel.setCurrentTheme(new DefaultMetalTheme());
      UIManager.setLookAndFeel( new MetalLookAndFeel() );
    } catch(Exception e) { }  // don't really care if it doesn't work

    //JLabel l1 = new JLabel("Twitter username:");
    //userfield = new JTextField( username,20 );
    //JLabel l2 = new JLabel("Twitter password:");
    //passfield = new JTextField( password,20 );

    JLabel l3 = new JLabel("keyword one:");
    mention1field = new JTextField( mentionString1,15 );
    JLabel l4 = new JLabel("keyword two:");
    mention2field = new JTextField( mentionString2,15 );

    JButton cancelbut = new JButton("CANCEL");
    JButton okbut     = new JButton("OK");

    JPanel p = new JPanel(new GridLayout( 5,2, 5,5 ) );
    p.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

    //p.add(l1); p.add(userfield);
    //p.add(l2); p.add(passfield);
    p.add(l3); p.add(mention1field);
    p.add(l4); p.add(mention2field);
    p.add(cancelbut); p.add(okbut);

    getContentPane().add(p);

    pack();
    setResizable(false);
    setLocationRelativeTo(frame); // center it on screen
    setTitle("BlinkATweet Setup");
    setVisible(true);

    cancelbut.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent ae) {
          setVisible(false);  // do nothing but go away
          //setupTwitter();
          System.exit(0);
        }
      });
    okbut.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent ae) {
          setVisible(false);
          //username = userfield.getText();
          //password = passfield.getText();
          mentionString1 = mention1field.getText();
          mentionString2 = mention2field.getText();
          keywords = new String[] { mentionString1, mentionString2 }; 
          watchString = "Watching Twitter for '"+keywords[0]+"' & '"+keywords[1]+"'";

          setupTwitter();
        }
      });
  }

}
