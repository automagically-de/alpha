#ifndef _MAPLOADER_H
#define _MAPLOADER_H

typedef struct {
  Uint8 tile;
  Uint16 offx, offy;
  Uint8 size;                /* lo 4 bit (0..3): width, hi 4 bit (4..7): height */
} MapTileData;

typedef struct {
  MapTileData *level1;
} MapTile;

int maploader_load(const char *filename, Uint8 fatal);

#endif
