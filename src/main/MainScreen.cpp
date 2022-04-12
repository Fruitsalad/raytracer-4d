#include <SDL2/SDL.h>
#include <cassert>

#include "MainScreen.hpp"
#include "inputs.hpp"
#include "realMain.hpp"
#include "raytrace.hpp"
#include "util/debug.hpp"
#include "math/shape_rasterize.hpp"


MainScreen::~MainScreen() {
  toggle_controls_display->onActivate = nullptr;
  toggle_status_display->onActivate = nullptr;
  toggle_smaller_view->onActivate = nullptr;
  turn_4D_up->onActivate = nullptr;
  turn_4D_down->onActivate = nullptr;
}


void MainScreen::init() {
  initAssets();
  
  toggle_controls_display->onActivate = [this]() {
    isControlsVisible = !isControlsVisible;
    isStartScreenVisible = false;
  };
  toggle_status_display->onActivate = [this]() {
    isStatusBarVisible = !isStatusBarVisible;
  };
  toggle_smaller_view->onActivate = [this]() {
    if (!isStatusBarVisible)
      return;
    // Switch between four different zoom modes
    isZoomManual = true;
    targetZoomFactor -= 0.2;
    if (targetZoomFactor <= 0.1)
      targetZoomFactor = 1;
  };
  turn_4D_up->onActivate = [this]() {
    queuedWyRotations++;
  };
  turn_4D_down->onActivate = [this]() {
    queuedWyRotations--;
  };
}


