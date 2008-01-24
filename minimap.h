#ifndef _MINIMAP_H
#define _MINIMAP_H

#define MINIMAP_GROUND       (1L << 0)
#define MINIMAP_TRACKS       (1L << 1)

void minimap_init(void);
int minimap_checkclick(Uint32 mx, Uint32 my, Uint32 *posx, Uint32 *posy);
void minimap_draw(Uint32 posx, Uint32 posy);
int minimap_puttile(Uint32 mapx, Uint32 mapy, Uint8 r, Uint8 g, Uint8 b,
                    Uint32 flags);
int minimap_puttilemap(Uint32 mapx, Uint32 mapy, const char *map, 
                       Uint8 r, Uint8 g, Uint8 b, Uint32 flags);

#endif
