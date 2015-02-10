package thingm.blink1;

import java.util.*;
import java.awt.Color;

public class Blink1 
{

  /**
   * pointer to native hid_device* 
   * do not use.
   */
  long blink1DevicePtr = 0;

  /**
   * Error code of last operation 
   * 0 = ok, anything else = error
   */
  int errorCode = 0;

  static {
    System.loadLibrary("Blink1");     // Load the library
  }  

  public static void usage() { 
    System.out.println("Usage: Blink1 <cmd> [options]");
  }

  /**
   * Simple command-line demonstration
   */
  public static void main(String args[]) {
    
    int rc;

    if( args.length == 0 ) {
      //usage();
    }
    
    System.out.println("Looking for blink(1) devices...");
    Blink1.enumerate(); 
    int count = Blink1.getCount();

    System.out.println("found "+count+ " devices");

    if( count == 0 ) {
      System.out.println("no devices found, would normally exit. continuing for error testing");
    }

    System.out.println("device id : serial : path:");
    String paths[] = Blink1.getDevicePaths();
    String serials[] = Blink1.getDeviceSerials();
    for( int i=0; i<paths.length; i++ ) { 
      System.out.println( i + ": "+ serials[i] + " : " + paths[i]);
    }
    
    System.out.println("Opening deviceId 0");

    Blink1 blink1 = Blink1.open();

    if( blink1.error() ) { 
      System.out.println("error on open(), no blink(1), next call will return error ");
    }

    System.out.print("fading to 10,20,30 ");
    rc = blink1.fadeToRGB( 100, 10,20,30 );
    System.out.println(" -- rc = "+rc);

    int ver = blink1.getFirmwareVersion();
    System.out.println("firmware version: " + ver);

    blink1.close();

    if( serials.length >= 2 ) {
      String serialA  = serials[0];
      String serialB  = serials[1];
      System.out.println("opening two devices: "+serialA+" and "+serialB);
      Blink1 blink1A = Blink1.openBySerial( serialA );
      Blink1 blink1B = Blink1.openBySerial( serialB );
      System.out.println("fading "+ serialA + " to red"); 
      blink1A.fadeToRGB( 100, 255,0,0 );
      Blink1.pause(500);
      System.out.println("fading "+ serialB + " to green");
      blink1B.fadeToRGB( 100, 0,255,0 );
      Blink1.pause(500);
      System.out.println("fading "+ serialA + " to blue"); 
      blink1A.fadeToRGB( 100, 0,0,255 );
      Blink1.pause(500);
      System.out.println("fading "+ serialB + " to purple"); 
      blink1B.fadeToRGB( 100, 255,0,255 );
      Blink1.pause(500);
      blink1A.close();
      blink1B.close();
      Blink1.pause(500);
    }

    Random rand = new Random();
    for( int i=0; i<10; i++ ) {
      int r = rand.nextInt() & 0xFF;
      int g = rand.nextInt() & 0xFF;
      int b = rand.nextInt() & 0xFF;
      
      int id = (count==0) ? 0 : rand.nextInt() & (count-1);
      
      System.out.print("setting device "+id+" to color "+r+","+g+","+b+"   ");

      blink1 = Blink1.openById( id );
      if( blink1.error() ) { 
        System.out.print("couldn't open "+id+" ");
      }
      
      // can do r,g,b ints or a single Color
      //rc = blink1.setRGB( r,g,b );
      Color c = new Color( r,g,b );
      rc = blink1.setRGB( c );
      if( rc == -1 ) 
        System.out.println("error detected");
      else 
        System.out.println();
      
      blink1.close();
      
      Blink1.pause( 300 );
    }

    System.out.println("Turn off all blink(1)s.");
    for(int n=0; n < count; n++){
      blink1 = Blink1.openById(n);
      blink1.setRGB(Color.BLACK);
      blink1.close();
    }

    System.out.println("Done.");
  }

  //--------------------------------------------------------------------------

  /**
   * Constructor.  Normally not used.  
   * Use static methods Blink1.open...() to get a Blink1 object
   */
  public Blink1() {
    //enumerate();
  }

  public boolean error() {
    //System.out.println("errorCode: "+errorCode);
    return (errorCode < 0);
  }

  /**
   * (re)Enumerate the bus and return a count of blink(1) device found.
   * @returns blink1_command response code, -1 == fail 
   */
  public static native int enumerate();

  /**
   * Get a count of blink(1) devices that have been enumerated.
   *
   */
  public static native int getCount();

  /**
   * Return the list of blink(1) device paths found by enumerate.
   *
   * @returns array of device paths
   */
  public static native String[] getDevicePaths();

  /**
   * Return the list of blink(1) device serials found by enumerate.
   *
   * @returns array of device serials
   */
  public static native String[] getDeviceSerials();


  /**
   * Open the first (or only) blink(1) device.
   */
  //public static native Blink1 openFirst();

  /**
   * Open the first (or only) blink(1) device.  
   * Causes an enumerate to happen.
   * Stores open device id statically in native lib.
   *
   * @returns Blink1 object 
   */
  public static native Blink1 open();

