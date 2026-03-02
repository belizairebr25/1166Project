int ledPin = 8;
int cardiacPin = 7;
int lowPowerPin = 6;
int state = -1;
int StartTime = 1;
int mode = 1; //mode 1: regular, mode 2: LowPower, mode 3: cardiac, mode 10: both issues

void cycle(){
  if(millis() > StartTime + 1000 / mode){ // 1 second divided by mode. 50% duty cycle
    StartTime = 0;
    state = state * -1;
    if(state == 1){
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  } 
}

int checkCardiac(){
  if(digitalRead(cardiacPin) == HIGH){
    return 0;
  } else {
    return 1;
  }
}

int checkLowPower(){
  if(digitalRead(lowPowerPin) == HIGH){
    return 0;
  } else {
    return 1;
  }
}

void setup() {
  // put your setup code here, to run once:
  int StartTime = millis();
  pinMode(ledPin, OUTPUT);
  pinMode(cardiacPin, INPUT_PULLUP);
  pinMode(lowPowerPin, INPUT_PULLUP);
}

void loop() {
  // put your main ode here, to run repeatedly:
  //interval = output mode; 3 modes: normal, Cardiac Alert, Low Voltage
  cycle();
  if(checkCardiac() == 1){
    mode = 3;
  }else if(checkLowPower() == 1){
    mode = 2;    
  }else if(checkCardiac() == 1 && checkLowPower() == 1){
    mode = 10;
  }
}
