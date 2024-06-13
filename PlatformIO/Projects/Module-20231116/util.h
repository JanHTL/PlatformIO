#include <arduino.h>

#ifndef __UTIL_H__
#define __UTIL_H__

#define KEY0 41
#define KEY1 40
#define KEY2 39
#define KEY3 38
#define KEY4 6
#define KEY5 7
#define KEY6 8
#define KEY7 9

#define LED0 49
#define LED1 48
#define LED2 47
#define LED3 46
#define LED4 45
#define LED5 44
#define LED6 43
#define LED7 42

extern uint8_t keys[];
extern uint8_t leds[];

bool kbhit(void);
uint8_t getch(bool wait);
void setLed(uint8_t led, uint8_t on);


#endif
