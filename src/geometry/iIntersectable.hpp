#pragma once

#include "math.hpp"

struct iIntersectable {
  virtual ~iIntersectable() = default;
  virtual Vec4 findIntersection(const Ray& ray) const = 0;
};