void MainScreen::render() {
  clearScreen();
  
  
  // Zoom...
  
  int viewWidth = int(windowWidth);
  int viewHeight = int(windowHeight);
  SDL_Rect src = {0, 0, viewWidth, viewHeight};
  SDL_Rect dest = {0, 0, 0, 0};
  
  if (zoomFactor != 1) {
    viewWidth = int(viewWidth * zoomFactor);
    viewHeight = int(viewHeight * zoomFactor);
    int view_x = (int(windowWidth) - viewWidth) / 2;
    int view_y = (int(windowHeight) - viewHeight) / 2;
    src = {0, 0, viewWidth, viewHeight};
    dest = {view_x, view_y, 0, 0};
  }
  
  
  // Viewport raytracing...

#ifdef ENABLE_THREADS
  camera.forEachRay(viewWidth, viewHeight, threads, [](int x, int y, Ray ray) {
#else
  camera.forEachRay(viewWidth, viewHeight, [](int x, int y, Ray ray) {
#endif
    Vec4 pixelColor = raytrace(ray);
    setPixel(canvas, x, y, pixelColor);
  });
  
  SDL_BlitSurface(canvas, &src, screen, &dest);
  
  
  // User interface...
  drawWyIndicator();
  
  if (isStatusBarVisible)
    drawStatusBar();
  
  if (sphereTouchedPoints > 0 && (isControllingCamera || !isControlsVisible)) {
    isStartScreenVisible = false;
    drawPointsLabel();
  }
  
  if (!isZoomManual) {
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, "ADJUSTING RESOLUTION");
    writeOnScreen({0.5, 0.98}, {0.5, 1}, "© Zowie van Dillen, 2022");
    writeOnScreen({0.5, 1}, {0.5, 1},
                  "Feel free to redistribute this program unedited");
  } else if (!isControllingCamera
             && (isControlsVisible || isStartScreenVisible)) {
    writeOnScreen({0.5, 0.9}, {0.5, 0.5},
        "Click this window with the right mouse button to capture the mouse.");
    writeOnScreen({0.5, 0.98}, {0.5, 1}, "© Zowie van Dillen, 2022");
    writeOnScreen({0.5, 1}, {0.5, 1},
                  "Feel free to redistribute this program unedited");
  } else if (isControlsVisible) {
    drawControls();
  } else if (isStartScreenVisible) {
    drawStartScreen();
  }
  
  // Finalize...
  SDL_UpdateWindowSurface(window);
}


void MainScreen::update() {
  // Handle zooming...
  using std::min;
  
  if (!isZoomManual)
    adjustZoomLevelToFramerate();
  else if (abs(zoomFactor - targetZoomFactor) < 0.001)
    zoomFactor = targetZoomFactor;  // Prevent slight jiggling
  else
    zoomFactor = lerp(zoomFactor, targetZoomFactor, min(deltaTime * 4, 1.0));
  
  
  // Rotate the camera...
  if (turn_horizontally != 0 || turn_vertically != 0)
    camera.rotate(turn_horizontally, turn_vertically, 0);
  
  if (turn_4D > 0)
    camera.rotateWY_clockwise();
  else if (turn_4D < 0)
    camera.rotateWY_counterClockwise();
  
  if (queuedWyRotations >= 1)
    do camera.rotateWY_clockwise(); while (--queuedWyRotations > 1);
  else if (queuedWyRotations <= -1)
    do camera.rotateWY_counterClockwise(); while (++queuedWyRotations < -1);
  
  
  // Move around...
  Vec4 velocity = {0,0,0,0};
  
  // All the go_xxx variables are booleans, so int(go_xxx) is either 1 or 0.
  velocity.x = int(*go_right) - int(*go_left);
  velocity.y = int(*go_Y_plus) - int(*go_Y_minus);
  velocity.z = int(*go_up) - int(*go_down);
  velocity.w = int(*go_W_plus) - int(*go_W_minus);
  
  camera.velocity = maybe_normalize(velocity);
  camera.applyMovement(deltaTime);
  
  updateWhiteSphere();
}



static void addCuboid(
    Vec4 start, Vec4 end,
    Vec4 lightColor, Vec4 darkColor
) {
  auto newCuboid = make_unique<AlignedHypercuboid>();
  newCuboid->min = start;
  newCuboid->max = end;
  newCuboid->lightColor = lightColor;
  newCuboid->darkColor = darkColor;
  world.push_back(std::move(newCuboid));
}


static void addSphere(
    Vec4 center, double radius,
    Vec4 lightColor, Vec4 darkColor
) {
  auto newSphere = make_unique<Hypersphere>();
  newSphere->center = center;
  newSphere->radius = radius;
  newSphere->lightColor = lightColor;
  newSphere->darkColor = darkColor;
  world.push_back(std::move(newSphere));
}


void MainScreen::initAssets() {
  Vec4 white = {1,1,1,1};
  Vec4 grey = {0.2,0.2,0.2,1};
  Vec4 grey2 = {.7,.7,.7,1};
  Vec4 red = {1,.2,.2,1};
  Vec4 dark_red = {.4,.1,.1,1};
  Vec4 green = {.2,1,.2,1};
  Vec4 green2 = {.4,1,.2,1};
  Vec4 greenish_blue = {.3,.5,.4,.3};
  Vec4 greenish_blue2 = {.3,.53,.38,.3};
  Vec4 blue = {.1,.1,.8,1};
  Vec4 dark_blue = {.1,.1,.4,1};
  Vec4 black = {0,0,0,1};
  
  Vec4 darker = {-0.1, -0.1, -0.1, 0};
  Vec4 lighter = {0.1, 0.1, 0.1, 0};
  
  addCuboid({-1, -10, -1, -1}, {1, -8, 1, 1}, white, grey);
  addCuboid({-1.5, -10.5, -0.5, -2}, {0.5, -8.5, 1.5, -1}, red, dark_red);
  
  addCuboid({-6, -5, -1, -2}, {-1, -3, 1, 3}, red, dark_red);
  addSphere({8, 3, 3, 1}, 4, blue, dark_blue);
  
  // Clouds
  addCuboid({-10, -10, 50, -10}, {1, 80, 51, 30}, white, white + darker);
  addCuboid({-50, -30, 51, -1}, {-13, 7, 53, 1}, white, white + darker);
  
  // Ground
  addCuboid({-100, -100, -100, -100}, {100, 100, -4, 100}, green, greenish_blue);
  addCuboid({-100, -50, -100, -100}, {100, 5, -3, 100}, green2, greenish_blue2);
  
  // A big sphere towards W
  addSphere({0,0,10,40}, 20, red, dark_red);
  
  // Corner spheres
  addSphere({90, -90, 0, 90}, 10, white, grey);
  addSphere({-90, 90, 0, 90}, 10, white, dark_blue);
  addSphere({90, -90, 0, -90}, 10, white, dark_red);
  addSphere({-90, 90, 0, -90}, 10, white, greenish_blue);
  
  // Border fences
  addCuboid({-99, -99, -6, -99}, {99, -98, 0, 99}, red, dark_red + lighter);
  addCuboid({-99, 98, -6, -99}, {99, 99, 0, 99}, red, dark_red + lighter);
  addCuboid({98, -99, -6, -99}, {99, 99, 0, 99}, red, dark_red);
  addCuboid({-99, -99, -6, -99}, {-98, 99, 0, 99}, red, dark_red);
  addCuboid({-99, -99, -6, -99}, {99, 99, 0, -98}, red, dark_red + darker);
  addCuboid({-99, -99, -6, 98}, {99, 99, 0, 99}, red, dark_red + darker);
  
  // Black sphere
  addSphere({-35, 11, 10, -8}, 2, white, black);
  
  // Fenced area
  addCuboid({-30, -30, -3, 10}, {-20, -29.5, 0, 20}, red, dark_red + lighter);
  addCuboid({-30, -20.5, -3, 10}, {-20, -20, 0, 20}, red, dark_red + lighter);
  addCuboid({-20.5, -30, -3, 10}, {-20, -20, 0, 20}, red, dark_red);
  addCuboid({-30, -30, -3, 10}, {-29.5, -20, 0, 20}, red, dark_red);
  addCuboid({-30, -30, -3, 10}, {-20, -20, 0, 10.5}, red, dark_red + darker);
  addCuboid({-30, -30, -3, 19.5}, {-20, -20, 0, 20}, red, dark_red + darker);
  
  // House
  addCuboid({30, -30, -3, 30}, {40, -29.5, 0, 40}, red, dark_red + lighter);
  addCuboid({30, -20.5, -3, 30}, {40, -20, 0, 40}, red, dark_red + lighter);
  addCuboid({39.5, -30, -3, 30}, {40, -20, 0, 40}, red, dark_red);
  addCuboid({30, -30, -3, 30}, {40, -20, 0, 30.5}, red, dark_red + darker);
  addCuboid({30, -30, -3, 39.5}, {40, -20, 0, 40}, red, dark_red + darker);
  // wall with door hole
  addCuboid({30, -30, -3, 30}, {30.5, -20, 0, 34}, red, dark_red);
  addCuboid({30, -30, -3, 36}, {30.5, -20, 0, 40}, red, dark_red);
  addCuboid({30, -30, -3, 30}, {30.5, -25, 0, 40}, red, dark_red + darker);
  addCuboid({30, -23, -3, 30}, {30.5, -20, 0, 40}, red, dark_red + darker);
  // roof
  addCuboid({30, -30, 0, 30}, {40, -20, 0.5, 40}, red, red + darker);
  // floor
  addCuboid({30, -30, -3, 30}, {40, -20, -2.99, 40}, white, grey2);
  
  addSphere({38, -21.3, -1.5, 35}, WHITE_SPHERE_RADIUS, white, white + darker);
  whiteHypersphere = dynamic_cast<Hypersphere*>(world.back().get());
}


void MainScreen::drawStatusBar() {
  auto as_str = [](double a) -> String {
    return $(int(std::round(a)));
  };
  
  auto as_degrees = [](double a) -> String {
    return $(int(std::round(a*(180/pi))));
  };
  
  frameCounter.updateFramerate();
  
  const Vec4 p = camera.pos;
  String text =
      "[X"+as_str(p.x)
      +" Y"+as_str(p.y)
      +" Z"+as_str(p.z)
      +" W"+as_str(p.w)+"]"
      +"   YAW "+as_degrees(camera.yaw)+"°"
      +"   PITCH "+as_degrees(camera.pitch)+"°"
      +"   WY "+as_degrees(camera.wy_rotation)+"°"
      +"   Running at "+as_str(frameCounter.framerate)+" Hz";
  
  if (zoomFactor < 1)
    // Weird way to turn a double into a string with significance but it works
    text += "  ZOOM x0."+as_str(zoomFactor * 10);
  
  writeOnScreen({0.5, 0}, {0.5, 0}, text);
}


void MainScreen::drawPointsLabel() {
  // Increasingly congratulatory texts depending on the amount of points.
  // The pinnacle of game design
  static const List<String> numbersTenToTwenty = {
      "TWENTY", "TWENTYONE", "TWENTYTWO", "TWENTYTHREE", "TWENTYFOUR",
      "TWENTYFIVE", "TWENTYSIX", "TWENTYSEVEN", "TWENTYEIGHT", "TWENTYNINE",
      "THIRTY"
  };
  static const List<String> congrats = {
      "CONGRATULATIONS", "AMAZING", "WELL DONE", "BRAVO", "ASTOUNDING", "GOLLY"
  };
  
  static String$ score_one = "You touched the white hypersphere one time";
  
  
  if (sphereTouchedPoints >= 30) {
    // This used to be a bug when you had 20 points, but I like the way it looks
    if (garbageText.empty()) {
      garbageText.resize(20, '#');
      for (char& c : garbageText)
        c = char(rand());
    }
    
    String msg = "Wow! You have touched the white hypersphere "
                 +$(sphereTouchedPoints)+" times?!? "+garbageText+"!!!!!!!!!!!";
    
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, msg);
    
  } else if (sphereTouchedPoints >= 20) {
    int a = sphereTouchedPoints - 20;
    assert(a < 10);
    String msg = "Wow! You have touched the white hypersphere "
                 +numbersTenToTwenty[a]+" times?!? "+congrats[a/2];
    // Add a lot of exclamation marks
    msg.resize(msg.size() + a, '!');
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, msg);
    
  } else if (sphereTouchedPoints > 10) {
    String msg = "You have touched the white hypersphere "
                 +$(sphereTouchedPoints)+" times. That's a lot!";
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, msg);
    
  } else if (sphereTouchedPoints == 10) {
    String msg = "You have touched the white hypersphere ten times. Nice!";
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, msg);
  
  } else if (sphereTouchedPoints == 1) {
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, score_one);
    
  } else {
    String msg = "You touched the white hypersphere "+$(sphereTouchedPoints)
                 + " times";
    writeOnScreen({0.5, 0.9}, {0.5, 0.5}, msg);
  }
}


