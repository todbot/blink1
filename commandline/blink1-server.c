//
//
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
    char result[1000] = "-";

    if (event == MG_NEW_REQUEST) {
        const char* uri = request_info->uri;

        // get "id" query arg
        char cmd[32];
        char rgbstr[16];
        char msstr[8];
        get_qsvar(request_info, "cmd", cmd, sizeof(cmd));
      
        if( ! cmd[0] ) { // cmd is empy 
            ///
        }
        else {
            if( strcasecmp(cmd, "setrgb")==0 ) { 
                get_qsvar(request_info, "rgb", rgbstr, sizeof(rgbstr));
                get_qsvar(request_info, "millis", msstr, sizeof(msstr));
                uint32_t rgbval = strtoul(rgbstr,NULL,16);
                uint16_t millis = strtoul(msstr,NULL,0);
                uint8_t r = ((rgbval >> 16) & 0xff);
                uint8_t g = ((rgbval >>  8) & 0xff);
                uint8_t b = ((rgbval >>  0) & 0xff);
                sprintf(result, "setrgb: %x = %d,%d,%d @ %d", 
                        rgbval, r,g,b, millis );
                
                if( dev==NULL ) {    // first run
                    dev = blink1_open();
                }
                if( dev == NULL ) {  // no blink1 found
                    sprintf(result,"no blink1 : %s", result);
                } else {
                    if( blink1_fadeToRGB( dev, 100, r,g,b ) != 0 ) { 
                        sprintf(result, "fadeToRGB: couldn't find blink1");
                        blink1_close(dev);
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

  char exit_flag = 0;
  ctx = mg_start(&callback, NULL, options);
  printf("blink1-server: running on port %s\n",
         mg_get_option(ctx, "listening_ports"));

  //getchar();  // Wait until user hits "enter"
  while (exit_flag == 0) {
      sleep(1);
  }

  mg_stop(ctx);

  return 0;
}
