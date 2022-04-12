#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "no_deduce.hpp"


/** Returns `number` if `min <= number <= max`, otherwise returns `min` if
 * `number < min` or `max` if `number > max` */
template<class Scalar>
Scalar clamp(Scalar number, no_deduce_<Scalar> min, no_deduce_<Scalar> max) {
  // Clarifying on the use of no_deduce_: Only the number argument should be
  // used by the compiler to try to automatically derive the type of Scalar.
  // no_deduce_ ensures that the type of the min and the max argument aren't
  // a factor in the type deduction process.
  return std::min(std::max(number, min), max);
}



/** `lerp` stands for "linear interpolation" (I didn't come up with the
 * abbreviation)
 *
 * This returns a value between `start` and `end`, with the value being closer
 * to `end` if `interpolationFactor` is closer to 1 and the value being closer
 * to `start` if `interpolationFactor` is closer to 0. */
template<class T>
T lerp(T start, no_deduce_<T> end, double interpolationFactor) {
  return (1-interpolationFactor) * start + interpolationFactor * end;
}


/** A main loop function that works with Emscripten.
 * @param doSomething: A function that must return a boolean.
 *                     Return false to stop the loop. */
template<class CustomFunction>
void mainLoop(CustomFunction doSomething) {
  #ifdef __EMSCRIPTEN__
  // Emscripten implementation
  
  const static auto loopIteration = [](void* doSomething_void) {
    // Convert from void* to CustomFunction...
    CustomFunction& doSomething =
        *reinterpret_cast<CustomFunction*>(doSomething_void);
  
    // The actual meat of the loop...
    if (!doSomething())
      emscripten_cancel_main_loop();
  };
  
  emscripten_set_main_loop_arg(loopIteration, &doSomething, 0, true);
  #else
  // Normal implementation
  while (doSomething())
    ;
  #endif
}
