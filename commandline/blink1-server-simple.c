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

//hid_device* dev=NULL;

const char* blink1_server_version = "0.90b";


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
    char result[1000];  result[0] = 0;
    
    if (event == MG_NEW_REQUEST) {
        const char* uri = request_info->uri;
        
        char rgbstr[16];
        char timestr[8];
        
        if( strstr(uri, "/blink1/fadeToRGB") == uri ) { 
            uint32_t rgbval = 0;
            uint16_t millis = 100;
            get_qsvar(request_info, "rgb", rgbstr, sizeof(rgbstr));
            get_qsvar(request_info, "time", timestr, sizeof(timestr));

            if( rgbstr != NULL ) {
                rgbval = strtoul(rgbstr+1,NULL,16); // FIXME: hack
                printf("rgbstr:%s\n",rgbstr);
            }
            if( timestr != NULL ) {
                float time = strtof(timestr,NULL);
                millis = time * 1000;
            }
            uint8_t r = ((rgbval >> 16) & 0xff);
            uint8_t g = ((rgbval >>  8) & 0xff);
            uint8_t b = ((rgbval >>  0) & 0xff);
            sprintf(result, "fadeToRGB: %s = %d,%d,%d @ %d msec", 
                    rgbstr, r,g,b, millis );
            
            hid_device* dev = blink1_open();
            if( dev ) { 
                if( blink1_fadeToRGB( dev, millis, r,g,b ) == -1 ) { 
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
