#include "colors.inc"
#include "woods.inc"
#include "metals.inc"
#include "stones.inc"
#include "glass.inc"

#declare TexTile  = texture { pigment { color rgbt<0, 0.5, 0, 0> } }
#declare TexWood  = texture { T_Wood31 }
#declare TexSteel = texture { T_Silver_3C }

#macro RailStraight()
  #declare xB = 0;
  #declare xD = 1 / 4;
  #while(xB <= 1)
    intersection
    {
      box
      {
        <-0.05, 0.1, 0.2>, <0.05, 0.125, 0.8>
        translate x * xB
      }
      box { <0,0,0>, <1, 1, 1> }
      texture { TexWood }
    }
    #declare xB = xB + xD;
  #end
  union
  {
    cylinder { <0, 0.2, 0.35>, <1, 0.2, 0.35>, 0.02 }
    cylinder { <0, 0.2, 0.65>, <1, 0.2, 0.65>, 0.02 }
    box { <0, 0.125, 0.34>, <1, 0.175, 0.36> }
    box { <0, 0.125, 0.64>, <1, 0.175, 0.66> }
    texture { TexSteel }
  }
#end

#macro Rail90deg(rad, offZ)
  #declare aB = 0;
  #declare aR = 180 / (rad * 10);
  #while(aB <= 90)
    intersection
    {
      box
      {
        <-0.05, 0.1, 0.2>, <0.05, 0.125, 0.8>
        translate z*-(rad)
        rotate y*-aB
        translate z*(rad)
        translate z*offZ
      }
      box { <0, 0, 0>, <rad, 0.2, rad> translate z*offZ }
      texture { TexWood }
    }
    #declare aB = aB + aR;
  #end
  intersection
  {
    torus { (rad-0.35), 0.02 translate <0, 0.2, rad+offZ> }
    box { <0, 0, 0>, <rad, 0.5, rad> translate z*offZ }
    texture { TexSteel }
  }
  intersection
  {
    torus { (rad-0.65), 0.02 translate <0, 0.2, rad+offZ> }
    box { <0, 0, 0>, <rad, 0.5, rad> translate z*offZ }
    texture { TexSteel }
  }
  intersection
  {
    difference
    {
      cylinder { <0, 0.125, rad+offZ>, <0, 0.175, rad+offZ>, (rad-0.34) }
      cylinder { <0, 0.120, rad+offZ>, <0, 0.180, rad+offZ>, (rad-0.36) }
    }
    box { <0, 0, 0>, <rad, 0.5, rad> translate z*offZ }
    texture { TexSteel }
  }
  intersection
  {
    difference
    {
      cylinder { <0, 0.125, rad+offZ>, <0, 0.175, rad+offZ>, (rad-0.64) }
      cylinder { <0, 0.120, rad+offZ>, <0, 0.180, rad+offZ>, (rad-0.66) }
    }
    box { <0, 0, 0>, <rad, 0.5, rad> translate z*offZ }
    texture { TexSteel }
  }
#end

#macro Tile(offX, offZ)
  box
  {
    <0, 0.1, 0>, <1, 0.1, 1>
    translate <offX, 0, offZ>
    //texture { pigment { rgbf<1,1,1,1> } }
    texture { TexTile }
    interior { ior 5.0 }
  }
#end

