#include "colors.inc"

global_settings { ambient_light <1,1,1> }

background { rgbt<0,0,0,1> }

camera
{
  location <6, 0, 0>
  look_at <0, 0, 0>
  rotate z*30
  rotate y*-45
  orthographic
  translate z*1
}

light_source
{
  <15, 15, 15>
  color White
}

light_source
{
  <0, 10, 0>
  color White
  area_light <10,0,0>, <0,0,10>, 15, 15
  adaptive 1
  jitter
}

