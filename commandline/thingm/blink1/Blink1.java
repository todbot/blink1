

package thingm.blink1;

import java.io.*;
import java.util.*;
import java.util.regex.*;


public class Blink1 
{
  static {
    System.loadLibrary("Blink1");     // Load the library
  }  

  public static void usage() { 
    println(""+
"Usage: LinkM <cmd> [options]\n" +
            "");
  }

  //
  public static void main(String args[]) {
    
    if( args.length == 0 ) {
      usage();
    }
    
    // 
    Blink1 blink1 = new Blink1();

    //blink1.open(0,0,null,null);
    blink1.open();
      
    //while( //
    Random rand = new Random();
    for( int i=0; i<20; i++ ) {
      int r = rand.nextInt() & 0xFF;
      int g = rand.nextInt() & 0xFF;
      int b = rand.nextInt() & 0xFF;
      
      blink1.setRGB( r,g,b );
      blink1.pause( 1000 );
    }

    blink1.close();
    
  }


  /**
   * Open Blink1 dongle 
   * @param vid vendor id of device
   * @param pid product id of device
   * @param vstr vender string of device
   * @param pstr product string of device
   * Setting these to {0,0,null,null} will open first default device found
   */
  //public native void open(int vid, int pid, String vstr, String pstr)
  public native int open();

  /**
   * Do a transaction with the LinkM dongle
   * length of both byte arrays determines amount of data sent or received
   * @param cmd the linkm command code
   * @param buf_send is byte array of command to send, may be null
   * @param buf_recv is byte array of any receive data, may be null
   * @returns blink1_command response code, 0 == success, non-zero == fail
   */
  public native synchronized int command(int cmd, 
                                         byte[] buf_send, 
                                         byte[] buf_recv);


  /**
   *
   */
  public native synchronized int setRGB(int r, int g, int b);

  public native synchronized int fadeToRGB(int fadeMillis, int r, int g, int b);
  
 
  /**
   * Close LinkM dongle
   */
  public native void close();  

  //-------------------------------------------------------------------------
  // Utilty Class methods
  //-------------------------------------------------------------------------

  /**
   * Utility: A simple delay
   */
  static final public void pause( int millis ) {
      try { Thread.sleep(millis); } catch(Exception e) { }
  }

  static final public void println(String s) { 
    System.out.println(s);
  }
  static final public void print(String s) { 
    System.out.print(s);
  }

}