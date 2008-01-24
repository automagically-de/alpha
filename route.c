#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "waypoint.h"
#include "route.h"
#include "list.h"
#include "misc.h"
#include "gfx_prim.h"
#include "imagecache.h"

Waypoint *route_addwaypoint(Sint32 x, Sint32 y, Sint8 height, Uint16 dir)
{
  List *wplist = config.routing.waypoints;
  Waypoint *wp;

  if(wplist == NULL) wplist = config.routing.waypoints = list_create();

  wp = list_append(wplist, Waypoint);
  if(wp == NULL) return NULL;

  wp->x = x;
  wp->y = y;
  wp->direction = dir;
  wp->height = height;
  wp->num_routes = 0;
  wp->routes = NULL;
  wp->railblock = -1;
  return wp;
}

/*****************************************************************************/

Waypoint *route_getwaypoint(Sint32 x, Sint32 y, Sint8 height, Uint16 dir)
{
  List *wplist = config.routing.waypoints;
  Waypoint *wp;

  if(wplist == NULL) wplist = config.routing.waypoints = list_create();

  wp = list_getfirst(wplist, Waypoint);
  while(wp != NULL)
  {
    if((wp->x == x) && (wp->y == y) && (wp->height == height)) break;
    wp = list_getnext(wplist, Waypoint);
  }
  /* if dir is 0xFFFF, don't create waypoint */
  if((wp != NULL) || (dir == 0xFFFF)) return wp;
  else return route_addwaypoint(x, y, height, dir);
}

/*****************************************************************************/

int route_addwaypoints(Waypoint **waypoints, Uint32 num_waypoints,
                       Sint32 offx, Sint32 offy, Sint8 offh)
{
  int i;
  Waypoint **newwps = misc_malloc(num_waypoints * sizeof(Waypoint*));

#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: route_addwaypoints: called (%d)\n", num_waypoints);
#endif

  for(i=0; i<num_waypoints; i++)
  {
    Waypoint *wp = waypoints[i];
    Waypoint *globalwp = route_getwaypoint(wp->x + offx, wp->y + offy,
                                           wp->height + offh, wp->direction);

#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: route_addwaypoints: adding waypoint (%dx%d+%d+%d)"
                    "\n", wp->x, wp->y, offx, offy);
#endif
    if(globalwp == NULL)
    {
      fprintf(stderr, "ERROR: route_addwaypoints: couldn't get/add waypoint "
                      "(%dx%d+%d@%d)\n", wp->x + offx, wp->y + offy,
                      wp->height + offh, wp->direction);
      return EXIT_FAILURE;
    }
    newwps[i] = globalwp;
  }
  for(i=0; i<num_waypoints; i++)
  {
    int j;
    Waypoint *wp = waypoints[i];
    Waypoint *globalwp = newwps[i];
    for(j=0; j<wp->num_routes; j++)
    {
      Waypoint *routeto = (Waypoint*)wp->routes[j];
      Waypoint *globrto;
      Sint32 relx, rely;
      Sint8 relh;

      relx = routeto->x - wp->x;
      rely = routeto->y - wp->y;
      relh = routeto->height - wp->height;

      globrto = route_getwaypoint(globalwp->x + relx, globalwp->y + rely, 
                                  globalwp->height + relh, 0xFFFF);
      if(globrto == NULL)
      {
        fprintf(stderr, "ERROR: route_addwaypoints: couldn't find waypoint to "
                        "route to\n");
      }
      else
      {
        config.memory.used -= globalwp->num_routes * sizeof(Waypoint*);
        globalwp->num_routes++;
        globalwp->routes = realloc(globalwp->routes, 
                                   globalwp->num_routes * sizeof(Waypoint*));
        globalwp->routes[globalwp->num_routes - 1] = globrto;
        config.memory.used += globalwp->num_routes * sizeof(Waypoint*);
      }
    }
  }

  misc_free(newwps, num_waypoints * sizeof(Waypoint*));
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int route_draw(void)
{
  List *wplist = config.routing.waypoints;
  Waypoint *waypoint;
  SDL_Surface *flag = icache_getimage("flag_w.png");
  SDL_Rect rect;

  if(wplist == NULL) return EXIT_FAILURE;
  if(flag == NULL) return EXIT_FAILURE;

  rect.w = flag->w;
  rect.h = flag->h;

  waypoint = list_getfirst(wplist, Waypoint);
  while(waypoint != NULL)
  {
    Sint32 x = (Sint32)((double)waypoint->x * config.map.scale) - config.pos.x;
    Sint32 y = (Sint32)((double)waypoint->y * config.map.scale) - config.pos.y;

    rect.x = x;
    rect.y = y - flag->h;

    SDL_BlitSurface(flag, NULL, config.window.surface, &rect);
    waypoint = list_getnext(wplist, Waypoint);
  }
  return EXIT_SUCCESS; 
}

