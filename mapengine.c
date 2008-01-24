#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "calc.h"
#include "imagecache.h"
#include "mapengine.h"
#include "tiles.h"
#include "minimap.h"
#include "text.h"
#include "misc.h"
#include "mem.h"
#include "maploader.h"
#include "route.h"

typedef struct {
  void *itemlist[3];                /** items to draw on surface */
  SDL_Surface *surface;             /** created and destroyed dynamically */
} BufferStruct;

typedef struct {
  SDL_Surface *surface;
  Sint32 originx, originy;
  void *next;
} ItemListEntry;

typedef struct {
  SDL_Surface *surface;
  Uint32 mapx, mapy;
  Uint8 tx, ty;
  Uint8 level;
  Uint32 line;
  void *next;
} QueueEntry;

/*****************************************************************************/

#ifdef DEBUG
static Uint32 ColorLight, ColorDark;
#endif
static BufferStruct *buffers;
static QueueEntry *ClipTop = NULL;
static QueueEntry *ClipEnd = NULL;

int mapengine_addtoglobal(Uint32 offx, Uint32 offy, SDL_Surface *surface,
                          Uint8 level);
int mapengine_addtile(Uint32 mapx, Uint32 mapy, SDL_Surface *surface,
                      Uint8 tx, Uint8 ty, Uint8 level);

int _mapengine_buffers_free(void);
int _mapengine_buffers_create(void);
int _mapengine_buffers_fill(void);

int _mapengine_queue_draw(Uint32 line);
int _mapengine_queue_add(Uint32 mapx, Uint32 mapy, SDL_Surface *surface,
                         Uint8 tx, Uint8 ty, Uint8 level, Uint32 line);
SDL_Surface *_mapengine_getground(Uint32 mapx, Uint32 mapy, float scale);

/*****************************************************************************/

