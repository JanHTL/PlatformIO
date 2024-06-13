#ifndef __AMPELSER_H__
#define __AMPELSER_H__

#define setBit(adr,bit) (adr |= 1<<(bit))
#define clrBit(adr,bit) (adr &= ~(1<<(bit)))
#define invBit(adr,bit) (adr ^= 1<<(bit))
#define isBit(adr,bit)  (adr &  1<<(bit))

#define NDATA   0
#define NCLK    1
#define NSTB    2
#define NBUTTON 3
#define NDDR  DDRC
#define NPORT PORTC
#define NPIN  PINC

#define ODATA   4
#define OCLK    5
#define OSTB    6
#define OBUTTON 7
#define ODDR  DDRC
#define OPORT PORTC
#define OPIN  PINC

#define SDATA   0
#define SCLK    1
#define SSTB    2
#define SBUTTON 3
#define SDDR  DDRA
#define SPORT PORTA
#define SPIN  PINA

#define WDATA   4
#define WCLK    5
#define WSTB    6
#define WBUTTON 7
#define WDDR  DDRA
#define WPORT PORTA
#define WPIN  PINA

enum  {NONE,NORD,OST,SUED,WEST};

void initAmpelSer(void);
void updateAmpelSer(uint8_t ampelNr, uint8_t value);
uint8_t getAmpelSer(void);


#endif
