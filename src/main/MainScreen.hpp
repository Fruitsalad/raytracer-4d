#pragma once


#include "screen/iScreen.hpp"
#include "FlyingCameraController.hpp"
#include "FrameCounter.hpp"
#include "geometry/Hypersphere.hpp"


class MainScreen : public iScreen {
public:
  ~MainScreen();
  
  // iScreen
  void init() override;
  void render() override;
  void update() override;
  
  // custom
  void initAssets();
  void drawStatusBar();
  void drawPointsLabel();
  void drawControls();
  void drawStartScreen();
  void drawWyIndicator();
  void adjustZoomLevelToFramerate();
  void updateWhiteSphere();
  void moveWhiteSphereRandomly();
  
  FlyingCameraController camera;
  FrameCounter frameCounter;
#ifdef ENABLE_THREADS
  ThreadPool threads;
#endif
  
  // Inputs & screens
  bool isStartScreenVisible = true;
  bool isStatusBarVisible = false;
  bool isControlsVisible = false;
  int queuedWyRotations = 0;
  
  // Zooming
  #ifdef ENABLE_THREADS
  static constexpr double TARGET_FRAMERATE = 45;
  #else
  static constexpr double TARGET_FRAMERATE = 20;
  #endif
  
  static constexpr double TARGET_SAMPLES = 3;
  double time = 0;
  int frames = 0;
  bool isZoomManual = false;
  double zoomFactor = 0.2;
  double targetZoomFactor = 1.0;
  double previousZoomFactor = 0.2;
  
  // White hypersphere
  static constexpr double WHITE_SPHERE_RADIUS = 1;
  static constexpr double WHITE_SPHERE_TOUCH_RADIUS = 1.5;
  String garbageText = "";
  Hypersphere* whiteHypersphere = nullptr;
  bool isSphereDisappearing = false;
  uint sphereTouchedPoints = 0;
};
