#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "game.h"
#include "video.h"
#include "mem.h"
#include "imagecache.h"
#include "tiles.h"

int video_init(void)
{
  SDL_Surface *wmicon;

  config.window.surface = SDL_SetVideoMode(config.window.width,
                                           config.window.height, 0,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF |
               (config.window.fullscreen ? SDL_FULLSCREEN : 0) |
                (config.window.resizable ? SDL_RESIZABLE : 0));
  if(config.window.surface == 0)
  {
    fprintf(stderr, "ERROR: couldn't set video mode %dx%d\n",
            config.window.width, config.window.height);
    exit(EXIT_FAILURE);
  }

  SDL_ShowCursor(0);
  SDL_WM_SetCaption("IsoTest Vol.2", "IsoTest");
  wmicon = icache_getimage("wmicon.png");
  SDL_SetColorKey(wmicon, SDL_SRCCOLORKEY, 
                  SDL_MapRGB(wmicon->format, 0xFF, 0x80, 0xFF));
  SDL_WM_SetIcon(wmicon, NULL);

  config.memory.used += mem_ofsurface(config.window.surface);  

  tiles_init();

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int video_togglefullscreen(void)
{
  config.window.fullscreen ^= 1;
  config.window.surface = SDL_SetVideoMode(config.window.width,
                                           config.window.height, 0,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF |
               (config.window.fullscreen ? SDL_FULLSCREEN : 0) | 
                (config.window.resizable ? SDL_RESIZABLE : 0));
  return config.window.fullscreen;
}

/*****************************************************************************/

int video_resize(Uint32 width, Uint32 height)
{
  /* FIXME: no way to cancel a resize event? */
  if((width < 640) || (height < 480)) 
  {
    config.flags |= FLAG_WARNED;
    fprintf(stderr, "WARNING: don't make the window smaller than 640x480\n");
    /** warn and SEGV or just don't notice */
    /*
    return EXIT_FAILURE;
    */
  }

  config.window.width = width;
  config.window.height = height;

  config.window.surface = SDL_SetVideoMode(config.window.width,
                                           config.window.height, 0,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF |
               (config.window.fullscreen ? SDL_FULLSCREEN : 0) |
                (config.window.resizable ? SDL_RESIZABLE : 0));

  return EXIT_SUCCESS;
}

