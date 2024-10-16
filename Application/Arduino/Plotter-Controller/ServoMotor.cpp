#include "Arduino.h"
#include "ServoMotor.h"
#include "Globals.h"

void ServoMotor::init(int pwmPin, int posPin, int dirPin, float analogStart, float analogEnd){
  this->dirPin = dirPin;
  this->pwmPin = pwmPin;
  this->posPin = posPin;

  pinMode(posPin, INPUT);
  pinMode(dirPin, OUTPUT);

  this->pos[0] = 0;
  this->refPos = 0;
  this->vel[0] = 0;

  this->sig = 0;
  this->integral = 0;
  this->dif = 0;
  
  this->analogEnd = analogEnd;
  this->analogStart = analogStart;
}
void ServoMotor::updateInfo(unsigned long deltaTime){
  this->pos[2] = this->pos[1];
  this->pos[1] = this->pos[0];
  this->pos[0] = analogRead(this->posPin);
  

  this->vel[2] = this->vel[1];
  this->vel[1] = this->vel[0];
  if(deltaTime != 0){
    this->vel[0] = (this->pos[0] - this->pos[1]) / float(deltaTime);
    this->vel[0] = this->vel[1] * (velSmooth) + (1-velSmooth) * this->vel[0];
  }
}
void ServoMotor::update(unsigned long deltaTime){
  this->dif = this->refPos - this->pos[0];
  this->unsDif = abs(this->dif);

  if (5 < this->unsDif && this->unsDif < 20){
    this->integral += deltaTime * this->dif;
  }else{
    this->integral = 0;
  }

  this->sig = (this->dif) * propGain - this->vel[0] * derivGain + this->integral * integGain;

  if(this->sig < 0) digitalWrite(this->dirPin, 1);
  else digitalWrite(this->dirPin, 0);
  analogWrite(this->pwmPin, min(255, int(abs(this->vel[0]) < 0.025) * offset + abs(this->sig)));

  if(dif <= 3) this->reached = true;
  else reached = false;
}
void ServoMotor::setAnalog(float analog){
  this->refPos = analog;
}
void ServoMotor::setAngle(float angle){
  this->refPos = map(angle, 0, 180, this->analogStart, this->analogEnd);
}