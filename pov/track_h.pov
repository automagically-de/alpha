#include "macros.inc"
#include "settings.inc"

union
{
  Tile(0, 0)
  RailStraight()
  translate <-0.5, 0, -0.5>
  rotate y * 90
  translate <0.5, 0, 0.5>
}

