#pragma once

#include "geometry.hpp"

inline List<Unique<iIntersectable>> world;

inline Vec4 background_color = {.25,0,.05,1};  // purple
inline Vec4 background_color_2 = {0,0,.3,1};  // dark blue
inline Vec4 fallback_light_color = {1,1,1,1};  // white
inline Vec4 fallback_dark_color = {.5,.5,.5,1};  // grey



// Note: Drawing the whole screen is done in MainScreen::render

// Trace a single ray.
inline Vec4 raytrace(const Ray& ray) {
  // Go through all the forms to find the nearest form the ray hits...
  double smallestDistance = Limits<double>::max();
  const iIntersectable* nearestForm = nullptr;
  
  for (const auto& form : world) {
    Vec4 intersection = form->findIntersection(ray);
    
    if (intersection != nowhere) {
      // Hit!
      double distance = (ray.p - intersection).calcLength();
      
      if (distance < smallestDistance) {
        smallestDistance = distance;
        nearestForm = form.get();
      }
    }
  }
  
  
  if (nearestForm == nullptr) {
    // We didn't hit anything...
    return cos(ray.d.w) * background_color + sin(ray.d.w) * background_color_2;
  }
  
  
  // Send back the color of the nearest form...
  
  // See if the nearest form implements iColored, and take those colors if yes
  Vec4 lightColor = fallback_light_color;
  Vec4 darkColor = fallback_dark_color;
  
  if (auto colors = dynamic_cast<const iColored*>(nearestForm)) {
    lightColor = colors->getLightColor();
    darkColor = colors->getDarkColor();
  }
  
  // Blend dark & light colors based upon distance...
  double distance = smallestDistance;
  
  double brightness = 1;
  if (distance != 0)
    brightness = 1 / (distance*distance/10);
  
  brightness = clamp(brightness, 0, 1);
  double darkness = 1-brightness;
  
  Vec4 color = lightColor * brightness + darkColor * darkness;
  color.w = 1;
  return color;
}
