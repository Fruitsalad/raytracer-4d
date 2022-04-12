#include <SDL2/SDL.h>

#include "InputBool.hpp"
#include "handleInput.hpp"
#include "realMain.hpp"
#include "main/inputs.hpp"


static void activateKey(SDL_Keysym key);
static void deactivateKey(SDL_Keysym key);
static void onMouseMove(SDL_Event& event);
static void onMouseButtonDown(SDL_Event& event);
static void onScroll(SDL_Event& event);
static void onWindowEvent(SDL_Event& event);

static void placeholder_initScalars_();
static void placeholder_handleMouseMove_(int xrel, int yrel);
static void placeholder_handleScroll_(int scroll);



void handleInput() {
  SDL_Event event;
  
  placeholder_initScalars_();
  
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      please_stop = true;
    
    else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
      activateKey(event.key.keysym);
    
    else if (event.type == SDL_KEYUP)
      deactivateKey(event.key.keysym);
    
    else if (event.type == SDL_MOUSEBUTTONDOWN)
      onMouseButtonDown(event);
    
    else if (event.type == SDL_MOUSEMOTION)
      onMouseMove(event);
    
    else if (event.type == SDL_MOUSEWHEEL)
      onScroll(event);
    
    else if (event.type == SDL_WINDOWEVENT)
      onWindowEvent(event);
  }
}


static void activateKey(SDL_Keysym key) {
  auto range = inputbools.equal_range(key.sym);
  
  for (auto iter = range.first; iter != range.second; iter++)
    iter->second->activate();
}


static void deactivateKey(SDL_Keysym key) {
  auto range = inputbools.equal_range(key.sym);
  
  for (auto iter = range.first; iter != range.second; iter++)
    iter->second->deactivate();
}


static void onMouseMove(SDL_Event& event) {
  const auto& e = event.motion;
  
  if (isControllingCamera) {
    uint center_x = windowWidth / 2;
    uint center_y = windowHeight / 2;
    
    // This is the best way to ignore the motion events that
    // SDL_WarpMouseInWindow generates that I could think of.
    //if (e.x != center_x || e.y != center_y) {
      placeholder_handleMouseMove_(e.xrel, e.yrel);
      // SDL_WarpMouseInWindow(window, center_x, center_y);
    //}
  }
}


static void onMouseButtonDown(SDL_Event& event) {
  if (event.button.button == SDL_BUTTON_RIGHT) {
    isControllingCamera = !isControllingCamera;
    //SDL_ShowCursor(!isControllingCamera);
    
    // Unfortunately we can't do it the easy way, because relative mouse mode
    // doesn't work very well with breakpoints on Linux Mint.
    SDL_SetRelativeMouseMode((SDL_bool) isControllingCamera);
  }
}


static void onScroll(SDL_Event& event) {
  placeholder_handleScroll_(event.wheel.y);
}


static void onWindowEvent(SDL_Event& event) {
  if (event.window.event == SDL_WINDOWEVENT_CLOSE)
    please_stop = true;
}


static void placeholder_initScalars_() {
  turn_horizontally.value = 0;
  turn_vertically.value = 0;
  turn_4D.value = 0;
}


static void placeholder_handleMouseMove_(int xrel, int yrel) {
  turn_horizontally.value += xrel;
  turn_vertically.value += yrel;
}


static void placeholder_handleScroll_(int scroll) {
  turn_4D.value += scroll;
}
