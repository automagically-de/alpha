#include "colors.inc"

#include "macros.inc"
#include "settings.inc"


#if(0)
plane
{
  <0,1,0>, -1
  texture { pigment { color White } }
}
#end

#if(1)
#end

#if(0)
union
{
  Tile(0, 0)
  RailStraight()
  translate x * -3
}
#end

union
{
  Tile(0, 0)
  RailStraight()
  rotate y * 90
  translate x * 3
  translate z * 6
}

#if(0)
cylinder
{
  <3, 0, 3>, <3, 3, 3>, 0.1
  texture { TexSteel }
}
#end


