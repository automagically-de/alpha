#include "macros.inc"
#include "settings.inc"

union
{
  Tile(0, 0)
  Tile(0, 1)
  Tile(0, 2)
  Tile(0, 3)
  Tile(1, 0)
  Tile(1, 1)
  Tile(1, 2)
  Tile(1, 3)
  Tile(2, 0)
  Tile(2, 1)
  Tile(2, 2)
  Tile(2, 3)
  Tile(3, 0)
  Tile(3, 1)
  Tile(3, 2)
  Tile(3, 3)
  Rail90deg(4, 0)
  Rail90deg(3, 1) 
  //Rail90deg(2, 2)

  translate <-2, 0, -2>
  rotate y*180
  translate <2, 0, 2>
}

