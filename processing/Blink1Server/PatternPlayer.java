
import thingm.blink1.Blink1;

import java.util.*;

/**
 * PatternPlayer contains a list of Patterns to be play on one or more blink(1)s
 *
 * Upon addition of Pattern to Player, pattern is started playing
 * 
 *
 *
 */
public class PatternPlayer implements Runnable
{

  public static Blink1 blink1;
  public String statusText;
  long nextTimeMillis = 0;
  Thread runner;
  boolean done = false;

  Map<String,Pattern> patterns;
  // maybe this should be a hashmap?  biink1 -> pattern

  //
  public PatternPlayer()
  {
    patterns = new HashMap<String,Pattern>();

    blink1 = new Blink1();
    int rc = blink1.open();
    if( rc==-1 ) { 
      System.out.println("oops no blink1");
      statusText = "no blink1 found";
    }
    else {
      statusText = "blink1 found";
    }
    blink1.close();

    runner = new Thread(this, "PatternPlayer");
    runner.start();
  }
  
  //
  public void run()
  {
    while( !done ) { 
      long now = System.currentTimeMillis();
      if( now > nextTimeMillis ) {
        updatePlayer(now);
        //
      }
      try {	//delay 
        Thread.sleep( 5000 );
      } catch (InterruptedException e) { 
        System.out.println("player interrupted");
      }
    }

  }

  //
  public void updatePlayer( long now )
  {
    System.out.println("updatePlayer");
    for( String name : patterns.keySet() ) {
      Pattern p = patterns.get(name);
      p.update(now);
      System.out.println(name+" : "+ p);
    }    
    
    nextTimeMillis = now + (5 * 100);
    
  }

  //
  public void listPatterns()
  {
    int i=0;
    System.out.println("listPatterns:");
    //for( Pattern p : patterns ) {  // Set
    for( String name : patterns.keySet() ) {
      System.out.println(i+":"+name+" p:"+ patterns.get(name));
      i++;
    }
    
  }

  //
  public void playPattern(String blink1id, Pattern pattern)
  {
    patterns.put( pattern.name, pattern );
    //updatePlayer( System.currentTimeMillis() );
    runner.interrupt();
  }

  //
  public String getStatus()
  {
    return statusText;
  }


}
