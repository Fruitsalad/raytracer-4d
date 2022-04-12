#pragma once


#include <chrono>
#include "util.hpp"


struct FrameCounter {
  using clock = std::chrono::system_clock;
  using seconds = std::chrono::seconds;
  
  double framerate = 0;
  uint framesElapsed = 0;
  clock::time_point next_second = clock::now() + seconds(1);
  
  
  bool updateFramerate() {
    framesElapsed++;
    
    // Check if a second has passed yet...
    auto now = clock::now();
    
    if (now < next_second)
      return false;
    
    // Recalculate the framerate, reset the other variables...
    uint secondsElapsed = 0;
    
    while (now >= next_second) {
      next_second += seconds(1);
      secondsElapsed += 1;
    }
    
    framerate = framesElapsed / double(secondsElapsed);
    framesElapsed = 0;
    
    return true;
  }
};