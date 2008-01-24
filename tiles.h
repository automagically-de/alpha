#ifndef _TILES_H
#define _TILES_H

#include "SDL.h"

typedef struct {
  char *filename;
  Uint32 sizex, sizey;
  char *minimap_map;
  Uint32 num_waypoints;
  void **waypoints;
} TileInfo;

/*****************************************************************************/

int tiles_init(void);
int tiles_decode(const char codedchar, TileInfo *tileinfo);

#endif
