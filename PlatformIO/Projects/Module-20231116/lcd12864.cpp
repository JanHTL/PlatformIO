/********************************************************************
* Ergaenzung:
* 02.01.2015:   void charLcd12864(char x, char y, char ch)
*               void refreshLcd12864(int zeile)
*               void printLcd12864(char x, char y, char* str)
*               void drawFullScreen(char *p)
********************************************************************/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "lcd12864.h"

void spiSendChar(uint8_t send_char);
void sendCodeST7920(uint8_t code);
void sendDataST7920(uint8_t data);

void refreshLcd12864(int zeile);
uint8_t lcd_buffer[64];

#define LCD_DELAY() _delay_us(75)
//#define LCD_DELAY() _delay_us(200)


static char adrFeld[4]={0x80,0x90,0x88,0x98};

void initLcd12864(void)
{
    DDRB|=0x07;
    PORTB|=0x08;
    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 2*1843.200 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR=0x52;
 //   SPSR=0x01;
	sendCodeST7920(0x30);
    _delay_ms(20);
    sendCodeST7920(0x0C);
    sendCodeST7920(0x01);
    _delay_ms(2);
    memset(lcd_buffer,' ',sizeof(lcd_buffer));
}


void spiSendChar(uint8_t send_char)
{
    SPDR = send_char;
    while (!(SPSR & (1<<SPIF)))
        ;
}


void sendCodeST7920(uint8_t code)
{
    spiSendChar(0xF8);
    spiSendChar(code & 0xF0);
    spiSendChar((code<<4));
    LCD_DELAY();
}


void sendDataST7920(uint8_t data)
{
    spiSendChar(0xFA);
    spiSendChar(data & 0xF0);
    spiSendChar((data<<4));
    LCD_DELAY();
}


void refreshLcd12864(int zeile)
{
    int i;	
	char adr = adrFeld[zeile];    

    sendCodeST7920(adr);

	for(i=0;i<16;i++)
	{
		sendDataST7920(lcd_buffer[16*zeile+i]);
	}    
}

	
void charLcd12864(char x, char y, char ch)
{
	int adr = adrFeld[(int)y] + (x>>1);
	int bufferIndex=16*y + x;
	
	if(x>15 || y>3)
	{
		return;
	}
	
    lcd_buffer[bufferIndex] = ch;
	
	bufferIndex&=0xFFFE;
	
	sendCodeST7920(adr);
	sendDataST7920(lcd_buffer[bufferIndex]);
	sendDataST7920(lcd_buffer[bufferIndex+1]);
}


void printLcd12864(char x, char y, char* str)
{
    int adr = 16*y+x;
    
    while(*str)
    {
        lcd_buffer[adr++] = *str++;
    }

    refreshLcd12864(y);
}


void blankLcd12864(void)
{
//	sendCodeST7920(0x30);
	sendCodeST7920(0x01);
    memset(lcd_buffer,' ',sizeof(lcd_buffer));
    refreshLcd12864(0);    
    refreshLcd12864(1);
    refreshLcd12864(2);
    refreshLcd12864(3);
}


// Grafik

void drawFullScreen(char *p)
{
	int ygroup,x,y,i;
	int temp;
	int tmp;
	
	for(ygroup=0;ygroup<64;ygroup++)
	{                          
		if(ygroup<32)
		{
			x=0x80;
			y=ygroup+0x80;
		}
		else
		{
			x=0x88;
			y=ygroup-32+0x80;
		}
		sendCodeST7920(0x34);        
		sendCodeST7920(y);          
		sendCodeST7920(x);          
		sendCodeST7920(0x30);        
		tmp=ygroup*16;
		for(i=0;i<16;i++)
		{
			temp=p[tmp++];
			sendDataST7920(temp);
		}
	}
	sendCodeST7920(0x34);        
	sendCodeST7920(0x36);        
}




void setCGRAM(void)
{
	sendCodeST7920(0x40);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);

	sendDataST7920(0x01);
	sendDataST7920(0x02);
	sendDataST7920(0x04);
	sendDataST7920(0x08);
	sendDataST7920(0x10);
	sendDataST7920(0x20);
	sendDataST7920(0x40);
	sendDataST7920(0x80);
}

