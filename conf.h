#ifndef _CONF_H
#define _CONF_H

#include "SDL.h"
#include "list.h"

typedef struct {
  struct {
    Uint32 width;
    Uint32 height;
    Uint8 fullscreen;
    Uint8 resizable;
    SDL_Surface *surface;
  } window;

#ifdef HAVE_SDLMIXER
  struct {
    Uint8 enabled;
    Uint8 music;
    Uint16 frequency;
    void *musiclist;
  } sound;
#endif

  struct {
    struct {
      Uint32 width;
      Uint32 height;
    } tile;

    struct {
      Uint32 width;
      Uint32 height;
    } pixels;

    char *name;
    void **buffer;
    float scale;
    Uint8 *heights;
    Uint32 width;
    Uint32 height;
  } map;

  struct {
    Uint8 alpha;
    Sint32 background;
    double scalex;
    double scaley;
    SDL_Surface *ground;
    SDL_Surface *tracks;
  } minimap;

  struct {
    Uint32 width;
    Uint32 height;
    Uint32 keep_invis;
    Uint32 num_x;
    Uint32 num_y;
  } buffer;

  struct {
    Sint32 x, y;
  } pos;

  struct {
    SDL_Surface *surface;
    Uint8 enabled;
  } tooltip;

  struct {
    SDL_Surface *surface;
    Uint32 visnum;
    void *queue;
  } message;

  struct {
    Uint32 borderwidth;
    Uint32 distancex;
    Uint32 distancey;
    Uint8 invert;
  } scrolling;

  struct {
    Uint32 max;
    Uint32 used;
  } memory;

  struct {
    Uint8 alpha;
  } performance;

  struct {
    Sint32 x, y;
    SDL_Surface *cursor;
  } mouse;

  struct {
    void *imagecache;
    void *tileinfos;
  } caches;

  struct {
    List *waypoints;
    List *railblocks;
  } routing;
  
  char *progname;
  Uint32 flags;

} ConfStruct;

extern ConfStruct config;

int conf_init(int *argcp, char ***argvp);

#endif
