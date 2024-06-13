#include <Arduino.h>

const int steps[] = {B0001, B0011, B0010, B0110, B0100, B1100, B1000, B1001};
const int num_steps = 8;
int current_step = 0;

void setStep(int step) {
  digitalWrite(8, step & B0001);
  digitalWrite(9, step & B0010);
  digitalWrite(10, step & B0100);
  digitalWrite(11, step & B1000);
}

void setup() {
  for (int i = 8; i <= 11; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  setStep(steps[current_step]);
  current_step = (current_step + 1) % num_steps;
  delay(1);
}