void MainScreen::drawControls() {
  String text = "Controls:\n"
       "Right mouse button: Capture the mouse\n"
       "W, A, S & D: Move forwards, left, backwards and right respectively\n"
       "Shift & control: Move up and down\n"
       "R & F: Move on the W axis (or the Y axis, depending on rotation)\n"
       "Scroll: Rotate on the WY plane (alternative keys: O & P)\n"
       "F3: Show your coordinates\n"
       "F4: With coordinates visible, F4 switches between different viewport "
       "sizes\n"
       "    (smaller viewport gives you a higher framerate)\n"
       "\n"
       "Controls may vary if you aren't using a QWERTY keyboard, but you can\n"
       "always figure out the keybindings through experimentation.\n"
       "\n"
       "Press F1 to hide these controls";
  writeOnScreenWithLinebreaks({0.02,0.98},{0,1}, text);
  writeOnScreen({0.98,0.98},{1,1},
      "If you're direly in need of a goal, try finding and touching the white "
      "hypersphere!");
}

void MainScreen::drawStartScreen() {
  writeOnScreen({0.5,0.04},{0.5,0}, "SLICING 4D RAYTRACER");
  writeOnScreen({0.5,0.08},{0.5,0},
                "A renderer that shows a slice of a 4D world.");
  writeOnScreen({0.5,0.11},{0.5,0},
                "Press F1 for an overview of the controls.");
  
  #ifdef __EMSCRIPTEN__
  writeOnScreen({0.5,0.97},{0.5,0},
                "THIS IS THE WEB VERSION. TRY A DOWNLOADABLE VERSION FOR MUCH "
                "BETTER RESOLUTIONS AND FRAMERATES.");
  #endif
}


