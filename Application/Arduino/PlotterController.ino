#include "Globals.h"
#include "ServoMotor.h"
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

class Arm{
public:
  vec2 worldPos = vec2(0, 0);
  vec2 segLengths = vec2(0, 0);
  bool reversed = false;
  Arm(vec2 pos, vec2 length, bool rev);

  double calcAngle(vec2 pos);
};
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

ServoMotor servos[2];
Arm arm1(vec2(2.2265, 0), vec2(8, 8), true);
Arm arm2(vec2(-2.2265, 0), vec2(8, 8), false);

double speed = 0.03;

vec2 startPos = {0, 0};
vec2 curPos = {0, 10};
vec2 targetPos = { 0, 5};
vec2 difVec = {0, 0};
double distance = 0;
double total = 0;
double ratio = 0;
bool test = false;

char inChar = ' ';
String inStr = "";

unsigned long curTime = 0;
unsigned long prevTime = -1;
unsigned long deltaTime = 0;

void setup() {
  Serial.begin(19200);
  servos[1].init(9, A0, 2, 58, 940);
  servos[0].init(10, A1, 3, 130, 980);

  servos[0].setAngle(110);
  servos[1].setAngle(90);
}

void loop() {
  curTime = millis();
  deltaTime = curTime - prevTime;
  delay(5);
  for(ServoMotor& s: servos){
    s.updateInfo(deltaTime);
    s.update(deltaTime);
  }
  if(test){
    difVec = targetPos - curPos;
    distance = difVec.getLength();
    ratio = distance / total;
    if(distance >= 0.01){
      curPos = curPos + difVec * ((speed * 4 * (ratio * (1-ratio) + 0.01)) / distance);
      Serial.print(String(curPos[0]));
      Serial.print('\n');
      Serial.print(String(curPos[1]));
      Serial.print('\n');
      servos[1].setAngle(180-arm2.calcAngle(curPos + vec2(0.352, 1)));
      servos[0].setAngle(180-arm1.calcAngle(curPos + vec2(0.352, 1)));
    }else{
      delay(10);
      Serial.write("canReceive\n");
      test = false;
    }
  }
  prevTime = curTime;
}

typedef void (*serialState) (String str);
void defSerial(String str);
int index = 0;
void moveSerial(String str);

serialState curSerialState = defSerial;

void defSerial(String str){
  if(str.equals("M")){
    curSerialState = moveSerial;
  }
}
void moveSerial(String str){
  
  if(index == 0) targetPos.x = str.toDouble();
  if(index == 1) targetPos.y = str.toDouble();
  index ++;
  if(index == 2){
    index = 0;
    curSerialState = defSerial;
    startPos = curPos;
    total = (targetPos - startPos).getLength();
    test = true;
  }
}
void serialEvent() {
  while (Serial.available()) {
    inChar = (char)Serial.read();
    if(inChar == '\n'){
      curSerialState(inStr);
      inStr = "";
    }else{
      inStr += inChar;
    }
  }
}