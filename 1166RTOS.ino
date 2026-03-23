int ledPin = 8;
int cardiacPin = 7;
int lowPowerPin = 6;
int state = -1;
int StartTime = 0;
int mode = 1; //mode 1: regular, mode 2: LowPower, mode 3: cardiac, mode 10: both issues
int tonePin = A0;

/**/
void cycle(){
  if(millis() > StartTime + (2000 / (mode * mode))){
    state = state * -1;
    StartTime = millis();
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
  Serial.begin(9600);
  int StartTime = millis();
  pinMode(ledPin, OUTPUT);
  pinMode(cardiacPin, INPUT_PULLUP);
  pinMode(lowPowerPin, INPUT_PULLUP);
  pinMode(tonePin, OUTPUT);
  Serial.println("initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(20);
  //interval = output mode; 3 modes: normal, Cardiac Alert, Low Voltage, both
  cycle();
  tone(tonePin, (440 * mode));
  if (state == 1){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  if(checkCardiac() == 1 && checkLowPower() == 1){  
    mode = 10;
    Serial.println("Cardiac and Low Power Alert!");
  }else if(checkLowPower() == 1){
    mode = 2;    
    Serial.println("Low Power Alert");
  }else if(checkCardiac() == 1){
    mode = 4;
    Serial.println("Cardiac Alert");
  } else {
    mode = 1;
    StartTime = millis();
    noTone(tonePin);
    digitalWrite(ledPin, LOW);
  }

}
