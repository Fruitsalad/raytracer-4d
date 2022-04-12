#ifndef FRUIT_RAY_HPP
#define FRUIT_RAY_HPP

#include "Vec4.hpp"

struct Ray {
  Vec4 p; // The start coordinate of the ray.
  Vec4 d; // The (normalized) direction.
};

#endif //FRUIT_RAY_HPP
