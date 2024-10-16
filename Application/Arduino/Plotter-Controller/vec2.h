#include "Arduino.h"
#ifndef VEC2_H
#define VEC2_H
struct vec2{
  double x;
  double y;
  vec2(double w, double z){
    this->x = w;
    this->y = z;
  }
  double operator[](int i){
    if(i == 0){
      return this->x;
    }else if( i == 1){
      return this->y;
    }
  }
  double getLength(){
    return hypot(this->x, this->y);
  }
  vec2 operator+(vec2 vec){
    return vec2(this->x + vec.x, this->y + vec.y);
  }
  vec2 operator-(vec2 vec){
    return vec2(this->x - vec.x, this->y - vec.y);
  }
  vec2 operator*(double f){
    return vec2(this->x * f, this->y * f);
  }
};
#endif