#include "Arduino.h"
#ifndef ServoMotor_H
#define ServoMotor_H
class ServoMotor{
public:
  float analogStart = 0;
  float analogEnd = 0;
  float pos[3] = {0, 0, 0};
  float refPos = 0;
  float vel[3] = {0, 0, 0};

  float sig;
  float integral;
  float dif;
  float unsDif;

  int dirPin;
  int posPin;
  int pwmPin;

  void init(int pwmPin, int posPin, int dirPin, float analogStart, float analogEnd);
  void updateInfo(unsigned long deltaTime);
  void update(unsigned long deltaTime);
  void setAngle(float angle);
  void setAnalog(float analog);
};
#endif