#pragma once


#include "math.hpp"
#include "nowhere.hpp"


// Test class to see if I got things right
struct AlignedCube {
  
  Vec3 min; // The first corner
  Vec3 max; // The opposite corner
  // Together these define the aligned hypercuboid.
  
  Vec4 lightColor = {1,1,1,1};
  Vec4 darkColor = {.2,.2,.2,1};
  
  
  // Returns the first intersection or `nowhere` if there is no intersection.
  Vec4 findIntersection(const Ray& ray) const {
    
    // This algorithm was taken from:
    //   https://www.scratchapixel.com/lessons/3d-basic-rendering
    //   /minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
    // which in turn was taken from:
    //   An Efficient and Robust Ray-Box Intersection Algorithm,
    //   Amy Williams et al. 2004.
    //
    // Whenever I talk about "steps", I'm mentally stepping down the path of the
    // ray. We start at ray.p and travel some multiple of ray.d, with each ray.d
    // being one step.
    
    // I will use `t` for "the amount of steps to ..."
    
    // The amount of steps to hit the left side of the cuboid.
    double t_x0 = (min.x - ray.p.x) / ray.d.x;
    
    // The amount of steps to hit the right side of the cuboid.
    double t_x1 = (max.x - ray.p.x) / ray.d.x;
    
    // Calculate the same for all the other sides.
    // Each time 0 is the min side and 1 is the max side.
    double t_y0 = (min.y - ray.p.y) / ray.d.y;
    double t_y1 = (max.y - ray.p.y) / ray.d.y;
    double t_z0 = (min.z - ray.p.z) / ray.d.z;
    double t_z1 = (max.z - ray.p.z) / ray.d.z;
    
    // If any dimension of `ray.d` is 0, we want the values of the `t` variables
    // of that dimension to be -∞ and +∞.
    // Most compilers use IEEE 754, which means `1.0 / 0.0 == ∞`, but not all,
    // so it's worth a check.
    static_assert(Limits<double>::is_iec559,
                  "Unfortunately your compiler uses a floating point implementation that "
                  "is not compatible with an algorithm used by this program. "
                  "You cannot compile this program. "
                  "(Feel free to comment out this static_assert and try though)");
    
    // For each pair of sides, determine which side is closest to `ray.p`.
    bool is_x0_closer = t_x0 < t_x1;
    double t_x_near = is_x0_closer ? t_x0 : t_x1;
    double t_x_far = is_x0_closer ? t_x1 : t_x0;
    
    bool is_y0_closer = t_y0 < t_y1;
    double t_y_near = is_y0_closer ? t_y0 : t_y1;
    double t_y_far = is_y0_closer ? t_y1 : t_y0;
    
    bool is_z0_closer = t_z0 < t_z1;
    double t_z_near = is_z0_closer ? t_z0 : t_z1;
    double t_z_far = is_z0_closer ? t_z1 : t_z0;
    
    // A bunch of logic. Take a look at the description of the algorithm.
    //
    // Basically, if any near point is more steps away than any far point, that
    // means we don't have an intersection. Similarly, if any far point is less
    // steps away than a near point, we don't have an intersection.
    //
    // Moreover we're slowly tracking down the `t` of the near intersection and
    // the `t` of the far intersection (even though we discard the far
    // intersection, we still need `t_far` for logic)
    
    using std::min;
    using std::max;
    
    double t_near = t_x_near; // The amount of steps to the near intersection.
    double t_far = t_x_far; // The amount of steps to the far intersection.
    
    // Y
    if (t_y_far < t_near || t_y_near > t_far)
      return nowhere;
    
    t_near = max(t_near, t_y_near);
    t_far = min(t_far, t_y_far);
    
    // Z
    if (t_z_far < t_near || t_z_near > t_far)
      return nowhere;
    
    t_near = max(t_near, t_z_near);
    t_far = min(t_far, t_z_far);
    
    
    if (t_near < 0) {
      // Here we would have an intersection if we had a line instead of a ray.
      return nowhere;
    }
    
    // We found the intersection!
    return ray.p + ray.d * t_near;
  }
  
};