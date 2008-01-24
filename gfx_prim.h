#ifndef _GFX_PRIM_H
#define _GFX_PRIM_H

#include "SDL.h"

#define EDGE_IN    (1 << 0)
#define EDGE_OUT   (1 << 1)

Uint32 gfx_getpixel(SDL_Surface *surface, Uint32 x, Uint32 y);
void gfx_putpixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 color);
void gfx_rectangle(SDL_Surface *surface, Sint32 x, Sint32 y, 
                  Uint32 width, Uint32 height, Uint32 color);
void  gfx_line(SDL_Surface *surface, Sint32 x1, Sint32 y1,
               Sint32 x2, Sint32 y2, Uint32 color);
void gfx_edge(SDL_Surface *surface, Sint32 x, Sint32 y, Uint32 width, 
              Uint32 height, Uint32 light, Uint32 dark, Uint8 flags);

#endif
