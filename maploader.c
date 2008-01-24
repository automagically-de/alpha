#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "SDL.h"

#include "conf.h"
#include "vfs.h"
#include "maploader.h"
#include "message.h"

#define BUFSIZE 4096

#define MAPSIZE      (1 << 0)
#define MAPDATA      (1 << 1)
#define MAPHEIGHT    (1 << 2)
#define MAPOTHER     (1 << 7)

int _maploader_decode(Uint8 coded, Uint32 mapx, Uint32 mapy);
void _maploader_createbuffer(Uint32 newwidth, Uint32 newheight);
char *_maploader_getvalue(char *buffer, int maxlen, const char *line, int ln);

/*****************************************************************************/

int maploader_load(const char *filename, Uint8 fatal)
{
  vfs_FILE *mapfile;
  char path[1024];
  char buffer[BUFSIZE];
  int len, varlen, line = 0;
  char *bufp, *varp, *err;
  char varname[1024];
  char value[1024];
  Uint8 datasection = MAPSIZE;
  Uint8 uval8;
  Uint32 mapx = 0, mapy = 0;
  Uint32 newwidth = 0, newheight = 0;

#ifdef HAVE_SNPRINTF
  snprintf(path, 1024, DATADIR "/maps/%s", filename);
#else
  sprintf(path, DATADIR "/maps/%s", filename);
#endif

  /** open file */
  mapfile = vfs_fopen(path, "r");
  if(mapfile == NULL)
  {
    if(fatal)
    {
      /** it was the last chance, exit... */
      fprintf(stderr, "ERROR: couldn't open '%s'\n", path);
      exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    fprintf(stderr, "DEBUG: couldn't open '%s'\n", path);
#endif
    return EXIT_FAILURE;
  }

#ifdef DEBUG
  fprintf(stderr, "DEBUG: maploader_load: loading '%s'\n", path);
#endif

  /** load map */
  while(vfs_fgets(buffer, BUFSIZE, mapfile))
  {
    /** strip newline & increase line counter */
    len = strlen(buffer);
    if(buffer[len-1] == '\n') 
    {
      buffer[len-1] = '\0';
      line++;
    }
    bufp = buffer;
    /** remove leading spaces */
    while(*bufp && isspace(*bufp)) bufp++;
    /** ignore comments */
    if(*bufp && (*bufp == '#')) continue;
    /** remove empty lines */
    if(!*bufp) continue;
    if((datasection == MAPSIZE) || (datasection == MAPOTHER))
    {
      /** get variable or section name */
      varlen = 0;
      varp = bufp;
      while(*bufp && (isalnum(*bufp) || (*bufp == '<') || (*bufp == '>'))) 
      { 
        bufp++; 
        varlen++; 
      }
      /** copy variable/section name */
      strncpy(varname, varp, (varlen > 1024) ? 1024 : varlen);
      varname[(varlen > 1023) ? 1023 : varlen] = '\0';
      if(strcmp(varname, "<MAPDATA>") == 0)
      {
        datasection = MAPDATA;
#ifdef DEBUG
        fprintf(stderr, "DEBUG: maploader_load: starting MAPDATA section in "
                        "line %d\n", line);
#endif
        _maploader_createbuffer(newwidth, newheight);
        continue;
      }
      if(strcmp(varname, "<HEIGHTDATA>") == 0)
      {
        datasection = MAPHEIGHT;
        mapy = mapx = 0;
        continue;
      }
      /** handle variables in main section */
      if(varlen == 0)
      {
        fprintf(stderr, "ERROR: unrecognized data in '%s', line %d: %s\n",
                filename, line, varp);
        continue;
      }
#ifdef DEBUG
      fprintf(stderr, "DEBUG: maploader_load: variable '%s' found in line "
                      "%d\n", varname, line);
#endif
      err = NULL;
      if((strcmp(varname, "width") == 0) && (datasection == MAPSIZE))
      {
        _maploader_getvalue(value, 1024, bufp, line);
        newwidth = strtoul(value, &err, 0);
      }
      else if((strcmp(varname, "height") == 0) && (datasection == MAPSIZE))
      {
        _maploader_getvalue(value, 1024, bufp, line);
        newheight = strtoul(value, &err, 0);
      }
      else
      {
        fprintf(stderr, "ERROR: unknown variable in '%s', line %d: '%s'\n",
                filename, line, varname);
      }
      if(!err || (strlen(err) > 0))
      {
        fprintf(stderr, "ERROR: '%s', line %d: wrong number value\n",
                filename, line);
      }
    }
    else if(datasection == MAPDATA)
    {
#ifdef DEBUG_VV
      fprintf(stderr, "DEBUG: map (data): %s\n", bufp);
#endif
      if((newwidth == 0) || (newheight == 0))
      {
        fprintf(stderr, "ERROR: map '%s': entering data section, but width "
                        "or height not specified\n", filename);
        if(fatal) exit(EXIT_FAILURE);
        return EXIT_FAILURE;
      }
      if(strncmp(bufp, "<EOD>", 5) == 0)
      {
        datasection = MAPOTHER;
#ifdef DEBUG
        fprintf(stderr, "DEBUG: maploader_load: end of data section in line "
                        "%d\n", line);
#endif
        continue;
      }

      /** handle map data */
      if(*bufp && (*bufp == '\"'))
      {
        /** quoted line (recommended) */
        bufp++;
        while(*bufp && (*bufp != '\"'))
        {
          if(mapx >= newwidth) break;
          if(mapy >= newheight) break;
          _maploader_decode((Uint8)*bufp, mapx, mapy);
          mapx++;
          bufp++;
        }
        if(!*bufp || (*bufp != '\"'))
        {
          fprintf(stderr, "ERROR: '%s', line %d: unterminated string in "
                          "<MAPDATA> section\n", filename, line);
        }

        mapy++;
        mapx = 0;
      }
    }
    else if(datasection == MAPHEIGHT)
    {
      if(strncmp(bufp, "<EOD>", 5) == 0)
      {
        datasection = MAPOTHER;
        continue;
      }

      if(*bufp && (*bufp == '\"'))
      {
        bufp++;
        while(*bufp && (*bufp != '\"'))
        {
          if(mapx >= newwidth+1) break;
          if(mapy >= newheight+1) break;
          value[0] = *bufp;
          value[1] = '\0';
          /* '0'..'9', 'A'..'Z' => 0..34 */
          uval8 = (Uint8)strtoul(value, &err, 35);
          if(err && strlen(err))
          {
            fprintf(stderr, "ERROR: maploader_load: error in map file '%s', "
                            "line %d: invalid height spec\n", filename, line);
          }
          else config.map.heights[mapy * (newwidth+1) + mapx] = uval8;
          bufp++;
          mapx++;
        }
        if(!*bufp || (*bufp != '\"'))
        {
          fprintf(stderr, "ERROR: '%s', line %d: unterminated string in "
                          "<HEIGHTDATA> section\n", filename, line);
        }
        mapy++;
        mapx = 0;
      }
    }
  }

  /** close file */
  vfs_fclose(mapfile);

  message_add("map successfully loaded.", 0);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _maploader_decode(Uint8 coded, Uint32 mapx, Uint32 mapy)
{
  Uint32 pos = mapy * config.map.width + mapx;
  MapTile *maptile = config.map.buffer[pos];

  if(maptile->level1 == NULL)
  {
    maptile->level1 = (MapTileData*)malloc(sizeof(MapTileData));
    if(maptile->level1 == NULL) return EXIT_FAILURE;
    config.memory.used += sizeof(MapTileData);
  }
  maptile->level1->tile = coded;
  maptile->level1->offx = mapx;
  maptile->level1->offy = mapy;
  maptile->level1->size = 1 + (1 << 4);

  switch(coded)
  {
    case ',':
      maptile->level1->size = 3 + (3 << 4);
      break;
    default:
      break;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

void _maploader_createbuffer(Uint32 newwidth, Uint32 newheight)
{
  Uint32 i, hfsize;
  MapTile *maptile;

  /** to be sure... */
  if((newwidth == 0) || (newheight == 0)) return;

  if(config.map.buffer != NULL)
  {
    /** free old buffer */
    for(i=0; i<(config.map.width * config.map.height); i++)
    {
      if(config.map.buffer[i] != NULL)
      {
        config.memory.used -= sizeof(MapTile);
        free(config.map.buffer[i]);
      }
    }
    config.memory.used -= (config.map.width*config.map.height*sizeof(void*));
    free(config.map.buffer);
  }
  /** create new buffer */
  config.map.width = newwidth;
  config.map.height = newheight;

  config.map.buffer = malloc(newwidth * newheight * sizeof(void*));
  if(config.map.buffer == NULL)
  {
    fprintf(stderr, "ERROR: couldn't allocate memory for map tile buffer\n");
    exit(EXIT_FAILURE);
  }
  config.memory.used += newwidth * newheight * sizeof(void*);
  for(i=0; i<(newwidth * newheight); i++)
  {
    config.map.buffer[i] = malloc(sizeof(MapTile));
    config.memory.used += sizeof(MapTile);
    maptile = (MapTile*)config.map.buffer[i];
    maptile->level1 = NULL;
  }

  /* create height buffer */
  if(config.map.heights != NULL) 
  {
    hfsize = (config.map.width + 1) * (config.map.height + 1) * sizeof(Uint8);
    config.memory.used -= hfsize;
    free(config.map.heights);
  }
  hfsize = (newwidth + 1) * (newheight + 1) * sizeof(Uint8);
  config.map.heights = malloc(hfsize);
  if(config.map.heights == NULL)
  {
    fprintf(stderr, "ERROR: _maploader_createbuffer: couldn't allocate "
                    "memory for height buffer\n");
    exit(EXIT_FAILURE);
  }
  config.memory.used += hfsize;
  memset(config.map.heights, 0, hfsize);
}

/*****************************************************************************/

char *_maploader_getvalue(char *buffer, int maxlen, const char *line, int ln)
{
  char *lptr = (char*)line;
  int vallen = 0;

  if(!buffer) return NULL;

  buffer[0] = '\0';

  while(*lptr && isspace(*lptr)) lptr++;
  if(!*lptr || (*lptr != '=')) return NULL; else lptr++;
  while(*lptr && isspace(*lptr)) lptr++;
  if(!*lptr) return NULL;

  if(*lptr == '\"')
  {
    /** quoted string */
    lptr++;
    while(*lptr && (*lptr != '\"'))
    {
      buffer[vallen] = *lptr;
      lptr++;
      vallen++;
      if(vallen >= maxlen) return buffer;
    }
    if(!*lptr || (*lptr != '\"'))
    {
      fprintf(stderr, "ERROR: %d: unterminated quoted string value\n", ln);
    }
  }
  else
  {
    /** unquoted string */
    while(*lptr && (*lptr != '#'))
    {
      buffer[vallen] = *lptr;
      lptr++;
      vallen++;
      if(vallen >= maxlen) return buffer;
    }
    buffer[vallen] = '\0';
    while((vallen > 0) && isspace(buffer[vallen-1]))
    {
      vallen--;
      buffer[vallen] = '\0';
    }
  }
  return buffer;
}

