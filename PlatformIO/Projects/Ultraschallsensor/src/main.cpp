#include <Arduino.h>
#include <Servo.h>
#include <drive_functions.hpp>

#define Echo_EingangsPin 7
#define Trigger_AusgangsPin 8

DriveController driveController;
int maximumRange = 300;
int minimumRange = 2;
long Abstand;
long Dauer;

void setup() {
  pinMode(Trigger_AusgangsPin, OUTPUT);
  pinMode(Echo_EingangsPin, INPUT);
  Serial.begin (9600);
}

long read_Abstand()
{
  digitalWrite(Trigger_AusgangsPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger_AusgangsPin, LOW);
  Dauer = pulseIn(Echo_EingangsPin, HIGH);
  Abstand = Dauer/58.2;
  return Abstand;
}

long get_average_abstand()
{
  long average_abstand = 0;
  int j;
  Serial.print("Messungen: [");
  for(j = 1; j < 20; j++)
  {
    do
    {
      Abstand = read_Abstand();
      delay(60);  //damit Messungen nicht direkt hintereinander gemacht werden
    }while (Abstand >= maximumRange || Abstand <= minimumRange);
    Serial.print(Abstand);
    Serial.print("; ");
    average_abstand += Abstand;
  }
  Serial.println("]");
  average_abstand /= j;
  return average_abstand;
}

void loop() {
  double richtung[5];
  long distance[5];
  double finale_richtung;
  driveController.stop();

  for(int i = 0; i<9; i++)
  {
    richtung[i] = 22.5 * i;
    driveController.get_head()->write(richtung[i]);
    delay(250);
    distance[i] = get_average_abstand();
    Serial.print("Richtung  ");
    Serial.print(richtung[i]);
    Serial.print(": ");
    Serial.println(distance[i]);
  }

  for(int k = 0; k<5; k++)
  {
    if(k == 0)  finale_richtung = richtung[k];
    else if(distance[k]>=distance[k-1])  finale_richtung = richtung[k];
  }

  Serial.println("------------------------------");
  Serial.print("Fahrtrichtung-index:    ");
  Serial.println(finale_richtung);
  Serial.println("----------Clear Page----------");

  if(finale_richtung >= 0 && finale_richtung <= 90) driveController.turn_left(1930*(1-finale_richtung/90));
  else if(finale_richtung > 90 && finale_richtung <=180) driveController.turn_right(1930*(finale_richtung/90-1));

  driveController.get_head()->write(90);
  driveController.drive_forward(4000);
}