#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minifb/include/MiniFB.h"
#include "minifb/include/MiniFB_enums.h"

#define RESOLUTION_X 10
#define RESOLUTION_Y 10

typedef struct mfb_window mfb_window;
typedef struct { int x, y; } point;

unsigned int art_state[RESOLUTION_Y][RESOLUTION_X] = {};
mfb_update_state updated_state = MFB_STATE_OK;

bool mouse_is_pressed = false;
point window_size = {};

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

point mouse_position(mfb_window *window) {
  return (point){
    mfb_get_mouse_x(window),
    mfb_get_mouse_y(window)
  };
}

point translate(point p) {
  return (point){
    round(((float)RESOLUTION_X / window_size.x)*p.x),
    round(((float)RESOLUTION_Y / window_size.y)*p.y)
  };
}

void mouse_button(mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed) {
  if (!(mouse_is_pressed = is_pressed)) return;
  auto p = translate(mouse_position(window));
  art_state[p.y][p.x] = UINT32_MAX;
}

void mouse_move(mfb_window *window, int x, int y) {
  if (!mouse_is_pressed) return;
  auto p = translate((point){x, y});
  art_state[p.y][p.x] = UINT32_MAX;
}

unsigned int game_of_life(point p) {
  auto neighbours = 0;
  for (int y=-1; y<=p.y+1; y++) for (int x=-1; x<=p.x+1; x++) {
    if ((y < 0) || (x < 0) || (y >= RESOLUTION_Y) || (x >= RESOLUTION_X)) continue;
    neighbours += art_state[y][x] > 0 ? 1 : 0;
  }
  if (neighbours == 3) return UINT32_MAX;
  if (neighbours < 2 || neighbours > 3) return 0;
  return art_state[p.y][p.x] > 0 ? UINT32_MAX: 0;
}

int main(void) {
  mfb_window *window = mfb_open_ex("art", RESOLUTION_X, RESOLUTION_Y, MFB_WF_FULLSCREEN);
  if (!window) err("failed to open window");

  mfb_set_mouse_button_callback(window, mouse_button);
  mfb_set_mouse_move_callback(window, mouse_move);
  mfb_set_target_fps(10);

  window_size.x = mfb_get_window_width(window);
  window_size.y = mfb_get_window_height(window);

  do {
    if ((updated_state = mfb_update(window, art_state)) != MFB_STATE_OK) err(state_name(updated_state));
    for (int y=0; y<RESOLUTION_Y; y++) for (int x=0; x<=RESOLUTION_X; x++) art_state[y][x] = game_of_life((point){x, y});
  } while (mfb_wait_sync(window));

  return 0;
}
