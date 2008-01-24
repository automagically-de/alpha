#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "game.h"
#include "conf.h"
#include "mapengine.h"
#include "gui.h"
#include "video.h"
#include "imagecache.h"
#include "calc.h"
#include "minimap.h"
#include "message.h"
#include "text.h"
#include "tooltip.h"
#include "audio.h"

int _game_loop(void);
int _game_handleevents(void);

/*****************************************************************************/

int game_new(const char *mapname)
{
  mapengine_init(mapname);
  gui_init();

  if((config.map.pixels.width < config.window.width) ||
     (config.map.pixels.height < config.window.height))
    config.flags |= FLAG_MAPTOSMALL;

  /** start at center of map */
  config.pos.x = (config.map.pixels.width / 2) - (config.window.width / 2);
  config.pos.y = (config.map.pixels.height / 2) - (config.window.height / 2);
  /** if map size < window size */
  if(config.flags & FLAG_MAPTOSMALL) config.pos.x = config.pos.y = 0;
  /** don't scroll on start */
  SDL_WarpMouse(config.window.width / 2, config.window.height / 2);

  _game_loop();

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _game_loop(void)
{
  SDL_Rect mrect;
  Uint32 wfps, dwait;
  Uint32 tStart, tEnd;
  Uint32 ticks = 0, acticks = 0, frames = 0, rfps = 0;
#ifdef DELAY
  Sint32 wait;
#endif
  Sint32 mapx, mapy;             /** tile coordinates          */
#ifdef DEBUG
  char text[1024];
#endif

  config.mouse.cursor = icache_getimage("cursor.png");

  mrect.w = config.mouse.cursor->w;
  mrect.h = config.mouse.cursor->h;

  wfps = 25;
  dwait = 1000 / wfps;

  while(!(config.flags & FLAG_QUIT))
  { 
    tStart = SDL_GetTicks();
    _game_handleevents();
    /** scrolling */
    SDL_GetMouseState(&config.mouse.x, &config.mouse.y);

    calc_screentomap(config.mouse.x, config.mouse.y, 
                     config.pos.x, config.pos.y, &mapx, &mapy);
    
    /** scroll left */
    if((config.mouse.x < config.scrolling.borderwidth) &&
       (calc_screentomap(config.window.width / 2, config.window.height / 2,
                         config.pos.x-config.scrolling.distancex, config.pos.y, NULL, NULL)
        != EXIT_FAILURE))
    {   
      config.pos.x -= config.scrolling.distancex;
      calc_checkposition(&config.pos.x, &config.pos.y);
    }
    /** scroll right */
    if((config.mouse.x > 
        (config.window.width - config.scrolling.borderwidth)) &&
       (calc_screentomap(config.window.width / 2, config.window.height / 2,
                         config.pos.x+config.scrolling.distancex, config.pos.y, NULL, NULL)
        != EXIT_FAILURE))
    {
      config.pos.x += config.scrolling.distancex;
      calc_checkposition(&config.pos.x, &config.pos.y);
    }
    /** scroll up */
    if((config.mouse.y < config.scrolling.borderwidth) &&
       (calc_screentomap(config.window.width / 2, config.window.height / 2,
                         config.pos.x, config.pos.y-config.scrolling.distancey, NULL, NULL)
        != EXIT_FAILURE))
    {
      config.pos.y -= config.scrolling.distancey;
      calc_checkposition(&config.pos.x, &config.pos.y);
    }
    /** scroll down */
    if((config.mouse.y > 
        (config.window.height - config.scrolling.borderwidth)) &&
       (calc_screentomap(config.window.width / 2, config.window.height / 2,
                         config.pos.x, config.pos.y+config.scrolling.distancey, NULL, NULL)
        != EXIT_FAILURE))
    {
      config.pos.y += config.scrolling.distancey;
      calc_checkposition(&config.pos.x, &config.pos.y);
    }

    /** draw map */
    if(config.flags & FLAG_MAPTOSMALL)
    {
      SDL_FillRect(config.window.surface, NULL,
                   SDL_MapRGB(config.window.surface->format,
                              0x00, 0x00, 0x00));
    }
    mapengine_draw(config.pos.x, config.pos.y,
                   config.window.width, config.window.height);
    minimap_draw(config.pos.x, config.pos.y);
    message_draw();
    gui_draw();

#ifdef DEBUG
#ifdef HAVE_SNPRINTF
    snprintf(text, 1024, "%u fps", rfps);
#else
    sprintf(text, "%u fps", rfps);
#endif
    text_draw(config.window.surface, icache_getimage("font.png"), 2, 26, text);

#ifdef HAVE_SNPRINTF
    snprintf(text, 1024, "memory usage: %u kBytes",
             config.memory.used / 1024);
#else
    sprintf(text, "memory usage: %u kBytes", config.memory.used / 1024);
#endif
    text_draw(config.window.surface, icache_getimage("font.png"), 2, 36, text);

#ifdef HAVE_SNPRINTF
    snprintf(text, 1024, "mouse at (%d,%d)", mapx, mapy);
#else
    sprintf(text, "mouse at (%d,%d)", mapx, mapy);
#endif
    text_draw(config.window.surface, icache_getimage("font.png"), 2, 46, text);
#endif /* ifdef DEBUG */

    mrect.x = config.mouse.x;
    mrect.y = config.mouse.y;

    tooltip_draw(mrect.x + 32, mrect.y + 32);

    SDL_BlitSurface(config.mouse.cursor, NULL, config.window.surface, &mrect);
    SDL_Flip(config.window.surface);

    tEnd = SDL_GetTicks();

    ticks = tEnd;
    if((ticks - acticks) > 1000)
    {
      acticks = ticks;
      rfps = frames;
      frames = 0;
    }
    else
    {
      frames++;
    }
#ifdef DELAY
    wait = dwait - (tEnd-tStart);
    if(wait > 5) SDL_Delay(wait);
#endif    
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _game_handleevents(void)
{
  SDL_Event event;

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYUP:
        switch(event.key.keysym.sym)
        {
          case SDLK_q:
          case SDLK_ESCAPE:
            config.flags |= FLAG_QUIT;
            break;
          case SDLK_KP_ENTER:
          case SDLK_RETURN:
            if(event.key.keysym.mod & KMOD_ALT)
            {
              video_togglefullscreen();
            }
            break;
          case SDLK_n:
            audio_playnext();
            break;
          case SDLK_m:
            audio_togglemusic();
            break;
          case SDLK_l:
            icache_listcache();
            break;
          default:
            break;
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        switch(event.button.button)
        {
          case 1:
          case 3:
            config.mouse.cursor = icache_getimage("cursorp.png");
            break;
          default:
            break;
        }
        if(gui_checkclick(event.button.x, event.button.y,
                          event.button.button, 0)) break;
        switch(event.button.button)
        {
          case 1:
            if(minimap_checkclick(event.button.x, event.button.y,
                                  (Uint32*)&config.pos.x, (Uint32*)&config.pos.y))
            {
              config.flags |= FLAG_MMDRAG;
            }
            break;
          case 2:
            break;
          case 3:
            config.flags |= FLAG_DRAGMOVE;
            break;
          case 4: /* mwheel up */
            break;
          case 5: /* mwheel down */
            break;
          default:
            break;
        }
        break;

      case SDL_MOUSEBUTTONUP:
        /** actions always done */
        config.mouse.cursor = icache_getimage("cursor.png");
        switch(event.button.button)
        {
          case 1:
            config.flags &= ~FLAG_MMDRAG;
          case 3:
            config.flags &= ~FLAG_DRAGMOVE;
            break;
        }
        /** actions done in special windows */
        if(gui_checkclick(event.button.x, event.button.y,
                          event.button.button, 1)) break;
        /** actions done in main window */
        switch(event.button.button)
        {
          case 1: /* left button */
            break;
          case 2: /* middle button */
            break;
          case 3: /* right button */
            break;
          case 4: /* mwheel up */
            break;
          case 5: /* mwheel down */
            break;
          default:
            break;
        }
        break;

      case SDL_MOUSEMOTION:
        gui_mousemove(event.motion.x, event.motion.y,
                      event.motion.xrel, event.motion.yrel);
        /** coordinates for mouse cursor */
        config.mouse.x = event.motion.x;
        config.mouse.y = event.motion.y;
        if(config.flags & FLAG_MMDRAG)
        {
          minimap_checkclick(event.motion.x, event.motion.y,
                             (Uint32*)&config.pos.x, (Uint32*)&config.pos.y);
        }
        if(config.flags & FLAG_DRAGMOVE)
        {
          /* FIXME: "smooth scrolling" at border... */
          if(config.scrolling.invert)
          {
            if(calc_screentomap(config.window.width / 2,
                                config.window.height / 2,
                                config.pos.x + event.motion.xrel,
                                config.pos.y + event.motion.yrel,
                                NULL, NULL) != EXIT_FAILURE)
            {
              config.pos.x += event.motion.xrel;
              config.pos.y += event.motion.yrel;
            }
          }
          else
          {
            if(calc_screentomap(config.window.width / 2,
                                config.window.height / 2,
                                config.pos.x - event.motion.xrel,
                                config.pos.y - event.motion.yrel,
                                NULL, NULL) != EXIT_FAILURE)
            {
              config.pos.x -= event.motion.xrel;
              config.pos.y -= event.motion.yrel;
            }
          }
          calc_checkposition(&config.pos.x, &config.pos.y);
        }
        break;

      case SDL_VIDEORESIZE:
        video_resize(event.resize.w, event.resize.h);
        gui_resize(config.window.width, config.window.height);
        calc_checkposition(&config.pos.x, &config.pos.y);
        break;

      case SDL_QUIT:
        config.flags |= FLAG_QUIT;
        break;

      default:
        break;
    }
  }
  return EXIT_SUCCESS;
}
