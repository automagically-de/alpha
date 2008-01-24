#include "metals.inc"

#declare TrainWheel = union {
  difference {
    union {
      cylinder { <0, 0, 0>, <0, 0.05, 0>, 0.1 }
      cylinder { <0, 0, 0>, <0, 0.01, 0>, 0.12 } 
    }
    cylinder { <0, -0.1, 0>, <0, 0.1, 0>, 0.09 }
    texture { T_Silver_2C } 
  }
  cylinder { <0, 0, 0>, <0, 0.04, 0>, 0.09
    texture { pigment { color Red } } 
  }
  rotate x * 90
}

/*****************************************************************************/

#declare TrainChassis = union {
  object { TrainWheel translate <-0.2, 0.3, 0.13> }
  object { TrainWheel translate < 0.2, 0.3, 0.13> }
  object { TrainWheel rotate x * 180 translate <-0.2, 0.3, -0.13> }
  object { TrainWheel rotate x * 180 translate < 0.2, 0.3, -0.13> }
  union {
    cylinder { <-0.2, 0.3, -0.2>, <-0.2, 0.3, 0.2>, 0.02 }
    cylinder { < 0.2, 0.3, -0.2>, < 0.2, 0.3, 0.2>, 0.02 }
    box { <-0.40, 0.30, -0.20>, < 0.40, 0.35, -0.19> }
    box { <-0.40, 0.30,  0.20>, < 0.40, 0.35,  0.19> }
    box { <-0.40, 0.30, -0.20>, <-0.39, 0.35,  0.20> }
    box { < 0.40, 0.30, -0.20>, < 0.39, 0.35,  0.20> }
    box { <-0.05, 0.34, -0.20>, < 0.05, 0.35,  0.20> }
    texture { T_Silver_3C }
  }
}

