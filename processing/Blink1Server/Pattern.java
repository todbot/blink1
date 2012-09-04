
import java.awt.Color;

/**
 *
 * format: "name,repeats,color1,color1time,color2,c2time,..."
 * 
 * maybe: "name,repeats,color1,color1time,color1holdtime,color2,c2time,c2hold"
 *
 *
 */
public class Pattern
{
  public static final int START  = 0;
  public static final int CHANGE = 1;
  public static final int HOLD   = 2;
  public static final int STOP   = 3;

  PatternPlayer player;

  String name;    // name of pattern
  int repeats;    // number of repeats, 0 = inf
  Color[] colors; // list of colors in pattern
  float[] timeschng;  // list of times per color in pattern
  float[] timeshold;  // list of hold times per color

  int playpos = 0;
  int mode = STOP; 
  int playcount = 0;
  
  long lastMillis = 0;

  public Pattern(PatternPlayer parent, String patternstr) 
  {
    player = parent;

    String[] vals = patternstr.split(",");
    name = vals[0];
    repeats = Integer.valueOf( vals[1] ).intValue(); 
    int p=0;
    int l = (vals.length-2) / 2 ;
    System.out.println("l="+l+", "+vals.length);
    colors = new Color[l];
    timeschng = new float[l];
    timeshold = new float[l];
    for( int i=2; i< vals.length; i+=2 ) {
      Color c = Color.BLACK;
      float tc = (float)0.1;
      float th = (float)0.0;
      try { c = Color.decode( vals[i] ); } catch(Exception e) { } 
      //int t = Integer.valueOf( vals[i+1] ).intValue();
      try { tc = Float.parseFloat( vals[i+1] ); } catch(Exception e) { } 
      colors[p] = c;
      timeschng[p] = tc;
      timeshold[p] = th;
      System.out.println("c:"+c+", tc:"+tc+", th:"+th);
      p++;
    }
  }

  /**
   * actions: 
   * at beginning of new CHANGE or HOLD state, send comamnd to blink1
   * algorithm
   * - if pattern is new (playpos=0, mode=START), start playing (START->CHANGE)
   * - if change time is up, move to holding (CHANGE->HOLD)
   * - if hold time is up, move to next color (HOLD->CHANGE, playpos++)
   * -
   */
  public void update(long nowMillis)
  {
    float deltac=0, deltah=0;
    boolean updateLed = false;

    deltac = 1000 * timeschng[ playpos ];  // convert to millis
    deltah = 1000 * timeshold[ playpos ];  // convert to millis
    
    if( mode == STOP ) { 
      return;
    }
    else if( mode == START ) {  // we've just been started
      mode = CHANGE;     // next state is changing
      updateLed = true; // and we need to act
      player.updateBlink1( (int)deltac, colors[playpos] );
      lastMillis = nowMillis;
    }
    else if( mode == CHANGE ) { // we've started changing, 
      if( (nowMillis - lastMillis) > deltac ) { // are we at the top?
        mode = HOLD;
        updateLed = true;
        lastMillis = nowMillis;
      }
    }
    else if( mode == HOLD ) { 
      if( (nowMillis - lastMillis) > deltah ) { 
        mode = START; // loop
        updateLed = true;
        lastMillis = nowMillis;
        // increment play pos, decrement play count
        playpos++;
        if( playpos == colors.length ) { 
          playpos = 0;
          playcount++;
          if( playcount == repeats ) {  
            mode = STOP;
            playcount = 0;
          }
        }
      }
    }
    /*
    System.out.println(name+":"+playpos+":"+playcount+",mode:"+mode+
                       ",updateLed:"+updateLed+", dc:"+deltac+",dh:"+deltah+
                       ",last:"+lastMillis+",now:"+nowMillis);
    */
  }

  /*
  public boolean modeChange()
  {
    return modeChange;
    }*/

  //
  public void start()
  {
    mode = START;
  }
  //
  public void stop()
  {
    mode = STOP;
  }
  
  public int mode()
  {
    return mode;
  }

  //
  public String toString()
  {
    String str = "Pattern:"+name+":"+repeats+":"+mode+":"+playpos+":";
    for( int i=0; i< colors.length; i++ ) { 
      Color c = colors[i];
      String s = String.format( "#%02x%02x%02x,%.2f,%.2f,", 
                                c.getRed(),c.getGreen(),c.getBlue(),
                                timeschng[i], timeshold[i] );
      str += s;
    }
    return str;
  }
}
