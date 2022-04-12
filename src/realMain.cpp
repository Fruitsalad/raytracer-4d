#include <chrono>
#include <iostream>
#include <cassert>

#include "realMain.hpp"
#include "math.hpp"
#include "raytrace.hpp"
#include "io/handleInput.hpp"
#include "main/MainScreen.hpp"
#include "util/debug.hpp"


double calcDeltaTime();

String getSdlError();
void initSDL();

static void moreInterestingMain();



// realMain is just an exception catcher
void realMain() {
#ifdef PLEASE_JUST_CRASH
  // If an exception is thrown, just crash. Useful for debugging in an IDE.
  moreInterestingMain();
#else
  runAndPrintExceptions(moreInterestingMain);
#endif
}


// The meat of the program
static void moreInterestingMain() {
  MainScreen currentScreen;
  initSDL();
  currentScreen.init();
  
  mainLoop([&] {
    deltaTime = calcDeltaTime();
    handleInput();
    currentScreen.update();
    currentScreen.render();
    return !please_stop;
  });
  
  // Note: Emscripten builds never get past the invocation of mainLoop
}


// Color values should be between 0.0 and 1.0 for each channel
void setPixel(SDL_Surface* surface, int x, int y, Vec4 color) {
  assert(surface->format->format & SDL_PIXELFORMAT_RGBA8888);
  assert(x >= 0 && x < surface->w);
  assert(y >= 0 && y < surface->h);
  
  const int row_size = surface->pitch;
  const int pixel_size = 4;
  
  // Get a pointer to the first byte of the pixel.
  u8* pixel = reinterpret_cast<u8*>(surface->pixels)
              + row_size * y + pixel_size * x;
  
  // Set each of the color channels separately.
  for (int i = 0; i < 4; i++) {
    double value = color[i] * 255.0;
    pixel[i] = (u8) clamp(value, 0.0, 255.0);
  }
}


void setPixel(SDL_Surface* surface, int x, int y, u32 rgba) {
  assert_(surface->format->format == SDL_PIXELFORMAT_RGBA8888);
  
  const int row_size = surface->pitch;
  const int pixel_size = 4;
  
  u32* pixel = reinterpret_cast<u32*>(surface->pixels)
              + row_size * y + pixel_size * x;
  *pixel = rgba;
}


void clearScreen() {
  const u32 black = SDL_MapRGB(canvas->format, 0, 0, 0);
  SDL_FillRect(canvas, nullptr, black);
  SDL_FillRect(screen, nullptr, black);
}


SDL_Rect calcBlitDest(Vec2 pos, Vec2 anchor, int surface_w, int surface_h) {
  return {
    int(pos.x * windowWidth) - int(anchor.x * surface_w),
    int(pos.y * windowHeight) - int(anchor.y * surface_h)
  };
}


void writeOnScreen(Vec2 pos, Vec2 anchor, String$& text) {
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), white);
  
  SDL_Rect dest = calcBlitDest(pos, anchor, surface->w, surface->h);
  SDL_BlitSurface(surface, nullptr, screen, &dest);
  SDL_FreeSurface(surface);
}


void writeOnScreenWithLinebreaks(Vec2 pos, Vec2 anchor, String$& text) {
  List<String> lines = {""};
  for (char c : text) {
    if (c == '\n')
      lines.emplace_back();
    else lines.back() += c;
  }
  
  List<SDL_Surface*> surfaces;
  
  int width = 0;
  int height = 0;
  
  for (String$& line : lines) {
    // TTF_RenderUTF8_Shaded does not support "" as an input, so we check here.
    const char* line2 = (line.empty() ? " " : line.c_str());
    
    auto surface = TTF_RenderUTF8_Shaded(font, line2, white, black);
    width += surface->w;
    height += surface->h;
    surfaces.push_back(surface);
  }
  
  SDL_Rect dest = calcBlitDest(pos, anchor, width, height);
  
  for (SDL_Surface* surface : surfaces) {
    SDL_BlitSurface(surface, nullptr, screen, &dest);
    dest.y += surface->h;
    SDL_FreeSurface(surface);
  }
};


template<class Duration>
double toDouble(Duration duration) {
  return std::chrono::duration<double>(duration).count();
}


double calcDeltaTime() {
  using namespace std::chrono;
  static auto previousTime = system_clock::now();
  auto newTime = system_clock::now();
  double deltaTime = toDouble(newTime - previousTime);
  previousTime = newTime;
  return deltaTime;
}


String getSdlError() {
  return SDL_GetError();
}


void initSDL() {
  SDL_Init(SDL_INIT_VIDEO);
  
  window = SDL_CreateWindow(
      "4D",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      windowWidth, windowHeight,
      0
  );
  
  if (window == nullptr)
    THROW("Failed to create a window: " + getSdlError());
  
  screen = SDL_GetWindowSurface(window);
  canvas = SDL_CreateRGBSurfaceWithFormat(
      0, windowWidth, windowHeight, 32, SDL_PIXELFORMAT_RGBA32
  );
  
  if (canvas == nullptr)
    THROW("Failed to create a canvas: " + getSdlError());
  
  if (TTF_Init() == -1)
    THROW("Failed to initialize SDL TTF: " + String(TTF_GetError()));
  
  font = TTF_OpenFont("./roboto.ttf", 12);
  if (!font)
    THROW("Failed to load font: " + String(TTF_GetError()));
}
