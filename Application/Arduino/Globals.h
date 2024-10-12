#include "Arduino.h"
#ifndef extern
#define GLOBALS
extern float propGain;
extern float derivGain;
extern float integGain;
extern float offset;
extern float velSmooth;
extern float aidThreshold;
extern void sendData(String label, float value);
extern void logToSerial(String str);
extern void logToSerial(int str);
#endif