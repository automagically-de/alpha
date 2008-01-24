#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#ifdef HAVE_SDLMIXER
#include "SDL_mixer.h"
#endif

#include "conf.h"
#include "audio.h"
#include "music.h"

int audio_init(void)
{
#ifdef HAVE_SDLMIXER
  int retval;

  if(!config.sound.enabled) return EXIT_SUCCESS;

#define NR_OF_CHANNELS 2

  retval = Mix_OpenAudio(config.sound.frequency, 
                         MIX_DEFAULT_FORMAT, NR_OF_CHANNELS, 1024);
  atexit(Mix_CloseAudio);

#ifdef DEBUG
  fprintf(stderr, "DEBUG: audio_init: Mix_OpenAudio() = %d\n", retval);
#endif
  if(retval != EXIT_SUCCESS)
  {
    fprintf(stderr, "ERROR: couldn't open audio\n");
    exit(EXIT_FAILURE);
  }

  config.sound.musiclist = music_loadlist("music.pls");
  if(config.sound.music) music_playnext();

#endif
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int audio_playnext(void)
{
#ifdef HAVE_SDLMIXER
  if(!config.sound.enabled) return EXIT_FAILURE;

  music_playnext();
#endif  
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int audio_togglemusic(void)
{
#ifdef HAVE_SDLMIXER
  if(!config.sound.enabled) return EXIT_FAILURE;

  return music_toggle();
#else
  return EXIT_SUCCESS;
#endif
}


