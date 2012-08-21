
import java.awt.Color;

/**
 *
 * format: "name,repeats,color1,color1time,color1holdtime,color2,c2time,c2hold"
 * 
 *
 *
 */
public class Pattern
{
  public static final int OFF = 0;
  public static final int CHANGE = 1;
  public static final int HOLD = 2;

  String name;    // name of pattern
  int repeats;    // number of repeats, 0 = inf
  Color[] colors; // list of colors in pattern
  float[] timeschng;  // list of times per color in pattern
  float[] timeshold;  // list of hold times per color

  int playpos = 0;
  int mode = 0;  // 0=off, 1=changing, 2=holding
  int repsleft = 0;
  
  long lasttime = 0;
  long nexttime = 0;

  public Pattern( String patternstr) 
  {
    String[] vals = patternstr.split(",");
    name = vals[0];
    repeats = Integer.valueOf( vals[1] ).intValue(); 
    int p=0;
    int l = (vals.length-2) / 3 ;
    System.out.println("l="+l+", "+vals.length);
    colors = new Color[l];
    timeschng = new float[l];
    timeshold = new float[l];
    for( int i=2; i< vals.length; i+=3 ) {
      Color c = Color.BLACK;
      float tc = (float)0.1;
      float th = (float)0.5;
      c = Color.decode( vals[i] );  // FIXME:
      //int t = Integer.valueOf( vals[i+1] ).intValue();
      tc = Float.parseFloat( vals[i+1] );
      th = Float.parseFloat( vals[i+2] );
      //System.out.println("c:"+c+", tc:"+tc+", th:"+th);
      colors[p] = c;
      timeschng[p] = tc;
      timeshold[p] = th;
      p++;
    }
  }

  /**
   * actions: 
   * at beginning of new CHANGE or HOLD state, send comamnd to blink1
   * algorithm
   * - if pattern is new (playpos=0, mode=OFF), start playing (OFF->CHANGE)
   * - if change time is up, move to holding (CHANGE->HOLD)
   * - if hold time is up, move to next color (HOLD->CHANGE, playpos++)
   * -
   */
  public void update(long now)
  {
    lasttime = now;
    float deltac=0, deltah=0;
    boolean modeChange = false;

    deltac = 1000 * timeschng[ playpos ];
    deltah = 1000 * timeshold[ playpos ];
    
    if( mode == OFF ) { 
      mode = CHANGE;
      modeChange = true;
    }
    else if( mode == CHANGE ) {
      if( now > (deltac + lasttime) ) { // time to hold 
        mode = HOLD;
        modeChange = true;
      }
    }
    else if( mode == HOLD ) { 
      if( now > (deltah + lasttime) ) { 
        mode = CHANGE; // loop
        modeChange = true;
      }
    }
    nexttime = now + (int)deltac;
  }

  /*
  public boolean modeChange()
  {
    return modeChange;
    }*/

  //
  public String toString()
  {
    String str = "Pattern:"+name+","+repeats+",";
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
