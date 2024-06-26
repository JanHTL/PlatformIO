/*
 * File:            timer1.c
 * Author:          Thomas Jerman
 * Date Created:    DD.MM.YYYY
 *
 * Description:
 * Providing functions to make use of Timer/Counter1
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "template.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

struct Timer1InputCapture
{
    volatile uint32_t previousCapture;
    volatile uint32_t currentCapture;
    volatile uint32_t overflowCounter;
    volatile uint32_t *captureArray;
    volatile uint32_t captureArrayIndex;
    volatile uint32_t captureArraySize;
};

/****************************************************/
// LOCAL STATIC STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

static struct Timer1InputCapture this;

/****************************************************/
// LOCAL MACROS
/****************************************************/

/****************************************************/
// LOCAL FUNCTIONS
/****************************************************/

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

uint8_t timer1StartInputCapture(volatile uint32_t *captureArray, volatile uint32_t captureArraySize)
{
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1 = 0x0000;

    this.currentCapture = 0;
    this.previousCapture = 0;
    this.overflowCounter = 0;
    this.captureArrayIndex = 0;

    if (captureArray != NULL)
        this.captureArray = captureArray;
    else
        return 0;
    
    this.captureArraySize = captureArraySize;

    TIMSK1 = (1 << ICIE1) | (1 << TOIE1);   //Interrupt Enable of Input Capture and Timer1 Overflow
    TCCR1B = (1 << CS11);   // timer clk/8 -> 16 MHz/8 = 2 MHz -> 500 ns
    return 1;
}

volatile uint32_t timer1GetCaptureArrayIndex()
{
    return this.captureArrayIndex;
}

volatile uint32_t timer1GetOverflowCounter()
{
    return this.overflowCounter;
}

ISR(TIMER1_OVF_vect)
{
    this.overflowCounter++;
}

ISR(TIMER1_CAPT_vect)
{
    this.currentCapture = ICR1 + (this.overflowCounter << 16);  //this.overflowCounter * 65536 = this.overflowCounter << 16

    TCCR1B ^= (1 << ICES1);     // Toggle the edge trigger after ICR1 Register has been read
    TIFR1 |= (1 << ICF1);       // Clear ICF1 (by writing a logical one to it) after toggling of the edge trigger

    if (this.captureArrayIndex < this.captureArraySize && this.previousCapture)
        this.captureArray[this.captureArrayIndex++] = (this.currentCapture - this.previousCapture)/2;

    this.previousCapture = this.currentCapture;
}