#include "Globals.h"
float propGain = 7;
float derivGain = 300;
float integGain = 0;
float offset = 0;
float velSmooth = 0;
float aidThreshold = 0.025;

void sendData(String label, float value){
  Serial.print("d");
  Serial.print('\t');
  Serial.print(label);
  Serial.print('\t');
  Serial.print(value);
  Serial.print('\t');
}
void logToSerial(String str){
  Serial.print("l");
  Serial.print('\t');
  Serial.print(str);
  Serial.print('\t');
}
void logToSerial(int str){
  Serial.print("l");
  Serial.print('\t');
  Serial.print(str);
  Serial.print('\t');
}