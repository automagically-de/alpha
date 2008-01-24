#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#ifdef HAVE_SDLMIXER
#include "SDL_mixer.h"
#endif

#include "mem.h"
#include "conf.h"
#include "music.h"

int _music_load(MusicListEntry *entry);
int _music_play(MusicListEntry *entry);
void _music_playfinished_cb(void);

/*****************************************************************************/

MusicList *music_loadlist(const char *plsname)
{
#ifdef HAVE_SDLMIXER
  FILE *pls;
  char buffer[1024];
  char path[1024];
  MusicList *list = (MusicList*)malloc(sizeof(MusicList));
  MusicListEntry *new, *entry;

#ifdef HAVE_SNPRINTF
  snprintf(path, 1024, DATADIR "/music/%s", plsname);
#else
  sprintf(path, DATADIR "/music/%s", plsname);
#endif

#ifdef DEBUG
  fprintf(stderr, "DEBUG: music_loadlist: loading '%s'\n", path);
#endif
  list->playing = NULL;
  list->top = NULL;
  config.memory.used += sizeof(MusicList);

  pls = fopen(path, "r");
  if(pls == NULL) return NULL;

  while(fgets(buffer, 1024, pls) != NULL)
  {
    if(buffer[strlen(buffer)-1] == '\n') buffer[strlen(buffer)-1] = '\0';
    if(strlen(buffer) == 0) continue;

    /** create entry */
    new = (MusicListEntry*)malloc(sizeof(MusicListEntry));
    new->name = (char*)malloc(strlen(buffer)+1);
    strcpy(new->name, buffer);
    new->mixmusic = NULL;
    new->next = NULL;
    config.memory.used += sizeof(MusicListEntry);
    config.memory.used += strlen(buffer) + 1;

    /** add to list */
    entry = list->top;
    if(entry == NULL)
    {
      list->top = new;
    }
    else
    {
      while(entry->next != NULL) entry = (MusicListEntry*)entry->next;
      entry->next = new;
    }
  }
  fclose(pls);

  return list;
#else
  return NULL;
#endif /* HAVE_SDLMIXER */
}

/*****************************************************************************/

int music_playnext(void)
{
#ifdef HAVE_SDLMIXER
  MusicList *list = (MusicList*)config.sound.musiclist;
  MusicListEntry *entry;

  if(list == NULL) return EXIT_FAILURE;

  entry = list->playing;
  if(entry == NULL)
  {
    /** not playing */
    entry = list->top;
    if(entry == NULL)
    {
      /** list empty */
      return EXIT_FAILURE;
    }
  }
  else
  {
    entry = (MusicListEntry*)entry->next;
    if(entry == NULL)
    {
      /** was last in list */
      entry = list->top;
      if(entry == NULL)
      {
        /** list empty */
        return EXIT_FAILURE;
      }
    }
  }

  list->playing = entry;
  _music_load(entry);
  _music_play(entry);
#endif
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int music_toggle(void)
{
#ifdef HAVE_SDLMIXER
  if(Mix_PlayingMusic()) Mix_HaltMusic();
  else music_playnext();
#endif
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _music_load(MusicListEntry *entry)
{
#ifdef HAVE_SDLMIXER
  char path[1024];

  /** already loaded */
  if(entry->mixmusic != NULL) return EXIT_SUCCESS;
#ifdef HAVE_SNPRINTF 
  snprintf(path, 1024, DATADIR "/music/%s", entry->name);
#else
  sprintf(path, DATADIR "/music/%s", entry->name);
#endif

#ifdef DEBUG
  fprintf(stderr, "DEBUG: _music_load: loading '%s'\n", path);
#endif

  entry->mixmusic = Mix_LoadMUS(path);
  /* FIXME: get memory of Mix_Music */
#endif
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _music_play(MusicListEntry *entry)
{
#ifdef HAVE_SDLMIXER
  if(Mix_PlayingMusic())
  {
    /** fading ?? */
    Mix_HaltMusic();
  }
  Mix_PlayMusic(entry->mixmusic, 1);
  Mix_HookMusicFinished(_music_playfinished_cb);
#endif
  return EXIT_SUCCESS;
}

/*****************************************************************************/

void _music_playfinished_cb(void)
{
  music_playnext();
}


