#pragma once
#include <SDL_version.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include "int3.h"
#include "sdl/Geometries.h"
#include "GameConstants.h"

extern SDL_Window * mainWindow;
extern SDL_Renderer * mainRenderer;
extern SDL_Texture * screenTexture;

inline void SDL_WarpMouse(int x, int y)
{
    SDL_WarpMouseInWindow(mainWindow,x,y);
}

void SDL_UpdateRect(SDL_Surface *surface, int x, int y, int w, int h);

extern SDL_Surface * screen, *screen2, *screenBuf;
void blitAt(SDL_Surface * src, int x, int y, SDL_Surface * dst=screen);
void blitAt(SDL_Surface * src, const SDL_Rect & pos, SDL_Surface * dst=screen);

namespace CSDL_Ext {
    void update(SDL_Surface * what = screen); //updates whole surface (default - main screen)
    SDL_Surface * newSurface(int w, int h, SDL_Surface * mod=screen); //creates new surface, with flags/format same as in surface given

    void blitSurface(SDL_Surface * src, const SDL_Rect * srcRect, SDL_Surface * dst, SDL_Rect * dstRect);

    //scale surface to required size.
    //nearest neighbour algorithm
    SDL_Surface * scaleSurfaceFast(SDL_Surface *surf, int width, int height);
    // bilinear filtering. Uses fallback to scaleSurfaceFast in case of indexed surfaces
    SDL_Surface * scaleSurface(SDL_Surface *surf, int width, int height);
}

static Rect createCentered(int w, int h);