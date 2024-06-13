#ifndef LCD12864_H_
#define LCD12864_H_

void initLcd12864(void);
void blankLcd12864(void);
void charLcd12864(char x, char y, char ch);
void printLcd12864(char x, char y, char* str);


void drawFullScreen(char *p);

void setCGRAM(void);


void sendCodeST7920(uint8_t code);
void sendDataST7920(uint8_t data);


#endif /* LCD12864_H_ */