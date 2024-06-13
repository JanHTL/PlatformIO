#include <Arduino.h>
#define BLINKLED_BUILTIN 13

void setup() {
  pinMode(BLINKLED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(BLINKLED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(BLINKLED_BUILTIN, LOW);
  delay(1000);
}