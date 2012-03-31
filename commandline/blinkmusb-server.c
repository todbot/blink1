//
//
//
//
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "mongoose.h"

#include "blinkmusb-lib.h"

usbDevice_t *dev=NULL;

//
static void get_qsvar(const struct mg_request_info *request_info,
                      const char *name, char *dst, size_t dst_len) {
  const char *qs = request_info->query_string;
  mg_get_var(qs, strlen(qs == NULL ? "" : qs), name, dst, dst_len);

}

//
static void *callback(enum mg_event event,
                      struct mg_connection *conn,
                      const struct mg_request_info *request_info)
{
    char result[1000] = "none";

    if (event == MG_NEW_REQUEST) {
        const char* uri = request_info->uri;

        // get "id" query arg
        char cmd[32];
        get_qsvar(request_info, "cmd", cmd, sizeof(cmd));
      
        if( ! cmd[0] ) { // cmd is empy 
            ///
        }
        else {
            if( strcasecmp(cmd, "setrgb")==0 ) { 
                char rgb[32];
                get_qsvar(request_info, "rgb", rgb, sizeof(rgb));
                uint32_t rgbval = strtoul(rgb,NULL,16);
                uint8_t r = ((rgbval >> 16) & 0xff);
                uint8_t g = ((rgbval >>  8) & 0xff);
                uint8_t b = ((rgbval >>  0) & 0xff);
                sprintf(result, "setrgb: %x = %d,%d,%d", 
                        rgbval, r,g,b );
                
                if( dev==NULL ) {    // first run
                    dev = blinkmusb_open();
                }
                if( dev == NULL ) {  // no blinkmusb found
                    sprintf(result,"no blinkmusb");
                } else {
                    if( blinkmusb_fadeToRGB( dev, 100, r,g,b ) != 0 ) { 
                        sprintf(result, "fadeToRGB: couldn't find blinkmusb");
                        blinkmusb_close(dev);
                    }
                }
            }
            else if( strcasecmp(cmd, "random") == 0) { 

            }
        
        }

        // Echo requested URI back to the client
        mg_printf(conn, "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/plain\r\n\r\n"
                  "-- hello there! message received --\n"
                  "uri:%s\n"
                  "cmd:%s\n"
                  "result:%s\n"
                  "\n", 
                  uri,
                  cmd,
                  result
                  );
 
        return "";  // Mark as processed

    } else {
        return NULL;
    }
}

//
int main(void) {
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", NULL};

  ctx = mg_start(&callback, NULL, options);
  printf("blinkmusb-server: running on port %s\n",
         mg_get_option(ctx, "listening_ports"));
  getchar();  // Wait until user hits "enter"

  mg_stop(ctx);

  return 0;
}
