/* 
 * blinkraw.c
 *
 * fgmr 2012-09-22
 *
 * playing with the hidraw interface to blink(1)
 * 
 * Thank you Alan Ott for
 * http://lxr.free-electrons.com/source/samples/hidraw/hid-example.c
 */

#include <linux/types.h>
#include <linux/input.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>

#include "blink1raw-lib.h"

static void
usage(const char* hunh) {
  if (NULL != hunh) {
    fprintf(stderr, "Can't understand %s\n", hunh);
  }

  fputs(
          "Usage: blinkraw {arg, ...}\n"
          "  /dev/whatever  -- open device\n"
          "  ./whatever     -- open device\n"
          "  =R,G,B,t       -- fade to color\n"
          "  :R,G,B         -- set color (now)\n"
          "  @step:R,G,B,t  -- set step\n"
          "  +step          -- start playing at step\n"
          "  -[step]        -- stop playing at step (default zero)\n"
          "  %              -- clear all steps\n"
          "  _              -- turn off\n"
          "  _t             -- fade off\n"
          "\n"
          "       step is on [0,15]\n"
          "       R, G, B are on [0, 255]\n"
          "       t is time in centiseconds\n"
          "\n"
          "    Arguments are applied in order.  A new device, which is\n"
          "    a valid blink(1) device, will become the new target.\n"
          "\n"
          "    Example:\n"
          "    # blinkraw /dev/hidraw* % =255,0,0,100\n",
	  stderr
          );
  exit(1);
}

int
main(int argc, char *argv[]) {
  blink1_dev fd = -1;

  if (argc < 2) usage(NULL);

  while(++argv, --argc) {
    int rc = 0;
    uint8_t step = 0;
    uint8_t R = 0;
    uint8_t G = 0;
    uint8_t B = 0;
    uint16_t T = 0;
    char buf[16];

    memset(buf, 0, sizeof(buf));

    switch(**argv) {
    case '/': case '.':
      fd = blink1_openByPath(*argv);
      if (blink1_error(fd)) {
        perror(*argv);
        continue;
      }
      rc = fd;

      break;
    case '=':
      rc = sscanf(*argv, "=%hhu,%hhu,%hhu,%hu", &R, &G, &B, &T);
      if (rc != 4) usage(*argv);
      rc = blink1_fadeToRGB(fd, T, R, G, B);
      break;
    case ':':
      rc = sscanf(*argv, ":%hhu,%hhu,%hhu", &R, &G, &B);
      if (rc != 3) usage(*argv);
      rc = blink1_setRGB(fd, R, G, B);
      break;
    case '@':
      rc = sscanf(*argv, "@%hhu:%hhu,%hhu,%hhu,%hu", &step, &R, &G, &B, &T);
      if (rc != 5) usage(*argv);
      if ((step < 0) || step > 15) usage(*argv);
      rc = blink1_writePatternLine(fd, T, R, G, B, step);
      break;
    case '_':
      rc = sscanf(*argv, "_%hu", &T);
      if (rc == 1) rc = blink1_fadeToRGB(fd, T, 0, 0, 0);
      else rc = blink1_setRGB(fd, 0, 0, 0);
      break;
    case '+':
      rc = sscanf(*argv, "+%hhu", &step);
      if (rc != 1) usage(*argv);
      if ((step < 0) || step > 15) usage(*argv);
      rc = blink1_play(fd, 1, step);
      break;
    case '-':
      rc = sscanf(*argv, "-%hhu", &step);
      if (rc != 1) step = 0;
      if ((step < 0) || step > 15) step = 0;
      rc = blink1_play(fd, 0, step);
      break;
    case '%':
      for(step = 0; step < 16; ++step)
        rc |= blink1_writePatternLine(fd, 0, 0, 0, 0, step);
      break;
    default:
      usage(*argv);
    }
    if (rc < 0)
      perror("blink1 command");
  }

  blink1_close(fd);
  return 0;
}
