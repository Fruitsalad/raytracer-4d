#pragma once

#include "util.hpp"
#include "handleInput.hpp"


class InputBool {
  // I was supposed to make this more neat eventually,
  // but the underlying mechanisms are relatively crude.
public:
  uint sdl_keycode = 0;
  bool is_active = false;
  
  Function<void()> onActivate = nullptr;
  Function<void()> onDeactivate = nullptr;
  
  bool get() const {
    return is_active;
  }
  
  operator bool() const { return get(); }
  
  void activate() {
    is_active = true;
    if (onActivate)
      onActivate();
  }
  
  void deactivate() {
    is_active = false;
    if (onDeactivate)
      onDeactivate();
  }
};



inline InputBool* createInputBoolFromKeycode(String$& name, uint sdl_keycode) {
  InputBool* input = new InputBool();
  input->sdl_keycode = sdl_keycode;
  inputbools.emplace(sdl_keycode, input);
  return input;
}
