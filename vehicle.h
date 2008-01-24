#ifndef _VEHICLE_H
#define _VEHICLE_H

#include "SDL.h"

#include "list.h"

typedef struct {
  char *name;
  List *parts;
} Vehicle;

#endif
