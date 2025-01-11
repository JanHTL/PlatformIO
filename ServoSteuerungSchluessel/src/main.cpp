#include <Arduino.h>
#include <Servo.h>

Servo myServo;  // Servo-Objekt erstellen

int currentAngle = 90; // Ausgangsstellung in der Mitte (90 Grad)
int angleChange = 0; // Variabler Winkel

void setup() {
  Serial.begin(9600); // Serielle Kommunikation starten
  myServo.attach(9);  // Servo an Pin 9 anschließen
  myServo.write(currentAngle);  // Servo auf Ausgangsstellung setzen
  Serial.println("Gebe einen Winkel von -10 bis 10 ein: ");
}

void loop() {
  if (Serial.available() > 0) {  // Prüfen, ob Daten über den seriellen Monitor eingegeben wurden
    angleChange = Serial.parseInt();  // Eingabewert als Ganzzahl lesen

    if (angleChange >= -10 && angleChange <= 10) {
      currentAngle = 90 + angleChange;  // Ausgangsstellung (90 Grad) um den Eingabewinkel ändern
      myServo.write(currentAngle);  // Servo auf neuen Winkel bewegen
      Serial.print("Neuer Winkel: ");
      Serial.println(currentAngle);
    } else {
      Serial.println("Bitte einen Wert zwischen -10 und 10 eingeben.");
    }
  }
}