int mapengine_init(const char *mapname)
{
  maploader_load(mapname, 1);
  minimap_init();

  mapengine_scale(0.5);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _mapengine_buffers_free(void)
{
  Uint32 x, y, l, pos;
  ItemListEntry *entry, *clear;

  for(y=0; y<config.buffer.num_y; y++)
  {
    for(x=0; x<config.buffer.num_x; x++)
    {
      pos = y * config.buffer.num_x + x;
      for(l=0; l<3; l++)
      {
        entry = buffers[pos].itemlist[l];
        while(entry != NULL)
        {
          clear = entry;
          entry = (ItemListEntry*)entry->next;
          misc_free(clear, sizeof(ItemListEntry));
        }
      }
      config.memory.used -= mem_ofsurface(buffers[pos].surface);
      SDL_FreeSurface(buffers[pos].surface);
    }
  }

  misc_free(buffers, config.buffer.num_x * config.buffer.num_y * 
                     sizeof(BufferStruct));

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _mapengine_buffers_create(void)
{
  Uint32 width, height, mem;

  config.map.pixels.width  = config.map.width  * (config.map.tile.width / 2) +
                             config.map.height * (config.map.tile.width / 2);
  config.map.pixels.height = config.map.width  * (config.map.tile.height / 2) +
                             config.map.height * (config.map.tile.height / 2);

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: _mapengine_buffers_create: mapsize: %ux%u pixels "
                  "(%ux%u)\n", 
          config.map.pixels.width, config.map.pixels.height, 
          config.map.width, config.map.height);
#endif

  width = config.map.pixels.width;
  height = config.map.pixels.height;

  config.buffer.num_x = (width / config.buffer.width) + 
                        ((width % config.buffer.width)?1:0);
  config.buffer.num_y = (height / config.buffer.height) + 
                        ((height % config.buffer.height)?1:0);

  mem = config.buffer.num_x * config.buffer.num_y * sizeof(BufferStruct);
#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: _mapengine_buffers_create: num_x: %u, num_y: %u\n",
          config.buffer.num_x, config.buffer.num_y);
  fprintf(stderr, "DEBUG: _mapengine_buffers_create: mem: %u bytes\n", mem);
#endif

  buffers = malloc(mem);
  memset(buffers, 0, mem);
  config.memory.used += mem;

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int mapengine_scale(float scale)
{
  SDL_Surface *stdtile = 
    icache_getimage_scaled("default/ground_green_eee.png", scale);

  /* not needed */
  if((buffers != NULL) && (scale == config.map.scale)) return EXIT_SUCCESS;

  if(buffers != NULL) _mapengine_buffers_free();

  config.map.scale = scale;

  if(stdtile != NULL)
  {
    config.map.tile.width  = stdtile->w;
    config.map.tile.height = stdtile->h;
  }

  _mapengine_buffers_create();
  _mapengine_buffers_fill();

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _mapengine_buffers_fill(void)
{
  Uint32 line, x1, x2, x, y, pos;
  SDL_Surface *ground;
  MapTile *maptile;
  TileInfo *tinfo = misc_malloc(sizeof(TileInfo));
  Uint8 add_routes = 0;

  if(config.routing.waypoints == NULL) add_routes = 1;

  for(line = 0; line < (config.map.width + config.map.height - 1); line++)
  {
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: mapengine_init: processing line %u...\n", line);
#endif
    if(line < config.map.height) x1 = 0;
    else x1 = line - config.map.height + 1;
    if(line < config.map.width) x2 = line;
    else x2 = config.map.width - 1;

    _mapengine_queue_draw(line);
    
    for(x = x1; x <= x2; x++)
    {
      y = line - x;

      ground = _mapengine_getground(x, y, config.map.scale);
      if(ground != NULL)
      {
        Uint32 color;

        SDL_SetColorKey(ground, SDL_SRCCOLORKEY | SDL_RLEACCEL, 
                        SDL_MapRGB(ground->format, 0xFF, 0x80, 0xFF));
        mapengine_addtile(x, y, ground, 1, 1, 0);
        /** update minimap */
        color = 0x70 + config.map.heights[y*(config.map.width+1)+x] * 0x08;
        if(color > 0xFF) color = 0xFF;
        minimap_puttile(x, y, 0x00, (Uint8)color, 0x00, MINIMAP_GROUND);
      }

      pos = y * config.map.width + x;
      maptile = (MapTile*)config.map.buffer[pos];

      if(maptile->level1 == NULL) continue;

      if(tiles_decode(maptile->level1->tile, tinfo) == EXIT_SUCCESS)
      {
        Sint32 scx, scy;

        if(add_routes)
        {
          calc_maptiletoscreen(x, y, tinfo->sizex, tinfo->sizey, &scx, &scy);
          scx = (Sint32)((double)scx / config.map.scale);
          scy = (Sint32)((double)scy / config.map.scale);
          route_addwaypoints((Waypoint**)tinfo->waypoints, 
                             tinfo->num_waypoints, scx, scy,
                             config.map.heights[y*(config.map.width+1)+x]);
        }
        if((tinfo->sizex == 1) && (tinfo->sizey == 1))
        {
          mapengine_addtile(x, y, 
            icache_getimage_scaled(tinfo->filename, config.map.scale), 
            1, 1, 1);
          minimap_puttile(x, y, 0x80, 0x80, 0x40, MINIMAP_TRACKS);
        }
        else
        {
          _mapengine_queue_add(x, y, 
            icache_getimage_scaled(tinfo->filename, config.map.scale),
            tinfo->sizex, tinfo->sizey, 1, line + tinfo->sizex - 1);
          minimap_puttilemap(x, y, tinfo->minimap_map, 0x80, 0x80, 0x40, 
                             MINIMAP_TRACKS);
        }
      }
    }
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int mapengine_addtoglobal(Uint32 offx, Uint32 offy, SDL_Surface *surface, 
                          Uint8 level)
{
  Uint32 width, height;
  Uint32 fx, fy, lx, ly;
  Uint32 i, j, pos;
  ItemListEntry *new, *last;

  if(surface == NULL) return EXIT_FAILURE;
  
  width = surface->w;
  height = surface->h;

  /** calculate which buffers are involved */
  fx = offx / config.buffer.width;
  fy = offy / config.buffer.height;
  lx = ((offx + width) / config.buffer.width) -
       (((offx + width) % config.buffer.width) ? 0 : 1);
  ly = ((offy + height) / config.buffer.height) -
       (((offy + height) % config.buffer.height) ? 0 : 1);

  if(lx > (config.buffer.num_x - 1)) lx = config.buffer.num_x - 1;
  if(ly > (config.buffer.num_y - 1)) ly = config.buffer.num_y - 1; 

  /** add to itemlist of each buffer */
  for(j=fy; j<=ly; j++)
  {
    for(i=fx; i<=lx; i++)
    {
      pos = j * config.buffer.num_x + i;
      last = buffers[pos].itemlist[level];
      if(last != NULL) while(last->next != NULL) last = last->next;
      new = malloc(sizeof(ItemListEntry));
      config.memory.used += sizeof(ItemListEntry);
      new->surface = surface;
      new->next = NULL;
      new->originx = offx - i * config.buffer.width;
      new->originy = offy - j * config.buffer.height;
      if(last != NULL) last->next = new;
      else buffers[pos].itemlist[level] = new;
    }
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int mapengine_addtile(Uint32 mapx, Uint32 mapy, SDL_Surface *surface, 
                      Uint8 tx, Uint8 ty, Uint8 level)
{
  Sint32 x = -1, y = -1;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: mapengine_addtile: adding tile (%u,%u)\n",
          mapx, mapy);
#endif

  calc_maptiletoscreen(mapx, mapy, tx, ty, &x, &y);

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: mapengine_addtile: (%u,%u): offrootx: %u\n",
          mapx, mapy, offrootx);
#endif

  mapengine_addtoglobal(x, y, surface, level);
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _mapengine_queue_draw(Uint32 line)
{
  QueueEntry *entry = ClipTop, *prev = NULL, *del;

  while(entry != NULL)
  {
    if(entry->line == line)
    {
      mapengine_addtile(entry->mapx, entry->mapy, entry->surface,
                        entry->tx, entry->ty, entry->level);
      /* delete from queue */
      if(prev == NULL) ClipTop = entry->next;
      else prev->next = entry->next;
      if(entry == ClipEnd) ClipEnd = prev;

      del = entry;
      entry = (QueueEntry*)entry->next;
      free(del);
      config.memory.used -= sizeof(QueueEntry);
    }
    else
    {
      prev = entry;
      entry = (QueueEntry*)entry->next;
    }
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _mapengine_queue_add(Uint32 mapx, Uint32 mapy, SDL_Surface *surface,
                         Uint8 tx, Uint8 ty, Uint8 level, Uint32 line)
{
  QueueEntry *new = malloc(sizeof(QueueEntry));
  if(new == NULL) return EXIT_FAILURE;

  new->mapx = mapx;
  new->mapy = mapy;
  new->surface = surface;
  new->tx = tx;
  new->ty = ty;
  new->level = level;
  new->line = line;
  new->next = NULL;

  config.memory.used += sizeof(QueueEntry);

  if(ClipEnd == NULL)
  {
    ClipTop = new;
    ClipEnd = new;
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _mapengine_queue_add: creating clipping queue\n");
#endif
  }
  else
  {
    ClipEnd->next = new;
    ClipEnd = new;
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _mapengine_queue_add: adding to clipping queue\n");
#endif
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

__inline__ void _mapengine_cleanup(Uint32 fx, Uint32 lx, Uint32 fy, Uint32 ly)
{
  Uint32 i,j, pos;

  for(j=0; j<config.buffer.num_y; j++)
  {
    for(i=0; i<config.buffer.num_x; i++)
    {
      if((j < (fy - config.buffer.keep_invis)) ||
         (j > (ly + config.buffer.keep_invis)) ||
         (i < (fx - config.buffer.keep_invis)) ||
         (i > (lx + config.buffer.keep_invis)))
      {
        pos = j * config.buffer.num_x + i;
        if(buffers[pos].surface != NULL)
        {
#ifdef DEBUG_VV
          fprintf(stderr, "DEBUG: _mapengine_cleanup: freeing buffer (%u,%u)"
                          "\n", i, j);
#endif
          config.memory.used -= mem_ofsurface(buffers[pos].surface);
          SDL_FreeSurface(buffers[pos].surface);
          buffers[pos].surface = NULL;
        }
      }
    }
  }
}

/*****************************************************************************/

__inline__ SDL_Surface *mapengine_getbuffer(Uint32 x, Uint32 y)
{
  SDL_Surface *buf;
  SDL_Rect trect;
  Uint32 pos;
  Uint8 level;
#ifdef DEBUG
  char text[1024];
#endif
  ItemListEntry *entry;

  /** get position in array */
  pos = y * config.buffer.num_x + x;

  /** invalid request */
  if(pos >= config.buffer.num_x * config.buffer.num_y) return NULL;

  /** if buffer cached, return it */
  buf = buffers[pos].surface;
  if(buf!=NULL) return buf;

  /** if not cached, create it */
  buf = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                             config.buffer.width, config.buffer.height,
                             16, 0, 0, 0, 0);

#ifdef DEBUG
  ColorLight = SDL_MapRGB(buf->format, 0xAA, 0xAA, 0xAA);
  ColorDark  = SDL_MapRGB(buf->format, 0x60, 0x60, 0x60);
#endif

  config.memory.used += mem_ofsurface(buf);
#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: mapengine_getbuffer: create buffer (%u,%u)\n", x, y);
#endif
#ifdef DEBUG
  SDL_FillRect(buf, NULL, ((x+y)%2)?ColorLight:ColorDark);
#ifdef HAVE_SNPRINTF
  snprintf(text, 1024, "buffer (%u,%u)", x, y);
#else
  sprintf(text, "buffer (%u,%u)", x, y);
#endif /* HAVE_SNPRINTF */
  text_draw(buf, icache_getimage("font.png"), 
            5, config.buffer.height-14, text);
#else /* DEBUG */
  /* area behind map */
  SDL_FillRect(buf, NULL, SDL_MapRGB(buf->format, 0x00, 0x00, 0x40));
#endif /* DEBUG */

  /** draw all items on buffer */
  for(level=0; level<3; level++)
  {
    entry = buffers[pos].itemlist[level];
    while(entry != NULL)
    {
      trect.x = entry->originx;
      trect.y = entry->originy;
      trect.w = entry->surface->w;
      trect.h = entry->surface->h;
      SDL_BlitSurface(entry->surface, NULL, buf, &trect);
      entry = (ItemListEntry *)entry->next;
    }
  }

  buffers[pos].surface = buf;
  return buf;
}

/*****************************************************************************/

int mapengine_draw(Uint32 sx, Uint32 sy, Uint32 width, Uint32 height)
{
  SDL_Surface *buffer;
  SDL_Rect srect;
  Uint32 firstx, firsty, lastx, lasty;
  Uint32 i, j;
#ifdef DEBUG_VV
  static int debug = 1;
#endif

  firstx = sx / config.buffer.width;
  firsty = sy / config.buffer.height;
  lastx = ((sx + width) / config.buffer.width) - 
          (((sx + width) % config.buffer.width) ? 0 : 1);
  lasty = ((sy + height) / config.buffer.height) - 
          (((sy + height) % config.buffer.height) ? 0 : 1);

#ifdef DEBUG_VV
  if(debug)
  {
    fprintf(stderr, "DEBUG: mapengine_draw: (%ux%u+%u+%u) x: %u->%u, "
                    "y: %u->%u\n", 
            width, height, sx, sy, firstx, lastx, firsty, lasty);
    debug = 0;
  }
#endif
  for(j=firsty; j<=lasty; j++)
  {
    for(i=firstx; i<=lastx; i++)
    {
      buffer = mapengine_getbuffer(i, j);
      if(buffer != NULL) /** happens, if mapsize < screensize */
      {
        srect.x = i * config.buffer.width - sx;
        srect.y = j * config.buffer.height - sy;
        srect.w = buffer->w;
        srect.h = buffer->h;
        SDL_BlitSurface(buffer, NULL, config.window.surface, &srect);
      }
    }
  }

  route_draw();

  /** if max. memory (-1MB) is reached, clean up buffer cache */
  if(config.memory.used > (config.memory.max - (1<<20)))
  {
    _mapengine_cleanup(firstx, lastx, firsty, lasty);
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

SDL_Surface *_mapengine_getground(Uint32 mapx, Uint32 mapy, float scale)
{
  char type[4];
  char filename[1024];
  int i;
  SDL_Surface *surface;

  type[0] = config.map.heights[mapy * (config.map.width + 1) + mapx];
  for(i=1; i<4; i++)
  {
    Uint8 x;
    Uint32 pos;
    pos = (mapy + ((i & 0x1)?1:0)) * (config.map.width + 1) + 
           mapx + ((i & 0x2)?1:0);
    x = config.map.heights[pos];
    if(x == type[0])     type[i] = 'e';
    else if(x < type[0]) type[i] = 'l';
    else                 type[i] = 'h';
  }
#ifdef HAVE_SNPRINTF
  snprintf(filename, 1024, "default/ground_green_%c%c%c.png", 
           type[1], type[2], type[3]);
#else
  sprintf(filename, "default/ground_green_%c%c%c.png", 
          type[1], type[2], type[3]);
#endif
  
  surface = icache_getimage_scaled(filename, scale);
  if(surface == NULL) 
    surface = icache_getimage_scaled("default/ground_green_eee.png", scale);

  return surface;
}

