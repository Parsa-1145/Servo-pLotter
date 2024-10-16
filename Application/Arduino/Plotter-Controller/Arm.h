#include "vec2.h"
#include "Arduino.h"

#ifndef Arm_H
#define Arm_H
struct Arm{
  vec2 worldPos = vec2(0, 0);
  vec2 segLengths = vec2(0, 0);
  bool reversed = false;
  Arm(vec2 pos, vec2 length, bool rev);

  double calcAngle(vec2 pos);
};
#endif