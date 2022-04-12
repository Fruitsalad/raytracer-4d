#pragma once

#include <SDL2/SDL.h>

#include "math.hpp"


class FlyingCameraController {
public:
  Vec4 pos = {0,0,0,0};
  
  double yaw = -pi / 2;
  double pitch = 0;
  double wy_rotation = 0;
  
  double target_wy_rotation = 0;
  
  Vec4 velocity = {0,0,0,0};
  
  double speed = 5;
  double sensitivity = 0.005;
  
  double fov_y = pi / 4.0;
  
  
  void rotate(double p_yaw, double p_pitch, double p_wy_rot) {
    static const double upperLimit = pi/2.0 - 0.001;
    static const double lowerLimit = -pi/2.0 + 0.001;
    
    yaw += p_yaw * sensitivity;
    pitch -= p_pitch * sensitivity;
    wy_rotation += p_wy_rot * sensitivity;
    
    yaw = std::remainder(yaw - pi, 2*pi) + pi;
    pitch = clamp(pitch, lowerLimit, upperLimit);
  }
  
  void rotateWY_clockwise() {
    target_wy_rotation += pi / 8;
  }
  
  
  void rotateWY_counterClockwise() {
    target_wy_rotation -= pi / 8;
  }
  
  
  void applyMovement(double deltaTime) {
    Vec4 direction = dir_vec(yaw, pitch, wy_rotation);
    Vec4 right = dir_vec(yaw + pi/2, 0, wy_rotation);
    Vec4 up = dir_vec(yaw, pitch + pi/2, wy_rotation);
    Vec4 w_up = {0,sin(wy_rotation),0,cos(wy_rotation)};
    
    pos += speed * deltaTime * velocity.y * direction;
    pos += speed * deltaTime * velocity.x * right;
    pos += speed * deltaTime * velocity.z * up;
    pos += speed * deltaTime * velocity.w * w_up;
    
    double lerpFactor = std::min(deltaTime * 4, 1.0);
    wy_rotation = lerp(wy_rotation, target_wy_rotation, lerpFactor);
    
    // Keep wy_rotation between 0 and 2*pi
    if (wy_rotation < 0 || wy_rotation > 2 * pi) {
      double full_rotations = std::floor(wy_rotation / (2 * pi));
      wy_rotation -= full_rotations * 2 * pi;
      target_wy_rotation -= full_rotations * 2 * pi;
    }
  }
  
  
  // Calculates a ray for each pixel on the screen, and hands that ray to the
  // provided function (stored in `doSomething`).
  template<class CustomFunction>
  void forEachRay(int width, int height, CustomFunction doSomething) const {
    const auto wy_r = wy_rotation;
    
    double screen_ratio = width / double(height);
    double fov_x = screen_ratio * fov_y;
    
    // The direction vector for the ray of the center of the screen
    Vec4 centerDir = dir_vec(yaw, pitch, wy_r);
  
    // The view rectangle is a slice of the view frustrum.
    // These two vectors define the orientation and size of the view rectangle
    // where viewrect_x determines the horizontal direction (which is on the xy
    // or xw plane when wy_rot is a multiple of 90 degrees)
    // and viewrect_y determines the vertical direction,
    // which should be orthogonal to viewrect_x and the forwards direction.
    // viewrect_y is inverted so that the z-axis points upwards instead of down.
    double viewrect_width = 2 * tan(fov_x/2);
    double viewrect_height = 2 * tan(fov_y/2);
    Vec4 viewrect_x = dir_vec(yaw + pi/2, 0, wy_r) * viewrect_width;
    Vec4 viewrect_y = dir_vec(yaw, pitch + pi/2, wy_r) * viewrect_height * -1;
  
  
    // For each pixel...
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        // Create a ray...
        double xn = x / double(width-1) - 0.5;  // xn: x from -0.5 to 0.5
        double yn = y / double(height-1) - 0.5;
        Vec4 rayDir = normalize(centerDir + xn*viewrect_x + yn*viewrect_y);
        Ray ray = {pos, rayDir};
      
        // And call the given function...
        doSomething(x, y, ray);
      }
    }
  }


#ifdef ENABLE_THREADS
  template<class CustomFunction>
  void forEachRay(
      int width, int height, ThreadPool& threadPool, CustomFunction doSomething
  ) {
    // See the other implementation of forEachRay for comments on this part
    const auto wy_r = wy_rotation;
    double screen_ratio = width / double(height);
    double fov_x = screen_ratio * fov_y;
    Vec4 centerDir = dir_vec(yaw, pitch, wy_r);
    double viewrect_width = 2 * tan(fov_x/2);
    double viewrect_height = 2 * tan(fov_y/2);
    Vec4 viewrect_x = dir_vec(yaw + pi/2, 0, wy_r) * viewrect_width;
    Vec4 viewrect_y = dir_vec(yaw, pitch + pi/2, wy_r) * viewrect_height * -1;
    
    // Here's the new stuff:
    
    auto handleRow = [=](uint y) {
      for (int x = 0; x < width; x++) {
        // Create a ray...
        double xn = x / double(width-1) - 0.5;  // xn: x from -0.5 to 0.5
        double yn = y / double(height-1) - 0.5;
        Vec4 rayDir = normalize(centerDir + xn*viewrect_x + yn*viewrect_y);
        Ray ray = {pos, rayDir};
    
        // And call the given function...
        doSomething(x, y, ray);
      }
    };
    
    
    uint task_count = 4 * threadPool.num_threads();
    uint rows_per_task = height / task_count;
    List<std::future<void>> futures;
  
    // Create the first n-1 tasks...
    for (uint i = 0; i < task_count-1; i++) {
      futures.emplace_back(threadPool.Submit([=]() {
        uint min_y = rows_per_task * i;
        uint max_y = min_y + rows_per_task;
        
        for (uint y = min_y; y < max_y; y++)
          handleRow(y);
      }));
    }
    
    // Create the last task...
    // The last task might have a few more rows if `height` isn't
    // divisible by `task_count`.
    futures.emplace_back(threadPool.Submit([=]() {
      uint min_y = rows_per_task * (task_count - 1);
      uint max_y = height;
      
      for (uint y = min_y; y < max_y; y++)
        handleRow(y);
    }));
    
    // Wait until all the threads are done rendering.
    for (const auto& future : futures)
      future.wait();
  }
#endif
};
