#include <Arduino.h>

const int trigPin = 3;    // digital output pin for trigger signal
const int echoPin = 2;    // digital input pin for echo signal
const int ledPin = 9;     // digital output pin for LED
const int potPin = A0;    // analog input pin for potentiometer
const int buttonPin = 10; // digital input pin for button
const int maxDistance = 200;     // maximum distance to measure, in centimeters
const int minDistance = 2.5;     // minimum distance to measure, in centimeters
const int minBrightness = 20;    // minimum brightness for LED, on a scale of 0-255

int brightnessMode = 0;   // variable to store brightness mode (0 = distance, 1 = potentiometer)
int buttonState = 0;      // variable to store button state

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);  // read button state
  Serial.println(buttonState);

  // switch brightness mode if button is pressed
  if (buttonState == HIGH) {
    brightnessMode = 1 - brightnessMode;  // toggle brightness mode
    delay(50);  // small delay to debounce button
  }

  // distance mode
  if (brightnessMode == 0) {
    digitalWrite(trigPin, LOW);  // reset trigger pin
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);  // send 10 us trigger pulse
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    int pulseDuration = pulseIn(echoPin, HIGH);  // measure echo pulse duration
    int distance = pulseDuration / 58;  // convert pulse duration to distance in centimeters

    // limit distance to maximum and minimum
    distance = max(distance, minDistance);
    distance = min(distance, maxDistance);

    // turn off LED if distance is outside range, otherwise calculate brightness based on distance
    if (distance < minDistance || distance > maxDistance) {
      digitalWrite(ledPin, LOW);
    } else {
      int brightness = map(distance, minDistance, maxDistance, minBrightness, 255);
      analogWrite(ledPin, brightness);  // set LED brightness
    }

    delay(50);  // small delay to prevent rapid brightness changes
  }

  // potentiometer mode
  if (brightnessMode == 1) {
    int potValue = analogRead(potPin);  // read potentiometer value
    int brightness = map(potValue, 0, 1023, 0, 255);  // map potentiometer value to brightness range
    analogWrite(ledPin, brightness);  // set LED brightness
    delay(50);  // small delay to prevent rapid brightness changes
  }
}