void MainScreen::drawWyIndicator() {
  SDL_Surface* surface =
      SDL_CreateRGBSurface(0, 30, 30, 32,
                           0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  
  Vec2i center = {15, 15};
  int radius = 3;
  
  forEachPixelInCircle(center, radius, [&](int x, int y) {
    setPixel(surface, x, y, Vec4(1,1,1,1));
  });
  
  int lineLength = 10;
  Vec2 lineDirection = {sin(camera.wy_rotation), -cos(camera.wy_rotation)};
  Vec2 lineVector = lineDirection.withLength(lineLength);
  Vec2i lineEnd = center + Vec2i(lineVector.x, lineVector.y);
  
  forEachPixelInLineSegment(center, lineEnd, [&](int x, int y) {
    setPixel(surface, x, y, Vec4(1,1,1,1));
  });
  
  // Letters
  writeOnScreen({0.95,0.48}, {0.5, 1}, "Y");
  writeOnScreen({0.97,0.5}, {0, 0.5}, "W");
  
  SDL_Rect dest = calcBlitDest({0.95,0.5}, {0.5,0.5}, surface->w, surface->h);
  SDL_BlitSurface(surface, nullptr, screen, &dest);
  SDL_FreeSurface(surface);
}


void MainScreen::adjustZoomLevelToFramerate() {
  time += deltaTime;
  frames++;
  
  if (frames < TARGET_SAMPLES)
    return;
  
  double framerate = double(frames) / time;
  time = 0;
  frames = 0;
  
  if (framerate < TARGET_FRAMERATE) {
    targetZoomFactor = previousZoomFactor;
    isZoomManual = true;
  } else if (zoomFactor > 0.9) {
    targetZoomFactor = 1.0;
    isZoomManual = true;
  } else {
    previousZoomFactor = zoomFactor;
    zoomFactor = lerp(zoomFactor, targetZoomFactor, 0.01);
  }
}


void MainScreen::moveWhiteSphereRandomly() {
  const double p = sphereTouchedPoints * 4;
  const Vec4 minima = {-p, -p, -3, -p};
  const Vec4 maxima = {p, p, p/2, p};
  const Vec4 range_sizes = maxima - minima;
  
  if (whiteHypersphere == nullptr) {
    printf("White hypersphere not found.");
    return;
  }
  
  // rand() isn't getting seeded but I think the RNG is fine as is
  Vec4 random = Vec4(rand(),rand(),rand(),rand());
  Vec4 newPos = minima + random.elemMult(range_sizes) / RAND_MAX;
  whiteHypersphere->center = newPos;
}


void MainScreen::updateWhiteSphere() {
  Vec4 white = {1,1,1,1};
  Vec4 yellow = {1,1,0,1};
  auto& sphere = *whiteHypersphere;
  
  // Animate the radius...
  double goalRadius = (isSphereDisappearing ? 0 : WHITE_SPHERE_RADIUS);
  sphere.radius = lerp(sphere.radius, goalRadius, std::min(deltaTime * 4, 1.0));
  
  // Gradually color the sphere yellow if you just touched it...
  double a = sphere.radius / WHITE_SPHERE_RADIUS;
  sphere.lightColor = a*white + (1-a)*yellow;
  
  // Move the sphere after you touched it...
  if (isSphereDisappearing && sphere.radius < 0.001) {
    if (sphereTouchedPoints == 1)
      whiteHypersphere->center = {-25, -25, -1.5, 15};
    else
      moveWhiteSphereRandomly();
    isSphereDisappearing = false;
  }
  
  // Check if the sphere was touched...
  Vec4 diff = camera.pos - sphere.center;
  if (!isSphereDisappearing && diff.calcLength() < WHITE_SPHERE_TOUCH_RADIUS) {
    isSphereDisappearing = true;
    sphereTouchedPoints++;
    garbageText = "";
  }
}
