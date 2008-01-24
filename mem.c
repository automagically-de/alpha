#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

Uint32 mem_ofsurface(SDL_Surface *surface)
{
  if(surface == NULL) return 0;
#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: mem_ofsurface: surface %p\n", surface);
#endif
  return surface->w * surface->h * surface->format->BytesPerPixel +
         sizeof(SDL_Surface) + sizeof(SDL_PixelFormat);
}

