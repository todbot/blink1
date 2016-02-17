
import thingm.blink1.Blink1;

import java.util.*;
import java.util.concurrent.*;
import java.awt.Color;

//import java.util.concurrent.TimeUnit.*;

/**
 * PatternPlayer contains a list of Patterns to be play on one or more blink(1)s
 *
 * Upon addition of Pattern to Player, pattern is started playing
 * 
 *
 *
 */
public class PatternPlayer  //implements Runnable
{

  public static Blink1 blink1;
  public String statusText;
  long nextTimeMillis = 0;
  Thread runner;
  boolean done = false;

  Map<String,Pattern> patterns;
  // maybe this should be a hashmap?  biink1 -> pattern

  ScheduledExecutorService  executor;


  //
  public PatternPlayer()
  {
    patterns = new HashMap<String,Pattern>();

    blink1 = Blink1.open();
    if( blink1.error() ) { 
      System.out.println("oops no blink1");
      statusText = "no blink1 found";
    }
    else {
      statusText = "blink1 found";
    }
    //blink1.close();

    executor = Executors.newSingleThreadScheduledExecutor();
    executor.scheduleAtFixedRate( new Runnable() { 
        public void run() { 
          updatePatterns( System.currentTimeMillis() );
        }
      },
      0, 
      25, 
      //200, 
      TimeUnit.MILLISECONDS
      );
  }

  // note: synchronized
  public synchronized void updateBlink1( int colorMillis, Color c )
  {
    System.out.println("updateBlink1: "+colorMillis+":"+c);
    //blink1.open();
    blink1.fadeToRGB( colorMillis, c.getRed(), c.getGreen(), c.getBlue() );
    //blink1.close();
  }

  // note: synchronized
  public synchronized void pingBlink1()
  {
    //blink1.open();
    blink1.getFirmwareVersion();
    //blink1.close();
  }
  //
  public void updatePlayer( long nowMillis )
  {
    //long nowMillis = System.currentTimeMillis();
      if( nowMillis > nextTimeMillis ) {
        updatePatterns( nowMillis );
        nextTimeMillis = nowMillis + (5 * 100);
      }
  }

  //
  public void updatePatterns(long nowMillis)
  {
    for( String name : patterns.keySet() ) {
      Pattern p = patterns.get(name);
      //System.out.println("updatePatterns:"+name +":"+p.mode() );
      p.update(nowMillis);
    }
  }

  //
  public void listPatterns()
  {
    int i=0;
    System.out.println("listPatterns:");
    //for( Pattern p : patterns ) {  // Set
    for( String name : patterns.keySet() ) {
      System.out.println("-- "+i+":"+name+" p:"+ patterns.get(name));
      i++;
    }
    
  }

  // FIXME: what about blink1id
  public void addPattern(String blink1id, Pattern pattern)
  {
    patterns.put( pattern.name, pattern );
  }

  //
  public boolean playPattern(String blink1id, String patternName )
  {
    Pattern pattern = patterns.get( patternName );
    if( pattern == null )
      return false;
    pattern.start();
    return true;
  }


  /*
  // FIXME: what about blink1id
  public void playPattern(String blink1id, Pattern pattern)
  {
    addPattern( blink1id, pattern );
    pattern.start();
    //updatePlayer( System.currentTimeMillis() );
    //runner.interrupt();
  }
  */


  //
  public String getStatus()
  {
    return statusText;
  }


}
