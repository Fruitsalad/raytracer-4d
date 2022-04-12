#pragma once

#include "util.hpp"

inline MultiMap<SDL_Keycode, class InputBool*> inputbools;

void handleInput();
