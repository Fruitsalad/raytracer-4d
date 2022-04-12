#pragma once

#include "math.hpp"
#include "nowhere.hpp"
#include "iIntersectable.hpp"
#include "iColored.hpp"



struct Hypersphere : public iIntersectable, public iColored {
  Vec4 center;
  double radius;
  
  Vec4 lightColor = {1, 1, 1, 1};
  Vec4 darkColor = {.2, .2, .2, 1};
  
  
  // Returns the first intersection or `nowhere` if there is no intersection.
  Vec4 findIntersection(const Ray& ray) const override {
    // This algorithm was taken from:
    //  https://fiftylinesofcode.com/ray-sphere-intersection/
    
    Vec4 O_C = ray.p - center;
    double p = ray.d.dot(O_C);
    double q = O_C.dot(O_C) - radius*radius;
    
    double discriminant = p*p - q;
    if (discriminant < 0.0)
      return nowhere;
    
    double distance = -p - sqrt(discriminant);
    
    if (distance < 0.0)
      return nowhere;
    
    return ray.p + ray.d * distance;
  }
  
  Vec4 getLightColor() const override {
    return lightColor;
  }
  
  Vec4 getDarkColor() const override {
    return darkColor;
  }
};
