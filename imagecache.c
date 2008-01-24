#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_STAT
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "conf.h"
#include "mem.h"
#include "misc.h"
#include "image_scale.h"

struct ICImage 
{
  char *name;
  SDL_Surface *surface;
  void *next;
};

/*****************************************************************************/

int icache_listcache(void)
{
  struct ICImage *top = (struct ICImage *)config.caches.imagecache;

  fprintf(stderr, "INFO: ,-- listing image cache...\n");
  while(top != NULL)
  {
    fprintf(stderr, "INFO: | %-55s(%dx%dx%d)\n",
            top->name, top->surface->w, top->surface->h,
            top->surface->format->BitsPerPixel);
    top = (struct ICImage *)top->next;
  }

  return EXIT_SUCCESS;
}

/*******************************************++********************************/

SDL_Surface *icache_getimage(const char *filename)
{
  struct ICImage *top = (struct ICImage *)config.caches.imagecache;
  struct ICImage *iptr;
  char path[1024];
  SDL_Surface *surface;
#ifdef HAVE_STAT
  struct stat stats;
#endif

  iptr = top;
  while(iptr != NULL)
  {
    /** already in cache -> return */
    if(strcmp(filename, iptr->name) == 0) return iptr->surface;
    /** next try */
    iptr = (struct ICImage *)iptr->next;
  }

  /* Don't try to load (to be) scaled images... */
  if(strstr(filename, "::") != NULL) return NULL;

  /** load image */
#ifdef HAVE_SNPRINTF
  snprintf(path, 1024, DATADIR "/images/%s", filename);
#else
  sprintf(path, DATADIR "/images/%s", filename);
#endif
#ifdef HAVE_STAT
  if(stat(filename, &stats) == ENOENT)
  {
    return NULL;
  }
#endif
  surface = IMG_Load(path);
  if(surface == NULL)
  {
    /** error loading image, exit() ? */
    fprintf(stderr, "ERROR: imagecache: couldn't load '%s'\n", filename);
    return NULL;
  }

#ifdef DEBUG
  fprintf(stderr, "DEBUG: imagecache: loading image \"%s\" (%d bpp)\n", 
          filename, surface->format->BitsPerPixel);
#endif

#undef CONVERT_TO_SCREENFORMAT
#ifdef CONVERT_TO_SCREENFORMAT
  if(surface->format->BitsPerPixel == 32)
  {
    surface = SDL_DisplayFormatAlpha(surface);
#ifdef DEBUG
    fprintf(stderr, "DEBUG: imagecache: converting 32-bit surface to %d bit\n",
            surface->format->BitsPerPixel);
#endif    
  }
#endif

  /** not in cache, generate new entry */
  iptr = misc_malloc(sizeof(struct ICImage));
  iptr->surface = surface;
  iptr->name = misc_string_copym(filename);

  iptr->next = top;
  top = iptr;

  /** get memory of whole entry */
  if(iptr->surface != NULL)
  {
    config.memory.used += mem_ofsurface(iptr->surface);
  }

  config.caches.imagecache = top;
  return iptr->surface;
}

/*****************************************************************************/

SDL_Surface *icache_getimage_scaled(const char *name, float scale)
{
  SDL_Surface *surface, *scaled;
  struct ICImage *iptr;
  char text[1024];
  
  if(scale == 1.0)
  {
    return icache_getimage(name);
  }

#ifdef HAVE_SNPRINTF
  snprintf(text, 1024, "%s::%f", name, scale);
#else
  sprintf(text, "%s::%f", name, scale);
#endif

  surface = icache_getimage(text);
  if(surface != NULL) return surface;

  surface = icache_getimage(name);
  if(surface == NULL) return NULL;

  scaled = image_scale(surface, scale);
 
  iptr = misc_malloc(sizeof(struct ICImage));
  iptr->surface = scaled;
  iptr->name = misc_string_copym(text);

  iptr->next = config.caches.imagecache;
  config.caches.imagecache = iptr;

  if(iptr->surface != NULL)
  {
    config.memory.used += mem_ofsurface(iptr->surface);
  }

  return iptr->surface;
}


