#include "settings.inc"
#include "macros.inc"
#include "macros_train.inc"

#if(0)
union {
  RailStraight()
  translate <-0.5, 0, -0.5>
}
#end

object { 
  TrainChassis 
  rotate y * (clock * 360)
  }

