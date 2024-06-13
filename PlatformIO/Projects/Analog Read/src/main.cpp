#include <Arduino.h>

String inputString = "";
bool stringComplete = false;

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);
  Serial.println("test");
}

void loop() {
  if(stringComplete)
  {
    Serial.print("he");
    if(inputString == "ar\n")
      Serial.println(analogRead(A0)*5.0/1024);
    inputString = "";
    stringComplete = false;
  }
}

void SerialEvent(){
  while(Serial.available()){
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n')
      stringComplete = true;
  }
}