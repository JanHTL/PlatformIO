#include <Arduino.h>
#include <Servo.h>

#define Drehung 1930
#define Fahren 2500

Servo linkesRad;
Servo rechtesRad;

void setup() {
  linkesRad.attach(10); //10
  rechtesRad.attach(12); //9
}

void stop() {
  linkesRad.write(90);
  rechtesRad.write(90);
  delay(250);
}

void drive() {
  stop();
  rechtesRad.write(100);
  linkesRad.write(80);
  delay(Fahren);
}

void turn() {
  stop();
  rechtesRad.write(93);
  linkesRad.write(93);
  delay(Drehung);
}

void loop() {
  drive();
  turn();
}

