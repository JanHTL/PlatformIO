#include <Arduino.h>

int LED[8] =  {49, 48, 47, 46, 45, 44, 43, 42};
int BUT[8] = {9, 8, 7, 6, 38, 39, 40, 41};

bool kbhit(void)
{
  for (int i = 0; i < 8; i++)
  {
    if (digitalRead(BUT[i]) == LOW)
    return true;
  }
  return false;
}

uint8_t getch(void)
{
  while (!kbhit())
    ;

  for (int i = 0; i < 8; i++)
    if (!digitalRead(BUT[i])) return i;
}

void setLed(uint8_t ledNr, int on)
{
  digitalWrite(LED[ledNr], on ? HIGH : LOW);
}


void setup() 
{
  for(int i=0; i<8; i++)
  {
    pinMode(LED[i], OUTPUT);
    pinMode(BUT[i], INPUT_PULLUP);
  }
}

void loop() 
{
  if(kbhit())
  {
    setLed(BUT[getch()], 1);
  }
  else
  {
    for(int i=0; i<8; i++)
      {
        digitalWrite(LED[i], LOW);
      }
  }
}