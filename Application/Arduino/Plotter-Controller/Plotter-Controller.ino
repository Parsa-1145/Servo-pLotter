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
  int command = 0;
  int index = 0;
  int state = 0;
  float inValues[10];
  void parse() override;
  void execute();
} configState;
struct serialDrawingState: public serialState{
  void parse() override;
} drawingState;

char inChar;
String inStr;
serialState* curSerialState = &idleState;


serialIdleState::parse(String str){
  if(str.equals("c")){
    curSerialState = &configState;
    configState.messageNum = 0;
    configState.state = 0;
  }
}
serialConfigState::execute(){
  switch (this->command) {
    case 0:
      
  }
}
serialConfigState::parse(String str){
  switch this->state:
    case 0:
      this->command = str.toInt();
      this->state = 1;
      break;
    case 1:
      if(str.equals("end")){
        curSerialState = &idleState;
        this->execute();
        this->state = 0;
        this->command = 0;
        this->index = 0;
        break;
      }
      this->inValues[this->index] = str.toFloat();
      this->index++;
      break;
    default:
      break;
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