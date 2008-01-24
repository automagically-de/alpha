#include <stdlib.h>

#include "SDL.h"
#include "gfx_prim.h"

Uint32 gfx_getpixel(SDL_Surface *surface, Uint32 x, Uint32 y)
{
  int bpp;
  Uint32 color = 0; 
  Uint8 *p;

  if(surface == NULL) return 0;
  if((x >= surface->w) || (y >= surface->h)) return 0;

  bpp = surface->format->BytesPerPixel;
  p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: gfx_getpixel: (%d/%d): %d bpp\n", x, y, bpp);
#endif

#if 0
  if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
#endif
  switch(bpp)
  {
    case 1:
      color = *p;
      break;
    case 2:
      color = *(Uint16*)p;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) 
           color = p[0] << 16 | p[1] << 8 | p[2];
      else color = p[0] | p[1] << 8 | p[2] << 16;
      break;
    case 4:
      color = *((Uint32*)p);
      break;
    default:
      break;
  }
#if 0
  if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
#endif
#ifdef DEBUG_VV
      fprintf(stderr, "DEBUG: gfx_getpixel: (%d/%d): %u\n", x, y, color);
#endif 

  return color;
}

/*****************************************************************************/

void gfx_putpixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pixel)
{
  Uint8 *p;
  int bpp;

  if(surface == NULL) return;

  if((x < 0) || (y < 0) ||
     (x >= surface->w) ||
     (y >= surface->h)) return;

  if(SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

  bpp = surface->format->BytesPerPixel;
  p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    default:
        break;
  }
  if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}

/*****************************************************************************/

void gfx_rectangle(SDL_Surface *surface, Sint32 x, Sint32 y, 
                  Uint32 width, Uint32 height, Uint32 color)
{
  Sint32 i;

  for(i=x; i < (x+width); i++) gfx_putpixel(surface, i, y, color);
  for(i=x; i < (x+width); i++) gfx_putpixel(surface, i, y+height-1, color);
  for(i=y; i < (y+height); i++) gfx_putpixel(surface, x, i, color);
  for(i=y; i < (y+height); i++) gfx_putpixel(surface, x+width-1, i, color);
}

/*****************************************************************************/

void  gfx_line(SDL_Surface *surface, Sint32 x1, Sint32 y1, 
               Sint32 x2, Sint32 y2, Uint32 color)
{
  Sint32 i;
  float co, rate;

  if(abs(x2 - x1) > abs(y2 - y1))
  {
    if((y2 - y1) == 0) rate = 0;
    else rate = (float)abs(y2 - y1) / (float)abs(x2 - x1);
    co = y1;
    for(i=x1; i<=x2; i++)
    {
      co += rate;
      gfx_putpixel(surface, i, (Uint32)co, color);
    }
  }
  else
  {
    if((x2 - x1) == 0) rate = 0;
    else rate = (float)abs(x2 - x1) / (float)abs(y2 - y1);
    co = x1;
    for(i=y1; i<=y2; i++)
    {
      co += rate;
      gfx_putpixel(surface, (Uint32)co, i, color);
    }
  }
}

/*****************************************************************************/

void gfx_edge(SDL_Surface *surface, Sint32 x, Sint32 y, Uint32 width,                         Uint32 height, Uint32 light, Uint32 dark, Uint8 flags)
{
  if(flags & EDGE_OUT)
  {
    gfx_line(surface, x + 1, y, x + width - 2, y, light);
    gfx_line(surface, x, y + 1, x, y + height - 2, light);
    gfx_line(surface, x + 1, y + height - 1, x + width - 2, y + height - 1,
             dark);
    gfx_line(surface, x + width - 1, y + 1, x + width - 1, y + height - 2,
             dark);
  }
  else if(flags & EDGE_IN)
  {
    gfx_line(surface, x + 1, y, x + width - 2, y, dark);
    gfx_line(surface, x, y + 1, x, y + height - 2, dark);
    gfx_line(surface, x + 1, y + height - 1, x + width - 2, y + height - 1,
             light);
    gfx_line(surface, x + width - 1, y + 1, x + width - 1, y + height - 2,
             light);
  }
}


