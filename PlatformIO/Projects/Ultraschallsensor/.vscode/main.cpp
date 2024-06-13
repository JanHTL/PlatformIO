#include <Arduino.h>

const int sensorTriggerPin = 3;
const int sensorEchoPin = 2;
const int ledPin = 9;
const int potPin = A0;
const int buttonPin = 10;
const int minDistance = 2.5; // minimum distance in cm
const int maxDistance = 400; // maximum distance in cm

int mode = 0; // 0 for potentiometer, 1 for sensor
int brightness = 0;
unsigned long lastSwitchTime = 0;

void setup() {
  pinMode(sensorTriggerPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  lastSwitchTime = millis();
}

void loop() {
  // check if it's time to switch mode
  if (millis() - lastSwitchTime > 10000) {
    mode = 1 - mode; // switch between 0 and 1
    lastSwitchTime = millis(); // update last switch time
  }
  
  // read the potentiometer if in potentiometer mode
  if (mode == 0) {
    brightness = map(analogRead(potPin), 0, 1023, 0, 255);
  }
  // measure distance if in sensor mode
  else {
    // trigger the sensor to send out a sound wave
    digitalWrite(sensorTriggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(sensorTriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTriggerPin, LOW);
    
    // measure the duration of the echo
    long duration = pulseIn(sensorEchoPin, HIGH);
    
    // calculate the distance from the duration
    float distance = duration * 0.034 / 2.0;
    
    // map the distance value to a brightness value
    if (distance >= minDistance && distance <= maxDistance) {
      brightness = map(distance, minDistance, maxDistance, 0, 255);
    }
    else {
      brightness = 0; // turn off the LED if distance is outside range
    }
  }
  
  // read the button state to toggle mode immediately
  if (digitalRead(buttonPin) == LOW) {
    mode = 1 - mode; // switch between 0 and 1
    lastSwitchTime = millis(); // update last switch time
  }
  
  // set the LED brightness
  analogWrite(ledPin, brightness);
  
  // add a delay to avoid flickering
  delay(10);
}
