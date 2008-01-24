#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SDL.h"

#include "conf.h"
#include "mem.h"
#include "gfx_prim.h"
#include "image_scale.h"

SDL_Surface *_image_scale_down(SDL_Surface *srcimage, float factor);
SDL_Surface *_image_scale_up(SDL_Surface *srcimage, float factor);

/*****************************************************************************/

SDL_Surface *image_scale(SDL_Surface *srcimage, float factor)
{
  if((srcimage == NULL) || (factor == 1.0)) return srcimage;
  if(factor < 1.0) return _image_scale_down(srcimage, factor);
              else return _image_scale_up(srcimage, factor);
}

/*****************************************************************************/

SDL_Surface *_image_scale_down(SDL_Surface *srcimage, float factor)
{
  SDL_Surface *newimage;
  Uint32 x, y, bpp;
  Uint32 sx, sy;
  Uint32 color;
  Uint32 r, g, b, a, n;
  Uint8 cr, cg, cb, ca;
  Uint32 newwidth  = (Uint32)(double)(srcimage->w * factor);
  Uint32 newheight = (Uint32)(double)(srcimage->h * factor);

  bpp = srcimage->format->BitsPerPixel;

  if(bpp > 8)
  {
    newimage = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 
                                    newwidth, newheight, bpp, 
                                    (bpp==32)?0xFF000000:0xF000,
                                    (bpp==32)?0x00FF0000:0x0F00,
                                    (bpp==32)?0x0000FF00:0x00F0,
                                    (bpp==32)?0x000000FF:0x000F);
  }
  else
  {
    newimage = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCCOLORKEY,
                                    newwidth, newheight, 16, 0,0,0,0);
  } 

  if(newimage == NULL) 
  {
    fprintf(stderr, "ERROR: _image_scale_down: couldn't create new surface\n");
    return NULL;
  }

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: _image_scale_down: scaling image to %dx%d\n", 
          newwidth, newheight);
#endif

  for(y=0; y<newheight; y++)
  {
    for(x=0; x<newwidth; x++)
    {
      r = g = b = a = n = 0;
      for(sy=(Uint32)floor(y/factor); sy<(Uint32)floor((y+1)/factor); sy++)
      {
        for(sx=(Uint32)floor(x/factor); sx<(Uint32)floor((x+1)/factor); sx++)
        {
          color = gfx_getpixel(srcimage, sx, sy);
          if((bpp != 8) || (color != srcimage->format->colorkey))
          {
            n++;
            SDL_GetRGBA(color, srcimage->format, &cr, &cg, &cb, &ca);
#ifdef DEBUG_VV
            fprintf(stderr, "DEBUG: _image_scale_down: (%dx%d): "
                            "(%d/%d/%d/%d)\n", sx, sy, cr, cg, cb, ca);
#endif
            r += cr;
            g += cg;
            b += cb;
            a += ca;
          }
        }
      }
      if(n == 0) 
      {
#if 0
        fprintf(stderr, "ERROR: _image_scale_down: n==0 :(\n");
#endif
        ca = 0xFF;
        SDL_GetRGB(srcimage->format->colorkey, srcimage->format, 
                   &cr, &cg, &cb);
      }
      else
      {
        cr = (Uint8)(r/n);
        cg = (Uint8)(g/n);
        cb = (Uint8)(b/n);
        ca = (Uint8)(a/n);
      }
#ifdef DEBUG_VV
      fprintf(stderr, "DEBUG: _image_scale_down: RGBA: %d/%d/%d/%d\n", 
              cr, cg, cb, ca);
#endif
      color = SDL_MapRGBA(newimage->format, cr, cg, cb, ca);
      gfx_putpixel(newimage, x, y, color);
    }
  }

  /** set colorkey on 8-bit images */
  color = srcimage->format->colorkey;
  if((bpp == 8) && (color != 0))
  {
    SDL_GetRGB(color, srcimage->format, &cr, &cg, &cb);
    color = SDL_MapRGB(newimage->format, cr, cg, cb);
    SDL_SetColorKey(newimage, SDL_SRCCOLORKEY | SDL_RLEACCEL, color);
  }
  config.memory.used += mem_ofsurface(newimage);
  
  return newimage;
}

/*****************************************************************************/

SDL_Surface *_image_scale_up(SDL_Surface *srcimage, float factor)
{
  /* TODO: ... */
  return srcimage;
}

