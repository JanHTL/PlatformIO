#include <Arduino.h>
#include <Servo.h>

#define Echo_EingangsPin      7
#define Trigger_AusgangsPin   6
#define servo_Links_pin       12
#define servo_Rechts_pin      10
#define servo_Kopf_pin        13

int maximumRange = 300;
int minimumRange = 2;
long Abstand;
long Dauer;
  
Servo linkes_R;
Servo rechtes_R;
Servo kopf;

void setup() {
  kopf.attach(servo_Kopf_pin);
  rechtes_R.attach(servo_Rechts_pin);
  linkes_R.attach(servo_Links_pin);
  pinMode(Trigger_AusgangsPin, OUTPUT);
  pinMode(Echo_EingangsPin, INPUT);
  Serial.begin (9600);
  kopf.write(90);
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
  for(j = 1; j < 5; j++)
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
  Serial.println(average_abstand);
  return average_abstand;
}

void turn_left(int DELAYTIME)
{
  linkes_R.write(84);
  rechtes_R.write(84);
  delay(DELAYTIME);
}

void turn_right(int DELAYTIME)
{
  linkes_R.write(96);
  rechtes_R.write(96);
  delay(DELAYTIME);
}

void stop()
{
  linkes_R.write(90);
  rechtes_R.write(90);
}


void loop() {
  static long abstand;
  abstand = get_average_abstand();
  if(abstand > 5)
  {
    linkes_R.write(96);
    rechtes_R.write(84);
  }
  else
  {
    stop();
  }

  turn_left(1000);
  stop();
  delay(1000);
  turn_right(1000);
  stop();

  //if(finale_richtung >= 0 && finale_richtung <= 90) turn_left(1930*(1-finale_richtung/90));
  //else if(finale_richtung > 90 && finale_richtung <=180) turn_right(1930*(finale_richtung/90-1));
}