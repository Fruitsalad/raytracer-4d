#pragma once

#include "math.hpp"

struct iColored {
  virtual ~iColored() = default;
  virtual Vec4 getLightColor() const = 0;
  virtual Vec4 getDarkColor() const = 0;
};
