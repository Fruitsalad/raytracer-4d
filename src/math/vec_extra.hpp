#pragma once


#include "Vec4.hpp"



/** Creates a direction vector for the given angles. */
inline Vec4 dir_vec(double yaw, double pitch, double wy_rotation) {
  Vec4 direction;
  direction.x = cos(yaw) * cos(pitch);
  direction.y = sin(yaw) * cos(wy_rotation) * cos(pitch);
  direction.z = sin(pitch);
  direction.w = -sin(yaw) * sin(wy_rotation) * cos(pitch);
  return normalize(direction);
}



#ifdef ENABLE_DOCTEST
#include <doctest/doctest.h>
TEST_CASE("dir_vec test") {
  
  auto approxEquals = [](Vec4 a, Vec4 b) -> bool {
    Vec4 diff = a - b;
    return diff.calcLength() < 0.000001;
  };
  
  double hpi = pi / 2.0;
  
  CHECK(dir_vec(0, 0, 0) == Vec4(1, 0, 0, 0));
  CHECK(approxEquals(dir_vec(hpi, 0, 0), Vec4(0, 1, 0, 0)));
  CHECK(approxEquals(dir_vec(0, hpi, 0), Vec4(0, 0, 1, 0)));
  CHECK(approxEquals(dir_vec(hpi, 0, hpi), Vec4(0, 0, 0, 1)));
}
#endif
