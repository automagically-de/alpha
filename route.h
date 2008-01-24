#ifndef _ROUTE_H
#define _ROUTE_H

#include "SDL.h"
#include "waypoint.h"

Waypoint *route_addwaypoint(Sint32 x, Sint32 y, Sint8 height, Uint16 dir);
Waypoint *route_getwaypoint(Sint32 x, Sint32 y, Sint8 height, Uint16 dir);
int route_addwaypoints(Waypoint **waypoints, Uint32 num_waypoints,
                       Sint32 offx, Sint32 offy, Sint8 offh);
int route_draw(void);

#endif /* _ROUTE_H */
