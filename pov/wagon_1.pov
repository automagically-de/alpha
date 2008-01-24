#include "colors.inc"
#include "glass.inc"

#include "settings.inc"
#include "macros_train.inc"

#if(1)
union
{
  box { <10, 0.01, 0.005>, <-10, 0.0, -0.005> }
  box { <0.005, 0.01, 10>, <-0.005, 0.0, -10> }
  rotate y*45
  pigment { color White }
}
#end

union
{
  difference
  {
    union
    {
      box { <0.35, 0.5, 1.9>, <-0.35, 1.0, -1.9> }
      cylinder { <0, -1.9, 0>, <0, 1.9, 0>, 0.35
        rotate x*90 scale <1, 0.5, 1> translate y*1.0 }
    }
    union
    {
      box { <0.33, 0.52, 1.85>, <-0.33, 1.01, -1.85> }
      cylinder { <0, -1.85, 0>, <0, 1.85, 0>, 0.33
        rotate x*90 scale <1, 0.5, 1> translate y*1.0 }
      texture { pigment { color rgb <1,1,1> } }
    }
    #declare i=0;
    #while(i<4)
      union
      {
        box { <1, 0.65, i*0.4+0.05>, <-1, 0.77, i*0.4+0.35> }
        box { <1, 0.65, -(i*0.4+0.05)>, <-1, 0.77, -(i*0.4+0.35)> }
        box { <1, 0.78, i*0.4+0.05>, <-1, 0.90, i*0.4+0.35> }
        box { <1, 0.78, -(i*0.4+0.05)>, <-1, 0.90, -(i*0.4+0.35)> }
        texture { pigment { color rgb <1,1,1> } }
      }
      #declare i=i+1;
    #end
    box { <0.15, 0.57, 2>, <-0.15, 0.9, -2> }
  }
  
  texture { pigment { color rgb <0.8, 0.2, 0.2> } }
}

union
{
  box { <0.33, 0.5, 1.8>, <0.34, 0.98, -1.8> }
  box { <-0.33, 0.5, 1.8>, <-0.34, 0.98, -1.8> }
  texture { T_Glass3 }
}

object { TrainChassis rotate y * 90 translate z *  1.5 }
object { TrainChassis rotate y * 90 translate z * -1.5 }
