#pragma once

#include "Vec2.hpp"


template<class CustomFunction>
void forEachPixelInLineSegment(
    Vec2i start, Vec2i end, CustomFunction doSomething
) {
  // Bresenham's line algorithm
  
  // For when diff.x >= diff.y, so we always need to move at most 1 pixel on the
  // y-axis each time we move a pixel to the right on the x-axis.
  auto bresenham_for_gentle_slopes = [](
      int x, int y, int dx, int dy, int x_step, int y_step, auto doSomething2
  ) {
    doSomething2(x, y);
    int d = 2*dy - dx;
  
    for (int i = 0; i < dx; i++) {
      x += x_step;
    
      if (d < 0)
        d += 2*dy;
      else {
        y += y_step;
        d += 2*dy - 2*dx;
      }
    
      doSomething2(x, y);
    }
  };
  
  // For when diff.x < diff.y, so we will sometimes need to move more than
  // 1 pixel on the y-axis for each step to the right.
  auto bresenham_for_steep_slopes = [&](
      int x, int y, int dx, int dy, int x_step, int y_step, auto doSomething2
  ) {
    // The trick is that it's the same as the gentle slope version but with
    // x and y flipped. I think this solution I'm using is bordering on being
    // overcomplicated in the way it tries to reduce the amount of code but...
    // I already wrote it by now.
    auto flip_xy = [&](int x, int y) { doSomething2(y,x); };
    bresenham_for_gentle_slopes(y, x, dy, dx, y_step, x_step, flip_xy);
  };
  
  
  const int dx = abs(end.x - start.x);
  const int dy = abs(end.y - start.y);
  const int x_step = (start.x <= end.x ? 1 : -1);
  const int y_step = (start.y <= end.y ? 1 : -1);
  const int x = start.x;
  const int y = start.y;
  
  if (dx > dy)
    bresenham_for_gentle_slopes(x, y, dx, dy, x_step, y_step, doSomething);
  else
    bresenham_for_steep_slopes(x, y, dx, dy, x_step, y_step, doSomething);
}



template<class CustomFunction>
void forEachPixelInCircle(
    Vec2i center, int radius, CustomFunction doSomething
) {
  // Bresenham's circle algorithm
  auto doSomethingForEachOctant = [&](int x, int y) {
    doSomething(center.x + x, center.y + y);
    doSomething(center.x - x, center.y + y);
    doSomething(center.x + x, center.y - y);
    doSomething(center.x - x, center.y - y);
    doSomething(center.x + y, center.y + x);
    doSomething(center.x - y, center.y + x);
    doSomething(center.x + y, center.y - x);
    doSomething(center.x - y, center.y - x);
  };
  
  doSomething(center.x, center.y + radius);
  doSomething(center.x + radius, center.y);
  doSomething(center.x, center.y - radius);
  doSomething(center.x - radius, center.y);
  
  int x = 0;
  int y = radius;
  int d = 3 - 2*radius;
  
  while (y >= x) {
    x++;
    
    if (d > 0) {
      y--;
      d = d + 4*(x-y) + 10;
    }
    else d = d + 4*x + 6;
    
    doSomethingForEachOctant(x, y);
  }
}
