#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minifb/include/MiniFB.h"
#include "minifb/include/MiniFB_enums.h"

#define RESOLUTION_X 10
#define RESOLUTION_Y 10
#define FRAME_RATE 10

typedef struct mfb_window mfb_window;
typedef struct { int x, y; } point;

// double buffering; which_buffer controls which buffer is used as the
// current frame and which is used as the next frame
unsigned int _frame_buffer[2][RESOLUTION_Y][RESOLUTION_X] = {};
#define next_frame _frame_buffer[which_buffer ? 1 : 0]
#define frame _frame_buffer[which_buffer ? 0 : 1]
bool which_buffer = false;

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

// the frame may be stretched to fit the screen, so mouse positions
// must be translated to their position within the frame
point translate_position(point p) {
  return (point){
    ((float)RESOLUTION_X / window_size.x)*p.x,
    ((float)RESOLUTION_Y / window_size.y)*p.y
  };
}

// callback for "mouse button" event
void mouse_button(mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed) {
  // if a mouse button is presed, activate the point it's hovering over
  if (!(mouse_is_pressed = is_pressed)) return;
  auto p = translate_position(mouse_position(window));
  frame[p.y][p.x] = UINT32_MAX;
}

// callback for "mouse move" event
void mouse_move(mfb_window *window, int x, int y) {
  // if a mouse button is presed, activate the point it's hovering over
  if (!mouse_is_pressed) return;
  auto p = translate_position((point){x, y});
  frame[p.y][p.x] = UINT32_MAX;
}

// calculate the next state for a particular point in the frame according to "game of life" rules
unsigned int game_of_life(point p) {
  auto neighbour_count = 0;

  // for the neighbours around the given point...
  for (int y=p.y-1; y<=p.y+1; y++) for (int x=p.x-1; x<=p.x+1; x++) {

    // if this neighbour is off the screen, skip it
    if (y < 0 || x < 0 || y >= RESOLUTION_Y || x >= RESOLUTION_X) continue;

    // don't count the given point as a neighbour
    if (y == p.y && x == p.x) continue;

    // if the neighbour is active, count it
    neighbour_count += frame[y][x] > 0 ? 1 : 0;
  }

  // a point with 3 neighbours should always be activated
  if (neighbour_count == 3) return UINT32_MAX;

  // a point with <2 or >3 neighbours always dies
  if (neighbour_count < 2 || neighbour_count > 3) return 0;

  // a point with exactly 2 neighbours keeps it's current state
  return frame[p.y][p.x] > 0 ? UINT32_MAX: 0;
}

void render_next_frame() {
  // for each point, calculate it's value in the next frame
  for (int y=0; y<RESOLUTION_Y; y++) for (int x=0; x<RESOLUTION_X; x++) next_frame[y][x] = game_of_life((point){x, y});

  // flip which frame is active; next frame becomes current frame and vice versa
  which_buffer = !which_buffer;
}

int main(void) {
  mfb_window *window = mfb_open_ex("art", RESOLUTION_X, RESOLUTION_Y, MFB_WF_FULLSCREEN);
  if (!window) err("failed to open window");

  mfb_set_mouse_button_callback(window, mouse_button);
  mfb_set_mouse_move_callback(window, mouse_move);
  mfb_set_target_fps(FRAME_RATE);

  window_size.x = mfb_get_window_width(window);
  window_size.y = mfb_get_window_height(window);

  do {
    // display the current frame; exit on error
    if ((updated_state = mfb_update(window, frame)) != MFB_STATE_OK) err(state_name(updated_state));

    // if the mouse is not pressed, render the next frame
    if (!mouse_is_pressed) render_next_frame();

  // wait until it's time to display the next frame
  } while (mfb_wait_sync(window));

  return 0;
}
