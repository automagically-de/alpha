#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "mem.h"
#include "imagecache.h"
#include "text.h"
#include "gfx_prim.h"

int _tooltip_create(const char *text);

/*****************************************************************************/

int tooltip_update(const char *text)
{
  if(!text || (strlen(text) == 0))
  {
    if(config.tooltip.surface)
    {
      config.memory.used -= mem_ofsurface(config.tooltip.surface);
      SDL_FreeSurface(config.tooltip.surface);
      config.tooltip.surface = NULL;
    }
    return EXIT_SUCCESS;
  }

  return _tooltip_create(text);
}

/*****************************************************************************/

int tooltip_draw(Sint32 x, Sint32 y)
{
  SDL_Rect rect;

  if(config.tooltip.enabled == 0) return EXIT_SUCCESS;
  if(config.tooltip.surface == NULL) return EXIT_SUCCESS;

  rect.x = x;
  rect.y = y;
  rect.w = config.tooltip.surface->w;
  rect.h = config.tooltip.surface->h;

  if((rect.x + rect.w) >= config.window.width) 
    rect.x = config.window.width - rect.w - 1;

  SDL_BlitSurface(config.tooltip.surface, NULL, config.window.surface, &rect);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _tooltip_create(const char *text)
{
#define FONT_TT  "font_6x10_black.png"
  Uint32 tw, th, bgcolor, border;

  text_getsize(icache_getimage(FONT_TT), text, &tw, &th);
  
  config.tooltip.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                tw + 6, th + 4, 16, 0,0,0,0);
  bgcolor = SDL_MapRGB(config.tooltip.surface->format, 0xFF, 0xE9, 0x40);
  border  = SDL_MapRGB(config.tooltip.surface->format, 0x00, 0x00, 0x00);

  SDL_FillRect(config.tooltip.surface, NULL, bgcolor);
  gfx_rectangle(config.tooltip.surface, 0, 0, tw + 6, th + 4, border);
  text_draw(config.tooltip.surface, icache_getimage(FONT_TT), 3, 2, text);

  config.memory.used += mem_ofsurface(config.tooltip.surface);

  return EXIT_SUCCESS;
#undef FONT_TT
}


