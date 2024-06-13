#include <Arduino.h>
#include "ampelSer.h"

void updateAmpelSer(uint8_t ampelNr, uint8_t val)
{
    switch(ampelNr)
    {
        case NORD:
            for(int i=0; i<8; i++,val>>=1)
            {
                val%2 ? setBit(NPORT,NDATA) : clrBit(NPORT,NDATA);
                setBit(NPORT, NCLK);
                clrBit(NPORT, NCLK);
            }
            setBit(NPORT, NSTB);
            clrBit(NPORT, NSTB);
            break;

        case OST:
            for(int i=0; i<8; i++,val>>=1)
            {
                val%2 ? setBit(OPORT,ODATA) : clrBit(OPORT,ODATA);
                setBit(OPORT, OCLK);
                clrBit(OPORT, OCLK);
            }
            setBit(OPORT, OSTB);
            clrBit(OPORT, OSTB);
            break;

        case SUED:
            for(int i=0; i<8; i++,val>>=1)
            {
                val%2 ? setBit(SPORT,SDATA) : clrBit(SPORT,SDATA);
                setBit(SPORT, SCLK);
                clrBit(SPORT, SCLK);
            }
            setBit(SPORT, SSTB);
            clrBit(SPORT, SSTB);
            break;

        case WEST:
            for(int i=0; i<8; i++,val>>=1)
            {
                val%2 ? setBit(WPORT,WDATA) : clrBit(WPORT,WDATA);
                setBit(WPORT, WCLK);
                clrBit(WPORT, WCLK);
            }
            setBit(WPORT, WSTB);
            clrBit(WPORT, WSTB);
            break;

        default:
            break;
    }
}


uint8_t getAmpelSer(void)
{
    if(!isBit(NPIN,NBUTTON))
        return NORD;

    if(!isBit(OPIN,OBUTTON))
        return OST;

    if(!isBit(SPIN,SBUTTON))
        return SUED;

    if(!isBit(WPIN,WBUTTON))
        return WEST;

    return NONE;
}


void initAmpelSer(void)
{
    NDDR |= (1<<NDATA) | (1<<NCLK) | (1<<NSTB);
    NDDR &= ~(1<<NBUTTON);
    NPORT|= (1<<NDATA) | (1<<NBUTTON);  // Data=1; Pullup ein
    ODDR |= (1<<ODATA) | (1<<OCLK) | (1<<OSTB);
    OPORT|= (1<<ODATA) | (1<<OBUTTON);  // Data=1; Pullup ein
    SDDR |= (1<<SDATA) | (1<<SCLK) | (1<<SSTB);
    SPORT|= (1<<SDATA) | (1<<SBUTTON);  // Data=1; Pullup ein
    WDDR |= (1<<WDATA) | (1<<WCLK) | (1<<WSTB);
    WPORT|= (1<<WDATA) | (1<<WBUTTON);  // Data=1; Pullup ein

    DDRL =  0xFF;
    PORTL = 0x00;
}
