#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "minifb/include/MiniFB.h"
#include "minifb/include/MiniFB_enums.h"

#define XRES 10
#define YRES 10
#define LEN YRES * XRES

mfb_update_state state = MFB_STATE_OK;
unsigned int frame[YRES][XRES] = {};

void err(const char *msg) {
  fprintf(stderr, "ERR! %s\n", msg);
  exit(1);
}

const char* state_name(mfb_update_state state) {
  switch (state) {
    case MFB_STATE_OK:
      return "OK";
    case MFB_STATE_EXIT:
      return "EXIT";
    case MFB_STATE_INVALID_WINDOW:
      return "INVALID WINDOW";
    case MFB_STATE_INVALID_BUFFER:
      return "INVALID BUFFER";
    case MFB_STATE_INTERNAL_ERROR:
      return "INTERNAL ERROR";
    default:
      return "<INVALID>";
  }
}

int main(void) {
  struct mfb_window *window = mfb_open_ex("art", XRES, YRES, MFB_WF_FULLSCREEN);
  if (!window) err("failed to open window");

  do {
    if ((state = mfb_update(window, frame)) != MFB_STATE_OK) err(state_name(state));
  } while (mfb_wait_sync(window));

  return 0;
}
