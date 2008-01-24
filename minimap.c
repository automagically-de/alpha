#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "mem.h"
#include "calc.h"
#include "gfx_prim.h"
#include "minimap.h"

void minimap_init(void)
{
  Uint32 width;
  Uint32 height;
  Uint32 bgcolor;

  width = (config.map.width + config.map.height) * config.minimap.scalex+ 4;
  height = (config.map.width + config.map.height) * config.minimap.scaley+ 4;

  config.minimap.ground = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 
                                               16, 0,0,0,0);
  config.minimap.tracks = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
                                               16, 0,0,0,0);

  config.memory.used += mem_ofsurface(config.minimap.ground);
  config.memory.used += mem_ofsurface(config.minimap.tracks);

  if(config.minimap.background == -1)
  {
    /** transparent background */
    bgcolor = SDL_MapRGB(config.minimap.ground->format, 0xFF, 0x80, 0xFF);
    SDL_FillRect(config.minimap.ground, NULL, bgcolor);
    SDL_SetColorKey(config.minimap.ground, SDL_SRCCOLORKEY, bgcolor);
  }
  else
  {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: minimap_init: minimap background: 0x%8.8X\n",
                    config.minimap.background);
#endif
    /** draw border around minimap window */
    bgcolor = SDL_MapRGB(config.minimap.ground->format, 
                         (config.minimap.background & 0x00FF0000) >> 16, 
                         (config.minimap.background & 0x0000FF00) >> 8, 
                         (config.minimap.background & 0x000000FF));
    SDL_FillRect(config.minimap.ground, NULL, bgcolor);
    gfx_rectangle(config.minimap.ground, 0, 0, width, height, 
                  SDL_MapRGB(config.minimap.ground->format, 0xFF, 0xFF, 0xFF));
  }
  bgcolor = SDL_MapRGB(config.minimap.tracks->format, 0xFF, 0x80, 0xFF);
  SDL_FillRect(config.minimap.tracks, NULL, bgcolor);
  SDL_SetColorKey(config.minimap.tracks, SDL_SRCCOLORKEY, bgcolor);

  SDL_SetAlpha(config.minimap.ground, SDL_SRCALPHA, config.minimap.alpha);
  SDL_SetAlpha(config.minimap.tracks, SDL_SRCALPHA, config.minimap.alpha);
}

/*****************************************************************************/

int minimap_checkclick(Uint32 mx, Uint32 my, Uint32 *posx, Uint32 *posy)
{
  Sint32 px, py;
  SDL_Surface *minimap = config.minimap.ground;

  /** click not on minimap */
  if((mx < 4) ||
     (mx >= (4 + minimap->w)) ||
     (my < ((config.window.height-minimap->h) - 4)) ||
     (my >= (config.window.height - 4))) return 0;

  px = ((signed)mx - 4) * (signed)config.map.tile.width * 
       config.minimap.scalex / 2;
  py = ((signed)my - ((signed)config.window.height - (signed)minimap->h - 2)) *
       2 * (signed)config.map.tile.height * config.minimap.scaley;

  px -= config.window.width / 2;
  py -= config.window.height / 2;

  /** make sure that visible area is in map */
  if(px < 0) px = 0;
  if(py < 0) py = 0;
  if(px > config.map.pixels.width - config.window.width)
    px = config.map.pixels.width - config.window.width;
  if(py > config.map.pixels.height - config.window.height)
    py = config.map.pixels.height - config.window.height;

  if(calc_screentomap(config.window.width / 2, config.window.height / 2,
                      px, py, NULL, NULL) != EXIT_FAILURE)
  {
    *posx = px;
    *posy = py;
    return 1;
  } else return 0;
}

/*****************************************************************************/

void minimap_draw(Uint32 posx, Uint32 posy)
{
  SDL_Rect mrect;
  Uint32 x, y, width, height;
  SDL_Surface *minimap = config.minimap.ground;

  x = posx / config.map.tile.width * 2 * config.minimap.scalex;
  y = posy / config.map.tile.height * 2 * config.minimap.scaley;
  width = (config.window.width / config.map.tile.width * 2 + 2) * 
           config.minimap.scalex;
  height = (config.window.height / config.map.tile.height * 2 + 2) *
            config.minimap.scaley;

  mrect.x = 4;
  mrect.y = config.window.height - (minimap->h + 4);
  mrect.w = minimap->w;
  mrect.h = minimap->h;

  SDL_BlitSurface(minimap, NULL, config.window.surface, &mrect);
  SDL_BlitSurface(config.minimap.tracks, NULL, config.window.surface, &mrect);

  gfx_rectangle(config.window.surface, x+mrect.x+2, y+mrect.y+2, width, height,
                SDL_MapRGB(config.window.surface->format, 0xFF, 0xFF, 0xFF));
}

/*****************************************************************************/

int minimap_puttile(Uint32 mapx, Uint32 mapy, Uint8 r, Uint8 g, Uint8 b, 
                    Uint32 flags)
{
  Uint32 x, y;
  Uint32 color;
  SDL_Surface *minimap;

  if(flags & MINIMAP_GROUND)
  {
    minimap = config.minimap.ground;
  }
  else if(flags & MINIMAP_TRACKS)
  {
    minimap = config.minimap.tracks;
  }
  else return EXIT_FAILURE;

  x = (config.map.height + 1) + 
      ((signed)mapx - (signed)mapy) * config.minimap.scalex;
  y = 2 + (mapy + mapx) * config.minimap.scaley;
  color = SDL_MapRGB(minimap->format, r, g, b);
#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: minimap_puttile: (%u,%u): x: %u, y: %u\n",
          mapx, mapy, x, y);
#endif

  /** draw pixels */
  gfx_putpixel(minimap, x, y, color);
  gfx_putpixel(minimap, x+1, y, color);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int minimap_puttilemap(Uint32 mapx, Uint32 mapy, const char *map, 
                       Uint8 r, Uint8 g, Uint8 b, Uint32 flags)
{
  char *bufp = (char *)map;
  Uint32 x = mapx, y = mapy;

  if(!map) return EXIT_FAILURE;
  
  while(*bufp)
  {
    if(*bufp == '\n')
    {
      y++;
      x = mapx;
    }
    else
    {
      if((*bufp != '0') && (*bufp != ' '))
      {
#ifdef DEBUG_VV
        fprintf(stderr, "DEBUG: minimap_puttilemap: minimap_puttile(%u, %u, "
                        "...)\n", x, y);
#endif
        minimap_puttile(x, y, r, g, b, flags);
      }
      x++;
    }
    bufp++;
  }
  return EXIT_SUCCESS;
}

