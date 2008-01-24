#ifndef _TEXT_H
#define _TEXT_H

#include "SDL.h"

int text_draw(SDL_Surface *screen, SDL_Surface *font, Uint32 x, Uint32 y,
              const char *text);
int text_drawwrapped(SDL_Surface *screen, SDL_Surface *font,
                     Uint32 x, Uint32 y, Uint32 width, Uint32 height,
                     const char *text);
int text_getsize(SDL_Surface *font, const char *text, Uint32 *width,
                 Uint32 *height);

#endif
