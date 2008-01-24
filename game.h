#ifndef _GAME_H
#define _GAME_H

int game_new(const char *mapname);

#define FLAG_QUIT              (1L << 0)
#define FLAG_MMDRAG            (1L << 1)
#define FLAG_DRAGMOVE          (1L << 2)
#define FLAG_MAPTOSMALL        (1L << 3)
#define FLAG_WARNED            (1L << 4)

#endif
