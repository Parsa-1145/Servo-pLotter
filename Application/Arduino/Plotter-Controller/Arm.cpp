#include "Arm.h"

double Arm::calcAngle(vec2 pos){
  vec2 dist = pos - this->worldPos;
  float z = hypot(dist[0], dist[1]);
  float a1 = atan(dist[1] / dist[0]);
  float a2 = acos((square(this->segLengths[0]) - square(this->segLengths[1]) + square(z)) / (2 * this->segLengths[0] * z));
  if (a1 < 0) a1 += PI;
  if (this->reversed) a2 *= -1;

  return degrees(a1 + a2);
}
Arm::Arm(vec2 pos, vec2 length, bool rev){
  this->reversed = rev;
  this->segLengths = length;
  this->worldPos = pos;
}