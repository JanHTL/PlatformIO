/*
 * File:            timer1.c
 * Author:          XYZ
 * Date Created:    DD.MM.YYYY
 *
 * Description:
 * Providing functions to make use of Timer/Counter1
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer1.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

struct Timer1InputCapture {
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
    TCCR1A = 0x00; // Normal mode
    TCCR1B = 0x00;
    TCNT1 = 0x0000; // Clear timer

    this.currentCapture = 0;
    this.previousCapture = 0;
    this.overflowCounter = 0;
    this.captureArrayIndex = 0;

    if (captureArray != NULL)
        this.captureArray = captureArray;
    else
        return 0;

    this.captureArraySize = captureArraySize;

    TIMSK1 = (1 << ICIE1) | (1 << TOIE1); // Enable input capture interrupt and overflow interrupt
    TCCR1B = (1 << CS11); // Start timer with prescaler 8
    // Prescaler 8: 1 tick = 0.5us
    return 1;
}

ISR(TIMER1_OVF_vect)
{
    this.overflowCounter++;
}

ISR(TIMER1_CAPT_vect)
{
    this.currentCapture = ICR1 + (this.overflowCounter << 16); //this.overflowCounter * 65536 = this.overflowCounter << 16

    TCCR1B ^= (1 << ICES1); 
    // Toggle the edge trigger after ICR1 Register has been read
    TIFR1 |= (1 << ICF1);
    // Clear ICF1 (by writing a logical one to it) after toggling of the edge trigger

    if(this.captureArrayIndex < this.captureArraySize && this.previousCapture)
        this.captureArray[this.captureArrayIndex++] = this.currentCapture - this.previousCapture;

    this.previousCapture = this.currentCapture;
}