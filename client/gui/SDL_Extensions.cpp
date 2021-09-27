#include "SDL_Extensions.h"

void CSDL_Ext::update(SDL_Surface * what)
{
    if(!what)
        return;
    if(0 !=SDL_UpdateTexture(screenTexture, nullptr, what->pixels, what->pitch))
        logGlobal->error("%s SDL_UpdateTexture %s", __FUNCTION__, SDL_GetError());
}

void SDL_UpdateRect(SDL_Surface *surface, int x, int y, int w, int h)
{
    Rect rect(x,y,w,h);
    if(0 !=SDL_UpdateTexture(screenTexture, &rect, surface->pixels, surface->pitch))
        logGlobal->error("%sSDL_UpdateTexture %s", __FUNCTION__, SDL_GetError());

    SDL_RenderClear(mainRenderer);
    if(0 != SDL_RenderCopy(mainRenderer, screenTexture, NULL, NULL))
        logGlobal->error("%sSDL_RenderCopy %s", __FUNCTION__, SDL_GetError());
    SDL_RenderPresent(mainRenderer);

}

SDL_Surface * CSDL_Ext::newSurface(int w, int h, SDL_Surface * mod) //creates new surface, with flags/format same as in surface given
{
    SDL_Surface * ret = SDL_CreateRGBSurface(0,w,h,mod->format->BitsPerPixel,mod->format->Rmask,mod->format->Gmask,mod->format->Bmask,mod->format->Amask);
    if (mod->format->palette)
    {
        assert(ret->format->palette);
        assert(ret->format->palette->ncolors == mod->format->palette->ncolors);
        memcpy(ret->format->palette->colors, mod->format->palette->colors, mod->format->palette->ncolors * sizeof(SDL_Color));
    }
    return ret;
}

SDL_Surface * CSDL_Ext::scaleSurfaceFast(SDL_Surface *surf, int width, int height)
{
    if (!surf || !width || !height)
        return nullptr;

    //Same size? return copy - this should more be faster
    if (width == surf->w && height == surf->h)
        return copySurface(surf);

    SDL_Surface *ret = newSurface(width, height, surf);

    switch(surf->format->BytesPerPixel)
    {
        case 1: scaleSurfaceFastInternal<1>(surf, ret); break;
        case 2: scaleSurfaceFastInternal<2>(surf, ret); break;
        case 3: scaleSurfaceFastInternal<3>(surf, ret); break;
        case 4: scaleSurfaceFastInternal<4>(surf, ret); break;
    }
    return ret;
}

// scaling via bilinear interpolation algorithm.
// NOTE: best results are for scaling in range 50%...200%.
// And upscaling looks awful right now - should be fixed somehow
SDL_Surface * CSDL_Ext::scaleSurface(SDL_Surface *surf, int width, int height)
{
    if (!surf || !width || !height)
        return nullptr;

    if (surf->format->palette)
        return scaleSurfaceFast(surf, width, height);

    //Same size? return copy - this should more be faster
    if (width == surf->w && height == surf->h)
        return copySurface(surf);

    SDL_Surface *ret = newSurface(width, height, surf);

    switch(surf->format->BytesPerPixel)
    {
        case 2: scaleSurfaceInternal<2>(surf, ret); break;
        case 3: scaleSurfaceInternal<3>(surf, ret); break;
        case 4: scaleSurfaceInternal<4>(surf, ret); break;
    }

    return ret;
}

void blitAt(SDL_Surface * src, int x, int y, SDL_Surface * dst)
{
    if(!dst) dst = screen;
    SDL_Rect pom = genRect(src->h,src->w,x,y);
    CSDL_Ext::blitSurface(src,nullptr,dst,&pom);
}

void blitAt(SDL_Surface * src, const SDL_Rect & pos, SDL_Surface * dst)
{
    if (src)
        blitAt(src,pos.x,pos.y,dst);
}

void CSDL_Ext::blitSurface( SDL_Surface * src, const SDL_Rect * srcRect, SDL_Surface * dst, SDL_Rect * dstRect )
{
    if (dst != screen)
    {
        SDL_UpperBlit(src, srcRect, dst, dstRect);
    }
    else
    {
        SDL_Rect betterDst;
        if (dstRect)
        {
            betterDst = *dstRect;
        }
        else
        {
            betterDst = Rect(0, 0, dst->w, dst->h);
        }

        SDL_UpperBlit(src, srcRect, dst, &betterDst);
    }
}

Rect Rect::createCentered( int w, int h )
{
    return Rect(screen->w/2 - w/2, screen->h/2 - h/2, w, h);
}