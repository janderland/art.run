#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "minifb/include/MiniFB.h"
#include "minifb/include/MiniFB_enums.h"

#define XRES 10
#define YRES 10

typedef struct { int x, y; } point;

mfb_update_state updated_state = MFB_STATE_OK;
unsigned int window_state[YRES][XRES] = {};

point window_size = {0, 0};
bool mouse_is_pressed = false;

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

point translate(point p) {
  struct { float x, y; } factor = {
    (float)XRES / window_size.x,
    (float)YRES / window_size.y
  };

  return (point){
    factor.x*p.x,
    factor.y*p.y
  };
}

void mouse_button(struct mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed) {
  if (!(mouse_is_pressed = is_pressed)) return;
  point p;
  p.x = mfb_get_mouse_x(window);
  p.y = mfb_get_mouse_y(window);
  point actual = translate(p);
  window_state[actual.y][actual.x] = UINT32_MAX;
}

void mouse_move(struct mfb_window *window, int x, int y) {
  if (!mouse_is_pressed) return;
  point actual = translate((point){x, y});
  window_state[actual.y][actual.x] = UINT32_MAX;
}

int main(void) {
  struct mfb_window *window = mfb_open_ex("art", XRES, YRES, MFB_WF_FULLSCREEN);
  if (!window) err("failed to open window");

  mfb_set_mouse_button_callback(window, mouse_button);
  mfb_set_mouse_move_callback(window, mouse_move);

  window_size.x = mfb_get_window_width(window);
  window_size.y = mfb_get_window_height(window);

  do {
    if ((updated_state = mfb_update(window, window_state)) != MFB_STATE_OK) err(state_name(updated_state));
  } while (mfb_wait_sync(window));

  return 0;
}