  /**
   * Close blink(1) device. 
   */
  public native void close();  

  /**
   * Open blink(1) device by USB path, may be different for each insertion.
   *
   * @returns Blink1 object or NULL if no device with that path found
   */
  public static native Blink1 openByPath( String devicepath );
  
  /**
   * Open blink(1) device by blink(1) serial number.
   *
   * @returns Blink1 object or NULL if no device with that serial found
   */
  public static native Blink1 openBySerial( String serialnumber );
  
  /**
   * Open blink(1) device by blink(1) numerical id (0-getCount()).
   * Id list is ordered by serial number.
   * @returns Blink1 object or NULL if no device with that id found
   */
  public static native Blink1 openById( int id );
  
  /**
   * Do a transaction with the Blink1.
   * Length of both byte arrays determines amount of data sent or received.
   *
   * @param cmd the blink1 command code
   * @param buf_send is byte array of command to send, may be null
   * @param buf_recv is byte array of any receive data, may be null
   * @returns blink1_command response code, -1 == fail 
   * FIXME: this does not work correctly.  Use higher-level functions instead.
   */
  public native synchronized int command(int cmd, byte[] buf_send, byte[] buf_recv);

  /**
   * Set blink(1) RGB color immediately.
   *
   * @param r red component 0-255
   * @param g green component 0-255
   * @param b blue component 0-255
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int setRGB(int r, int g, int b);

  /**
   * Set blink(1) RGB color immediately.
   *
   * @param c Color to set
   * @returns blink1_command response code, -1 == fail 
   */
  public int setRGB(Color c) {
    return setRGB( c.getRed(), c.getGreen(), c.getBlue() );
  }


  /**
   * Fade blink(1) to RGB color over fadeMillis milliseconds.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param r red component 0-255
   * @param g green component 0-255
   * @param b blue component 0-255
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int fadeToRGB(int fadeMillis, int r, int g, int b);
  
  /**
   * Fade blink(1) to RGB color over fadeMillis milliseconds.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param r red component 0-255
   * @param g green component 0-255
   * @param b blue component 0-255
   * @param ledn which LED to address (0=all)
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int fadeToRGB(int fadeMillis, int r, int g, int b, int ledn);
  
  /**
   * Fade blink(1) to RGB color over fadeMillis milliseconds.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param c Color to set
   * @returns blink1_command response code, -1 == fail 
   */
  public int fadeToRGB(int fadeMillis, Color c) {
    return fadeToRGB( fadeMillis, c.getRed(), c.getGreen(), c.getBlue() );
  }

  /**
   * Fade blink(1) to RGB color over fadeMillis milliseconds.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param c Color to set
   * @param ledn which LED to address (0=all)
   * @returns blink1_command response code, -1 == fail 
   */
  public int fadeToRGB(int fadeMillis, Color c, int ledn) {
    return fadeToRGB( fadeMillis, c.getRed(), c.getGreen(), c.getBlue(), ledn );
  }

  /**
   * Write a blink(1) light pattern entry.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param r red component 0-255
   * @param g green component 0-255
   * @param b blue component 0-255
   * @param pos entry position 0-patt_max
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int writePatternLine(int fadeMillis, int r, int g, int b, int pos);

  /**
   * Write a blink(1) light pattern entry.
   *
   * @param fadeMillis milliseconds to take to get to color
   * @param c Color to set
   * @param pos entry position 0-patt_max
   * @returns blink1_command response code, -1 == fail 
   */
  public int writePatternLine(int fadeMillis, Color c, int pos) {
    return writePatternLine(fadeMillis, c.getRed(), c.getGreen(), c.getBlue(), pos);
  }

  /**
   * Play a color pattern.
   *
   * @param play  true to play, false to stop
   * @param pos   starting position to play from, 0 = start
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int play( boolean play, int pos);

  /**
   * Enable or disable serverdown / servertickle mode
   * @param on true = turn on serverdown mode, false = turn it off
   * @param millis milliseconds until light pattern plays if not updated 
   * @returns blink1_command response code, -1 == fail 
   */
  public native synchronized int serverdown( boolean on, int millis);

  /** 
   * Get version of firmware code in blink(1) device.
   * @returns blink1 version number as int (e.g. v1.0 == 100, v2.0 = 200)
   */
  public native synchronized int getFirmwareVersion();


  //-------------------------------------------------------------------------
  // Utilty Class methods
  //-------------------------------------------------------------------------

  /**
   * one attempt at a degamma curve.
   * //FIXME: this is now in blink1-lib
   */
  public static final int log2lin( int n ) {
    //return  (int)(1.0* (n * 0.707 ));  // 1/sqrt(2)
    return (((1<<(n/32))-1) + ((1<<(n/32))*((n%32)+1)+15)/32);
  }

  /**
   * Utility: A simple delay
   */
  public static final void pause(int millis) {
    try {
        Thread.sleep(millis);
    } catch (Exception e) {
    }
  }

}
