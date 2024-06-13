#include "util.h"

uint8_t keys[8]={KEY0,KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7};
uint8_t leds[8]={LED0,LED1,LED2,LED3,LED4,LED5,LED6,LED7};

bool kbhit(void)
{
  for(int i=0;i<8;i++)
    if(!digitalRead(keys[i]))
      return true;

  return false;
}

uint8_t getch(bool wait)
{
  if(wait)  
    while(!kbhit())
      ;

  for(int i=0;i<8;i++)
    if(!digitalRead(keys[i]))
      return i;

  return -1;  
}

void setLed(uint8_t led, uint8_t on)
{
  digitalWrite(leds[led], on);
}
