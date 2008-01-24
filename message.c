#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "message.h"
#include "misc.h"
#include "text.h"
#include "mem.h"
#include "imagecache.h"

int _message_update(void);

/*****************************************************************************/

int message_add(const char *text, Uint32 flags)
{
  MessageEntry *new;

  new = misc_malloc(sizeof(MessageEntry));

  new->message = misc_string_copym(text);
  new->flags = flags;
  new->next = config.message.queue;

  config.message.queue = new;

  config.message.visnum++;
  if(config.message.visnum > 6) config.message.visnum = 6;

  _message_update();
  
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int message_draw(void)
{
  SDL_Rect rect;
  static Uint32 interval = 0;

  if(config.message.surface == NULL) return EXIT_SUCCESS;
  if(config.message.visnum == 0) return EXIT_SUCCESS;

  rect.x = 160;
  rect.y = 24;
  rect.w = config.message.surface->w;
  rect.h = config.message.visnum * 10 + 4;

  SDL_BlitSurface(config.message.surface, NULL, config.window.surface, &rect);

  interval++;
  if(interval > 250)
  {
    if(config.message.visnum > 0)
    {
      config.message.visnum--;
      _message_update();
    }
    interval = 0;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _message_update(void)
{
  MessageEntry *message = (MessageEntry*)config.message.queue;
  SDL_Rect rect;
  int numlines = 0;
  Uint32 bgcolor;

  if(config.message.surface == NULL)
  {
    config.message.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                  config.window.width - 160, 
                                                  64, 32, 0,0,0,0);
    if(config.message.surface == NULL) return EXIT_FAILURE;

    if(config.performance.alpha)
    {
      SDL_SetAlpha(config.message.surface, SDL_SRCALPHA | SDL_RLEACCEL, 0xA0);
    }
    config.memory.used += mem_ofsurface(config.message.surface);
  }

  bgcolor = SDL_MapRGB(config.message.surface->format, 0xFF,0x80,0xFF);
  SDL_FillRect(config.message.surface, NULL, bgcolor);
  SDL_SetColorKey(config.message.surface, SDL_SRCCOLORKEY, bgcolor);

  if(config.message.visnum > 0)
  {
    rect.x = 0;
    rect.y = 0;
    rect.w = config.message.surface->w;
    rect.h = config.message.visnum * 10 + 4;
    bgcolor = SDL_MapRGB(config.message.surface->format, 0x40,0x40,0x40);
    SDL_FillRect(config.message.surface, &rect, bgcolor);
  }

  while((numlines < config.message.visnum) && (message != NULL))
  {
    text_draw(config.message.surface, icache_getimage("font_6x10_white.png"),
              2, (config.message.visnum - numlines) * 10 - 8, 
              message->message);
    message = (MessageEntry*)message->next;
    numlines++;
  }

  return EXIT_SUCCESS;
}

