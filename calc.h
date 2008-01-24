#ifndef _CALC_H
#define _CALC_H

#include "SDL.h"

int calc_screentomap(Uint32 screenx, Uint32 screeny,
                     Uint32 offsetx, Uint32 offsety,
                     Sint32 *mapx, Sint32 *mapy);
int calc_maptiletoscreen(Uint32 mapx, Uint32 mapy, Uint8 tx, Uint8 ty,
                         Sint32 *screenx, Sint32 *screeny);
int calc_checkposition(Sint32 *posx, Sint32 *posy);

#endif
