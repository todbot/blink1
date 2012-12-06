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

#include <unistd.h>

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

static void
color(blink1_dev fd, char action, int R, int G, int B, int T, int step) {
  char buf[16];
  int rc;

  if (blink1_error(fd)) return;

  memset(buf, 0, sizeof(buf));

  if (R<0) R=0;
  if (G<0) G=0;
  if (B<0) B=0;
  if (T<0) T=0;
  if (step<0) step=0;

  if (R>255) R=255;
  if (G>255) G=255;
  if (B>255) B=255;
  if (T>65535) T=65535;
  if (step>15) step=15;
  
  buf[0] = 1;
  buf[1] = action;
  buf[2] = R; /* R */
  buf[3] = G; /* G */
  buf[4] = B; /* B */
  buf[5] = (T >>8);    /* time/cs high */
  buf[6] = (T & 0xff); /* time/cs low */
  buf[7] = step;
  buf[8] = 0;

  if (blink1_write(fd, buf, 9) < 0)
	  perror("write");
}

static void
play(blink1_dev fd, char action, int play, int step) {
  char buf[16];
  int rc;

  if (blink1_error(fd)) return;

  memset(buf, 0, sizeof(buf));

  buf[0] = 1;
  buf[1] = action;
  buf[2] = play;
  buf[3] = step;

  if (blink1_write(fd, buf, 9) < 0)
	  perror("write");
}

int
main(int argc, char *argv[]) {
  blink1_dev fd = -1;

  if (argc < 2) usage(NULL);

  while(++argv, --argc) {
    int rc = -1;
    int step = 0;
    int R = 0;
    int G = 0;
    int B = 0;
    int T = 0;
    char buf[16];

    memset(buf, 0, sizeof(buf));

    switch(**argv) {
    case '/': case '.':
      fd = blink1_openByPath(*argv);
      if (blink1_error(fd)) {
        perror(*argv);
        continue;
      }

      break;
    case '=':
      rc = sscanf(*argv, "=%d,%d,%d,%d", &R, &G, &B, &T);
      if (rc != 4) usage(*argv);
      color(fd, 'c', R, G, B, T, 0);
      break;
    case ':':
      rc = sscanf(*argv, ":%d,%d,%d", &R, &G, &B);
      if (rc != 3) usage(*argv);
      color(fd, 'n', R, G, B, 0, 0);
      break;
    case '@':
      rc = sscanf(*argv, "@%d:%d,%d,%d,%d", &step, &R, &G, &B, &T);
      if (rc != 5) usage(*argv);
      if ((step < 0) || step > 15) usage(*argv);
      color(fd, 'P', R, G, B, T, step);
      break;
    case '_':
      rc = sscanf(*argv, "_%d", &T);
      if (rc == 1) color(fd, 'c', 0, 0, 0, T, 0);
      else color(fd, 'n', 0, 0, 0, 0, 0);
      break;
    case '+':
      rc = sscanf(*argv, "+%d", &step);
      if (rc != 1) usage(*argv);
      if ((step < 0) || step > 15) usage(*argv);
      play(fd, 'p', 1, step);
      break;
    case '-':
      rc = sscanf(*argv, "-%d", &step);
      if (rc != 1) step = 0;
      if ((step < 0) || step > 15) step = 0;
      play(fd, 'p', 0, step);
      break;
    case '%':
      for(step = 0; step < 16; ++step) {
        color(fd, 'P', 0, 0, 0, 0, step);
      }
      break;
    default:
      usage(*argv);
    }
  }

  blink1_close(fd);
  return 0;
}
