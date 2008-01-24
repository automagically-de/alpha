#ifndef _IMAGECACHE_H
#define _IMAGECACHE_H

int icache_listcache(void);
SDL_Surface *icache_getimage(const char *filename);
SDL_Surface *icache_getimage_scaled(const char *name, float scale);

#endif
