/**
 * An example of subclassing NanoHTTPD to make a custom HTTP server.
 *
 *
 *
 *  Turn blin1 to color #FF00FF (but continue playing)
 *  ?cmd=fadeToRGB&rgb=#FF00FF
 *
 *  Turn blink1 to color 255,255,255 (and stop playing):
 *  ?cmd=on
 *
 *  Turn blink1 to color 0,0,0 (and stop playing):
 *  ?cmd=off
 *
 *  Play a color pattern:
 *  ?cmd=play&pattern=name,3,#ff00ff,0.1,#0033ff,20,#ff0000,10
 *
 *  Stop an active pattern:
 *  ?cmd=stop&pattern=name
 * 
 *  List currently active patterns:
 *  ?cmd=list  
 *
 *
 */

import java.io.*;
import java.util.*;
import java.awt.Color;

import org.json.simple.JSONObject;

import thingm.blink1.Blink1;



public class Blink1NanoHTTPD extends NanoHTTPD
{
  File rootDir;
  Blink1 blink1;
  PatternPlayer player;

  public Blink1NanoHTTPD( int port, File docroot) throws IOException
  {
    super( port, docroot );
    rootDir = docroot;
  }
  
  //
  public void setPatternPlayer(PatternPlayer aplayer)
  {
    player = aplayer;
    blink1 = player.blink1;
  }

  // FIXME: return value
  public void blink1FadeToColor( int timems, Color rgb )
  {
    int r = rgb.getRed();
    int g = rgb.getGreen();
    int b = rgb.getBlue();
    
    blink1.open();
    blink1.fadeToRGB( timems, r,g,b );
    blink1.close();
  }

  //
  public Response serve( String uri, String method, Properties header, Properties parms, Properties files )
  {
    System.out.println( method + " '" + uri + "' "+parms );
    String msg = "none";

    //
    if( uri.startsWith("/blink1") ) { 

      JSONObject json = new JSONObject();

      // common arguments
      String cmd     = parms.getProperty("cmd","");
      String rgbstr  = parms.getProperty("rgb","");
      String timestr = parms.getProperty("time","");
      String statestr= parms.getProperty("state","");
      
      Color rgb = Color.black;
      float time = (float)0.50; 
      try { rgb = Color.decode( rgbstr ); } catch(Exception e) { };
      try { time = Float.parseFloat( timestr ); } catch(Exception e) { };
      int timems = (int)(time*1000);
      boolean state = statestr.equals("true");
      
      //cmd = cmd.toLowerCase();
      msg = cmd;
      if( cmd.equals("") ) { 
        msg = "param 'cmd' must be specified";
      } 
      else if( cmd.equalsIgnoreCase("enumerate") ) { 
        msg += " re-enumarting...";
        blink1.open();
        //blink1.version();
        blink1.close();
        int rc = blink1.enumerate();
        msg += "rc:"+rc;
      }
      else if( cmd.equalsIgnoreCase("fadeToRGB" ) ) { 
        msg += rgb+","+time+" sec";
        blink1FadeToColor( timems, rgb );
      }
      else if( cmd.equalsIgnoreCase("on") ) { 
        rgb = Color.WHITE;
        blink1FadeToColor( timems, rgb );
      }
      else if( cmd.equalsIgnoreCase("off") ) { 
        rgb = Color.BLACK;
        blink1FadeToColor( timems, rgb );
      }
      else if( cmd.equals("writePatternLine") ) { 
        
      }
      else if( cmd.equalsIgnoreCase("listBlink1s") ) {
        String serials[] = blink1.getDeviceSerials();
        for( int i=0; i<serials.length; i++ ) {
          System.out.println( "serial: "+serials[i]);
        }
        json.put("serials", new ArrayList(Arrays.asList(serials)));
      }
      else if( cmd.equalsIgnoreCase("servertickle") ||
               cmd.equalsIgnoreCase("serverdown") ) { 
        msg += "- state:"+state;
        blink1.open();
        blink1.serverdown( state, timems );
        blink1.close();
      }
      else if( cmd.equalsIgnoreCase("addPattern") ) {
        String patternstr = parms.getProperty("pattern");
        Pattern patt = new Pattern(player, patternstr );

        // add pattern to player
        player.addPattern( "", patt );

        msg+= " added pattern: "+patt;
        
      }
      else if( cmd.equalsIgnoreCase("playPattern") ) {
        String pattName = parms.getProperty("patternName");

        // add pattern to player
        boolean rc = player.playPattern( "", pattName );

        msg+= " play pattern: "+pattName+ ":"+rc;
        
      }
      else if( cmd.equalsIgnoreCase("listPatterns") ) {
        player.listPatterns();  // FIXME:
      }

      json.put( "cmd", cmd );
      json.put( "rgb", rgb.toString() );
      json.put( "time", new Float(time) );
      json.put( "state", new Boolean(state) );

      json.put( "status", msg );

      System.out.println( "status:" +json );
      return new NanoHTTPD.Response( HTTP_OK, MIME_PLAINTEXT, json.toString()+"\n" );
    }

    //
    if( uri.startsWith("/todbot") ) { 
      msg = "<html><body><h1>Hello server</h1>\n";
      if ( parms.getProperty("username") == null )
        msg +=
          "<form action='?' method='get'>\n" +
          "  <p>Your name: <input type='text' name='username'></p>\n" +
          "</form>\n";
      else
        msg += "<p>Hello, " + parms.getProperty("username") + "!</p>";
      
      msg += "</body></html>\n";
      return new NanoHTTPD.Response( HTTP_OK, MIME_HTML, msg );
    }
    return serveFile( uri, header, rootDir, true );
    //return super.serve(uri,method,header, parms, files);
  }

}