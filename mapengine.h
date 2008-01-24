#ifndef _MAPENGINE_H
#define _MAPENGINE_H

int mapengine_init(const char *mapname);
int mapengine_scale(float scale);
int mapengine_draw(Uint32 sx, Uint32 sy, Uint32 width, Uint32 height);

#endif
