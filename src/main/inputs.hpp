#pragma once

#include <SDL2/SDL.h>

#include "io/InputBool.hpp"
#include "io/InputScalar.hpp"


// I liked this better before I had to turn all the InputBools into pointers :(

inline InputBool* go_left = createInputBoolFromKeycode("go left", SDLK_a);
inline InputBool* go_right = createInputBoolFromKeycode("go right", SDLK_d);
inline InputBool* go_Y_plus = createInputBoolFromKeycode("go forward", SDLK_w);
inline InputBool* go_Y_minus = createInputBoolFromKeycode("go backward", SDLK_s);
inline InputBool* go_up = createInputBoolFromKeycode("go up", SDLK_LSHIFT);
inline InputBool* go_down = createInputBoolFromKeycode("go down", SDLK_LCTRL);
inline InputBool* go_W_plus = createInputBoolFromKeycode("go W plus", SDLK_r);
inline InputBool* go_W_minus = createInputBoolFromKeycode("go W minus", SDLK_f);
inline InputBool* toggle_controls_display = createInputBoolFromKeycode("toggle controls display", SDLK_F1);
inline InputBool* toggle_status_display = createInputBoolFromKeycode("toggle status display", SDLK_F3);
inline InputBool* toggle_smaller_view = createInputBoolFromKeycode("toggle smaller view", SDLK_F4);
inline InputBool* turn_4D_up = createInputBoolFromKeycode("turn 4D up", SDLK_o);
inline InputBool* turn_4D_down = createInputBoolFromKeycode("turn 4D down", SDLK_p);

inline InputScalar turn_horizontally;
inline InputScalar turn_vertically;
inline InputScalar turn_4D;


//inline InputScalar zoom_change;
