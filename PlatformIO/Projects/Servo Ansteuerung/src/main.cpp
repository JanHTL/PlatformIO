#include <Arduino.h>
#define DELAY 250

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setup() {
  myservo.attach(11);
  myservo.write(90); // attaches the servo on pin 9 to the servo object
}

void loop() {
  myservo.write(90);                  // sets the servo position according to the scaled value
  delay(DELAY);    
  myservo.write(180);                  // sets the servo position according to the scaled value
  delay(DELAY);  
  myservo.write(90);                  // sets the servo position according to the scaled value
  delay(DELAY); 
  myservo.write(0);                  // sets the servo position according to the scaled value
  delay(DELAY);                          // waits for the servo to get there
}