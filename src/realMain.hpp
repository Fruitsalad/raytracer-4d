#ifndef FRUIT_REALMAIN_HPP
#define FRUIT_REALMAIN_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "util.hpp"
#include "math.hpp"


inline bool please_stop = false;

// Normally this kind of thing is passed around as a parameter but it's so
// tiresome passing the same parameter everywhere. That's why it's a global.
inline double deltaTime = .001;

inline SDL_Window* window = nullptr;
// This Surface is the render target. We're doing good old software rendering.
inline SDL_Surface* screen = nullptr;
inline SDL_Surface* canvas = nullptr;

inline TTF_Font* font = nullptr;
inline SDL_Color white = {255, 255, 255};
inline SDL_Color black = {0, 0, 0};

inline uint windowWidth = 800;
inline uint windowHeight = 600;

inline bool isControllingCamera = false;


Vec4 raytrace(const Ray& ray);
void setPixel(SDL_Surface* surface, int x, int y, Vec4 color);
void setPixel(SDL_Surface* surface, int x, int y, u32 rgba);
void clearScreen();
SDL_Rect calcBlitDest(Vec2 pos, Vec2 anchor, int surface_w, int surface_h);
void writeOnScreen(Vec2 pos, Vec2 anchor, String$& text);
void writeOnScreenWithLinebreaks(Vec2 pos, Vec2 anchor, String$& text);





#endif //FRUIT_REALMAIN_HPP
