#include "Globals.h"
#include "ServoMotor.h"
#include "vec2.h"
#include "Arm.h"


ServoMotor servos[2];
Arm arm1(vec2(2.2265, 0), vec2(8, 8), true);
Arm arm2(vec2(-2.2265, 0), vec2(8, 8), false);

float speed = 4;
int delayT = 3;
float threshold = 0.05;

vec2 targetPos(0, 10);
vec2 curPos(0, 14);
vec2 difVec(0, 0);
float difLength = 0;
vec2 speedVec(0, 0);
int programState[10];
bool reached = false;
bool finishedPlotting = true;

unsigned long prevTime = -1;
unsigned long curTime = 0;
float deltaTime = 0;

void setup() {
  Serial.begin(115200);
  servos[1].init(9, A0, 2, 58, 940);
  servos[0].init(10, A1, 3, 130, 980);

  // servos[0].setAngle(90);
  // servos[1].setAngle(90);
}

void loop() {
  curTime = millis();
  deltaTime = curTime - prevTime;
  delay(delayT);

  difVec = targetPos - curPos;
  difLength = difVec.getLength();
  if(!finishedPlotting){
    if(difLength > threshold){
      reached = true;
      for(ServoMotor& s: servos){
        if(!s.reached){
          reached = false;
          break;
        }
      }
      if(true){
        speedVec = difVec * (speed / difLength * deltaTime / 1000);
        if(speedVec.getLength() > difLength){
          curPos = targetPos;
        }else{
          curPos = curPos + speedVec;
        }
      }
      servos[1].setAngle(180-arm2.calcAngle(curPos + vec2(0.352, 1)));
      servos[0].setAngle(180-arm1.calcAngle(curPos + vec2(0.352, 1)));
      Serial.print("m\t");
      Serial.print(curPos.x);      
      Serial.print('\t');
      Serial.print(curPos.y);      
      Serial.print('\t'); 

    }else{
      finishedPlotting = true;
      Serial.print("r\t");
    }
  }
  if(difLength > threshold) finishedPlotting = false;
  
  for(ServoMotor& s: servos){
      s.updateInfo(deltaTime);
      s.update(deltaTime);
  }
  sendData("pos1", servos[0].pos[0]);
  sendData("pos2", servos[1].pos[0]);
  sendData("tar1", servos[0].refPos);
  sendData("tar2", servos[1].refPos);
  sendData("tarx", finishedPlotting);
  Serial.print("\n");
  prevTime = curTime;
}

char inChar;
String inStr;
int serialState[10];
float floatBuffer[10];

void executeConfig(int command){
  switch (command){
    case 0:
      propGain = floatBuffer[0];
      derivGain = floatBuffer[1];
      integGain = floatBuffer[2];
      offset = floatBuffer[3];
      velSmooth = floatBuffer[4];
      break;
    case 1:
      speed = floatBuffer[0];
      delayT = (int)floatBuffer[1];
      break;
    default:
      break;
  }
}

void parseSerial(String str){
  switch (serialState[0]) {
    case 0:
      if(str.equals("c")){
        serialState[1] = 0;
        serialState[0] = 1;
      }else if(str.equals("M")){
        serialState[0] = 2;
        serialState[1] = 0;
      }
      break;
    case 1:
      switch (serialState[1]){
        case 0:
          serialState[2] = str.toInt();
          serialState[1] = 1;
          serialState[3] = 0;
          break;
        case 1:
          if(str.equals("end")){
            serialState[0] = 0;
            executeConfig(serialState[2]);
            break;
          }else{
            floatBuffer[serialState[3]] = str.toFloat();
            serialState[3]++;
          }
          break;
        default:
          break;
      }
      break;
    case 2:
      floatBuffer[serialState[1]] = str.toFloat();
      serialState[1]++;
      if(serialState[1] == 2){
        targetPos.x = floatBuffer[0];
        targetPos.y = floatBuffer[1];
        finishedPlotting = false;
        serialState[0] = 0;
      }
      break;
    default:
      break;
  }
}

void serialEvent() {
  while (Serial.available()) {
    inChar = (char)Serial.read();
    if(inChar == '\n'){
      if(inStr.length() != 0){
        parseSerial(inStr);
      }
      inStr = "";
    }else{
      inStr += inChar;
    }
  }
}