//
// blink1-tiny-server -- a small cross-platform REST/JSON server for
//                       controlling a blink(1) device
//
// 
// Supported URLs:
//
//  localhost:8080/blink1/on
//  localhost:8080/blink1/off
//  localhost:8080/blink1/blink?rgb=%23ff0ff&time=1.0&count=3
//  localhost:8080/blink1/fadeToRGB?rgb=%23ff00ff&time=1.0
//  
//
//


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "mongoose.h"

#include "blink1-lib.h"

const char* blink1_server_version = "0.92";


//
static void get_qsvar(const struct mg_request_info *request_info,
                      const char *name, char *dst, size_t dst_len) {
  const char *qs = request_info->query_string;
  mg_get_var(qs, strlen(qs == NULL ? "" : qs), name, dst, dst_len);

}

// parse a comma-delimited string containing numbers (dec,hex) into a byte arr
// FIXME: copy of same func in blink1-tool.c
static int  hexread(uint8_t *buffer, char *string, int buflen)
{
    char    *s;
    int     pos = 0;

    memset(buffer,0,buflen);  // bzero() not defined on Win32?
    while((s = strtok(string, ", ")) != NULL && pos < buflen){
        string = NULL;
        buffer[pos++] = (char)strtol(s, NULL, 0);
    }
    return pos;
}

// given a string of hex color code ("#FF3322") or rgb triple ("255,0,0" or
// "0xff,0x23,0x00"), produce a parsed byte array 
static void parse_rgbstr(uint8_t* rgb, char* rgbstr)
{
    if( rgbstr != NULL && strlen(rgbstr) ) {
        if( rgbstr[0] == '#' ) { 
            uint32_t rgbval = strtoul(rgbstr+1,NULL,16); // FIXME: hack
            rgb[0] = ((rgbval >> 16) & 0xff);
            rgb[1] = ((rgbval >>  8) & 0xff);
            rgb[2] = ((rgbval >>  0) & 0xff);
        }
        else { 
            hexread(rgb, rgbstr, 3);
        }
    }
}

//
static void *callback(enum mg_event event,
                      struct mg_connection *conn,
                      const struct mg_request_info *request_info)
{
    char result[1000];  result[0] = 0;
    
    if (event == MG_NEW_REQUEST) {
        const char* uri = request_info->uri;
        
        char rgbstr[16];
        char timestr[8];
        char countstr[4];

        uint16_t millis = 100;
        uint8_t rgb[3];
        
        if( strstr(uri, "/blink1/off") == uri ) {
            sprintf(result, "blink1 off");
            blink1_device* dev = blink1_open();
            if( blink1_fadeToRGB( dev, millis, 0,0,0 ) == -1 ) {
                fprintf(stderr, "off: blink1 device error\n");
                sprintf(result, "%s\noff: couldn't find blink1", result);
            }
            blink1_close(dev);
        }
        else if( strstr(uri, "/blink1/on") == uri ) {
            sprintf(result, "blink1 on");
            blink1_device* dev = blink1_open();
            if( blink1_fadeToRGB( dev, millis, 255,255,255 ) == -1 ) {
                fprintf(stderr, "on: blink1 device error\n");
                sprintf(result, "%s\non: couldn't find blink1", result);
            }
            blink1_close(dev);
        }
        else if( strstr(uri, "/blink1/blink") == uri ) {
            get_qsvar(request_info, "rgb", rgbstr, sizeof(rgbstr));
            get_qsvar(request_info, "time", timestr, sizeof(timestr));
            get_qsvar(request_info, "count", countstr, sizeof(countstr));

            uint8_t cnt = strtol(countstr, NULL,0);

            millis = (strlen(timestr)) ? (1000*strtof(timestr,NULL)) : millis;
             
            rgb[0] = 100; rgb[1] = 100; rgb[2] = 100; // default

            parse_rgbstr( rgb, rgbstr);

            sprintf(result, "blink %d times, rgb:%d,%d,%d @ %d ms",
                    cnt, rgb[0],rgb[1],rgb[2], millis);

            blink1_device* dev = blink1_open();
            for( int i=0; i<cnt; i++ ) {
                blink1_fadeToRGB( dev, millis, rgb[0],rgb[1],rgb[2] );
                blink1_sleep( millis ); // fixme
                blink1_fadeToRGB( dev, millis, 0,0,0 );
                blink1_sleep( millis ); // fixme
            }
            blink1_close(dev);
        }
        else if( strstr(uri, "/blink1/fadeToRGB") == uri ) { 
            get_qsvar(request_info, "rgb", rgbstr, sizeof(rgbstr));
            get_qsvar(request_info, "time", timestr, sizeof(timestr));

            millis = (timestr) ? 1000 * strtof(timestr,NULL) : millis;

            parse_rgbstr(rgb, rgbstr);

            sprintf(result, "fadeToRGB: '%s' = %d,%d,%d @ %d msec", 
                    rgbstr, rgb[0],rgb[1],rgb[2], millis );
            
            blink1_device* dev = blink1_open();
            if( dev ) { 
                if( blink1_fadeToRGB( dev, millis, 
                                      rgb[0], rgb[1], rgb[2]) == -1 ) {
                    fprintf(stderr, "fadeToRGB: blink1 device error\n");
                    sprintf(result, "%s\nfadeToRGB: couldn't find blink1",
                            result);
                }
            }
            else {
                sprintf(result,"no blink1 : %s", result);
            }
            blink1_close(dev);
        }
        else if( strstr(uri, "/blink1/random") == uri) { 
            sprintf(result, "random not implemented yet");
        }
        else {
            sprintf(result, "unrecognized uri");
        }
        if( result != NULL ) { 
            // Echo requested URI back to the client
            mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/plain\r\n\r\n"
                      "{\n"
                      "\"uri\":  \"%s\",\n"
                      "\"result\":  \"%s\",\n"
                      "\"version\": \"%s\"\n"
                      "}\n", 
                      uri,
                      result,
                      blink1_server_version
                      );
            
            return "";  // Mark as processed
        }
        else { 
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

//
int main(void) {
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", NULL};

  char exit_flag = 0;
  ctx = mg_start(&callback, NULL, options);
  printf("blink1-server: running on port %s\n",
         mg_get_option(ctx, "listening_ports"));

  while (exit_flag == 0) {
      sleep(1);
  }

  mg_stop(ctx);

  return 0;
}
