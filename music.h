#ifndef _MUSIC_H
#define _MUSIC_H

#ifdef HAVE_SDLMIXER
#include "SDL_mixer.h"
#endif

typedef struct {
  char *name;
#ifdef HAVE_SDLMIXER
  Mix_Music *mixmusic;
#endif
  void *next;
} MusicListEntry;

typedef struct {
  MusicListEntry *top;
  MusicListEntry *playing;
} MusicList;

/*****************************************************************************/

MusicList *music_loadlist(const char *plsname);
int music_playnext(void);
int music_toggle(void);

#endif
