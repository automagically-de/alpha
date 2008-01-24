#include <stdlib.h>

#include "SDL.h"

#include "conf.h"

/** returns EXIT_SUCCESS if given position is on map, EXIT_FAILURE else */
int calc_screentomap(Uint32 screenx, Uint32 screeny,
                     Uint32 offsetx, Uint32 offsety,
                     Sint32 *mapx, Sint32 *mapy)
{
  Sint32 sx, sy;
  double mx, my;

  /** (sx,sy) => (0,0) */
  sx = (signed)screenx + (signed)offsetx - 
       (signed)config.map.height * ((signed)config.map.tile.width) / 2;
  sy = (signed)screeny + (signed)offsety;

  mx = ((float)sx / ((float)config.map.tile.width)) +
       ((float)sy / ((float)config.map.tile.height));
  my = ((float)sy / (float)config.map.tile.height) -
       ((float)sx / (float)config.map.tile.width);

  if(((sy*2 - sx) < 0) || ((sy*2 + sx) < 0) ||
     (((Sint32)mx + 1) > config.map.width) ||
     (((Sint32)my + 1) > config.map.height))
  {
    if(mapx != NULL) *mapx = -1;
    if(mapy != NULL) *mapy = -1;
    return EXIT_FAILURE;
  }
  else
  {
    if(mapx != NULL) *mapx = (Sint32)mx;
    if(mapy != NULL) *mapy = (Sint32)my;
    return EXIT_SUCCESS; 
  }
}

/*****************************************************************************/

int calc_maptiletoscreen(Uint32 mapx, Uint32 mapy, Uint8 tx, Uint8 ty,
                         Sint32 *screenx, Sint32 *screeny)
{
  Uint32 offrootx;
  Sint32 x, y;
  
  offrootx = ((config.map.height - 1) * config.map.tile.width) / 2;
  x = (Uint32)((Sint32)offrootx +
               (Sint32)((mapx * config.map.tile.width) / 2) -
               (Sint32)((mapy * config.map.tile.width) / 2) -
               (Sint32)(((ty - 1) * config.map.tile.width) / 2));
  y = (mapy * (config.map.tile.height / 2)) +
      (mapx * (config.map.tile.height / 2)) -
      (config.map.heights[mapy * (config.map.width + 1) + mapx] *
       config.map.tile.height / 2);
  if(screenx != NULL) *screenx = x;
  if(screeny != NULL) *screeny = y;
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int calc_checkposition(Sint32 *posx, Sint32 *posy)
{
  if(*posx < 0) *posx = 0;
  if(*posy < 0) *posy = 0;
  if(*posx > config.map.pixels.width - config.window.width)
  {
    *posx = config.map.pixels.width - config.window.width;
  }
  if(*posy > config.map.pixels.height - config.window.height)
  {
    *posy = config.map.pixels.height - config.window.height;
  }

  return EXIT_SUCCESS;
}

