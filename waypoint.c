#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "tiles.h"
#include "waypoint.h"

int waypoint_addtotile(TileInfo *tileinfo, char *wpdata)
{
  Uint32 memsize;
  Uint32 n;
  unsigned int wpx, wpy, wpd;
  int wph;
  Waypoint *waypoint;

  if(sscanf(wpdata, "%u,%u,%u,%d", &wpx, &wpy, &wpd, &wph) != 4)
  {
    fprintf(stderr, "ERROR: parsing waypoint data failed (\"%s\")\n",
            wpdata);
    return EXIT_FAILURE;
  }

  if(tileinfo == NULL) return EXIT_FAILURE;

  n = tileinfo->num_waypoints;
  tileinfo->num_waypoints ++;
  memsize = tileinfo->num_waypoints * sizeof(void *);

  tileinfo->waypoints = realloc(tileinfo->waypoints, memsize);
  if(tileinfo->waypoints == NULL) return EXIT_FAILURE;

  tileinfo->waypoints[n] = malloc(sizeof(Waypoint));
  if(tileinfo->waypoints[n] == NULL) return EXIT_FAILURE;

  waypoint = (Waypoint*)(tileinfo->waypoints[n]);

#ifdef DEBUG
  fprintf(stderr, "DEBUG: waypoint_addtotile: new \"%s\" (%u)\n", wpdata, n+1);
#endif

  waypoint->x = wpx;
  waypoint->y = wpy;
  waypoint->direction = wpd;
  waypoint->height = wph;
  waypoint->num_routes = 0;
  waypoint->routes = NULL;
  waypoint->railblock = -1;

  config.memory.used += sizeof(void *);
  config.memory.used += sizeof(Waypoint);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int waypoint_addroute(TileInfo *tileinfo, char *route)
{
  Uint32 memsize;
  Uint32 n;
  unsigned int w1, w2;
  Waypoint *wp1, *wp2;

  if(sscanf(route, "%u-%u", &w1, &w2) != 2)
  {
    fprintf(stderr, "ERROR: parsing route data failed (\"%s\")\n", route); 
    return EXIT_FAILURE;
  }

  if(tileinfo == NULL) return EXIT_FAILURE;

  if((w1 > tileinfo->num_waypoints) || (w2 > tileinfo->num_waypoints))
  {
    fprintf(stderr, "ERROR: waypoint(s) do(es)n't exist (\"%s\")\n", route);
    return EXIT_FAILURE;
  }

#ifdef DEBUG
  fprintf(stderr, "DEBUG: waypoint_addroute: adding route from %d to %d\n",
          w1, w2);
#endif

  wp1 = (Waypoint*)tileinfo->waypoints[w1 - 1];
  wp2 = (Waypoint*)tileinfo->waypoints[w2 - 1];

  n = wp1->num_routes;
  wp1->num_routes ++;
  memsize = wp1->num_routes * sizeof(void *);
  wp1->routes = realloc(wp1->routes, memsize);

  wp1->routes[n] = wp2;

  config.memory.used += sizeof(void *);

  return EXIT_SUCCESS;
}

