#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "MiniFB.h"

#define XRES 1024
#define YRES 768
#define BPP  4
#define LEN  XRES * YRES * BPP

unsigned int frameA[LEN];
unsigned int frameB[LEN];
unsigned int output[LEN];

int main(void) {
  struct Window *window = mfb_open("Window", XRES, YRES);
  if (!window) {
    fprintf(stderr, "Failed to open window.");
  }

  for(int x = 0; x < XRES; x++) for(int y = 0; y < YRES; y++) {
    int i = y * XRES + x;
    frameA[i] = ~((x^y)-(x+y));
    frameB[i] = y * XRES * x;
  }
  
  unsigned char grade;
  bool up = true;
  while(true) {
    for(int y = 0; y < YRES; y++) for(int x = 0; x < XRES; x++) {
      int i = y * XRES + x;
      unsigned int step = (frameB[i] - frameA[i]) / 256;
      output[i] = frameA[i] + (step * grade);
    }

    UpdateState state = mfb_update(window, output);
    if (state != STATE_OK) {
      fprintf(stderr, "State is not 'ok'.");
      break;
    }

    ++grade;
  }

  return 0;
}
