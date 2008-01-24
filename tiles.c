#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "tiles.h"
#include "conf.h"
#include "misc.h"
#include "parse.h"
#include "vfs.h"
#include "waypoint.h"

#define MAX_LINE_LENGTH 4096

typedef struct {
  char tile;
  TileInfo tileinfo;
  void *next;
} TileListEntry;

/*****************************************************************************/

int tiles_init(void)
{
  char svar[MAX_LINE_LENGTH + 1], 
       sval[MAX_LINE_LENGTH + 1],
       line[MAX_LINE_LENGTH + 1];
  vfs_FILE *conffile;
  int retval, linenum = 0;
  TileListEntry *new = NULL;
  char *filename = misc_string_copym(DATADIR "/tiles.conf");

  conffile = vfs_fopen(filename, "r");
  if(conffile == NULL)
  {
    return EXIT_FAILURE;
  }
  while(vfs_fgets(line, MAX_LINE_LENGTH, conffile))
  {
    linenum++;
    retval = parse_line(line, svar, MAX_LINE_LENGTH, sval, MAX_LINE_LENGTH);
    if(retval == EXIT_SUCCESS)
    {
      if(strlen(svar))
      {
#ifdef DEBUG_VV
        fprintf(stderr, "DEBUG: tiles_init: %s: '%s'\n", svar, sval);
#endif
        if(strcmp(svar, "tile") == 0)
        {
          new = misc_malloc(sizeof(TileListEntry));
          new->tile = sval[0];
          new->tileinfo.sizex = 1;
          new->tileinfo.sizey = 1;
          new->tileinfo.filename = NULL;
          new->tileinfo.minimap_map = NULL;
          new->tileinfo.num_waypoints = 0;
          new->tileinfo.waypoints = NULL;
          new->next = config.caches.tileinfos;
          config.caches.tileinfos = new;
        }
        else if(new != NULL)
        {
          char *errp = NULL;

          if(strcmp(svar, "pixmap") == 0)
          {
            new->tileinfo.filename = misc_string_copym(sval);
          }
          else if(strcmp(svar, "minimap") == 0)
          {
            new->tileinfo.minimap_map = misc_string_copym(sval);
          }
          else if(strcmp(svar, "width") == 0)
          {
            new->tileinfo.sizex = strtoul(sval, &errp, 0);
          }
          else if(strcmp(svar, "height") == 0)
          {
            new->tileinfo.sizey = strtoul(sval, &errp, 0);
          }
          else if(strcmp(svar, "waypoint") == 0)
          {
            waypoint_addtotile(&(new->tileinfo), sval);
          }
          else if(strcmp(svar, "route") == 0)
          {
            waypoint_addroute(&(new->tileinfo), sval);
          }

          /* error in number conversion */
          if(errp && strlen(errp))
          {
            fprintf(stderr, "ERROR: tiles_init: error converting num value in "
                            "%s, line %d\n", filename, linenum);
          }
        }       /* ! tile */
      }         /* strlen(svar) */
    }           /* retval == EXIT_SUCCESS */
    else
    {
      fprintf(stderr, "ERROR: tiles_init: parse error in %s, line %d (E: %d)"
                      "\n", filename, linenum, retval);
    }
  }
  vfs_fclose(conffile);

#ifdef DEBUG
  fprintf(stderr, "DEBUG: tiles_init: leaving function...\n");
  new = (TileListEntry*)config.caches.tileinfos;
  while(new)
  {
    fprintf(stderr, "DEBUG: | '%c': %s\n", new->tile, new->tileinfo.filename);
    new = (TileListEntry*)new->next;
  }
#endif

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int tiles_decode(const char codedchar, TileInfo *tileinfo)
{
  TileListEntry *entry = (TileListEntry*)config.caches.tileinfos;

  if(tileinfo == NULL) return EXIT_FAILURE;

  if(tileinfo->filename)    misc_string_free(tileinfo->filename);
  if(tileinfo->minimap_map) misc_string_free(tileinfo->minimap_map);

  memset(tileinfo, 0, sizeof(TileInfo));

  while(entry != NULL)
  {
    if(codedchar == entry->tile)
    {
#ifdef DEBUG_VV
      fprintf(stderr, "DEBUG: tiles_decode: found one ('%c')\n", codedchar);
      fprintf(stderr, "DEBUG: | %s: %dx%d\n", entry->tileinfo.filename,
                      entry->tileinfo.sizex, entry->tileinfo.sizey);
#endif

      tileinfo->sizex = entry->tileinfo.sizex;
      tileinfo->sizey = entry->tileinfo.sizey;
      tileinfo->filename = misc_string_copym(entry->tileinfo.filename);
      tileinfo->minimap_map = misc_string_copym(entry->tileinfo.minimap_map);
      tileinfo->num_waypoints = entry->tileinfo.num_waypoints;
      tileinfo->waypoints = entry->tileinfo.waypoints;
      return EXIT_SUCCESS;
    }
    entry = (TileListEntry*)entry->next;
  }
  return EXIT_FAILURE;
}

