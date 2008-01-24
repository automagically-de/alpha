#ifndef _WAYPOINT_H
#define _WAYPOINT_H

#include "SDL.h"

typedef struct {
  Uint32 x, y;
  Uint16 direction;
  Sint8 height;
  Uint8 num_routes;
  void **routes;
  Sint32 railblock;
} Waypoint;

/*****************************************************************************/

#include "tiles.h"

int waypoint_addtotile(TileInfo *tileinfo, char *wpdata);
int waypoint_addroute(TileInfo *tileinfo, char *route);

#endif /* _WAYPOINT_H */
