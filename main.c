#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "video.h"
#include "audio.h"
#include "main.h"
#include "game.h"
#include "imagecache.h"

void sighandler(int sig);

/* test */

/*****************************************************************************/

int main(int argc, char **argv)
{
  conf_init(&argc, &argv);

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE
#ifdef HAVE_SDLMIXER
           | SDL_INIT_AUDIO
#endif
          );

  atexit(SDL_Quit);

  signal(SIGINT,  sighandler);
  signal(SIGTERM, sighandler); 
  signal(SIGSEGV, sighandler);

  /** initialize video */
  video_init();

  /** initialize audio */
  audio_init();

  game_new(config.map.name);
  return EXIT_SUCCESS;
}

/*****************************************************************************/

void sighandler(int sig)
{
  if((sig == SIGTERM) || (sig == SIGINT))
  {
    exit(EXIT_FAILURE);
  }
  if(sig == SIGSEGV)
  {
    if(config.flags & FLAG_WARNED)
    {
      fprintf(stderr, "You have been warned! ");
    }
    fprintf(stderr, "Segmentation fault. Come on, fix it!\n");
    exit(EXIT_FAILURE);
  }
}

