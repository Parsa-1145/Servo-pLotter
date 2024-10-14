void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}



struct serialState{
  virtual void parse(string str) = 0;
}
struct serialIdleState: public serialState{
  void parse() override;
} idleState;
struct serialConfigState: public serialState{
  int messageNum = 0;
  int state = 0;
  void parse() override;
} configState;
struct serialDrawingState: public serialState{
  void parse() override;
} drawingState;

char inChar;
string inStr;
serialState* curSerialState = &idleState;

serialIdleState::parse(string str){
  if(str.equals("c")){
    curSerialState = &configState;
    configState.messageNum = 0;
    configState.state = 0;
  }
}

serialConfigState::parse(string str){
  switch this->state:
    case 0:
      if(str.equals("end")){
        curSerialState = &idleState;
        this->messageNum = 0;
        this->state = 0;
        break;
      }
      this->messageNum = str.toInt();
      this->state = 1;
      break;
    case 1:
      
}

void serialEvent() {
  while (Serial.available()) {
    inChar = (char)Serial.read();
    if(inChar == '\n'){
      curSerialState->parse(inStr);
      inStr = "";
    }else{
      inStr += inChar;
    }
  }
}