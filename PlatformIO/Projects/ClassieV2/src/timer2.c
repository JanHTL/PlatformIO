/*
 * File:            timer2.c
 * Author:          Thomas Jerman
 * Date Created:    07.04.2024
 *
 * Description:
 * Providing timer functions using T/CNT2
 */

#include <stdio.h>
#include <string.h>

#include <avr/io.h>

#include "timer2.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

struct Timer2
{
    unsigned int period;
    unsigned long seconds;
    unsigned int tic[MAX_TIC_TOCS_AVAILABLE];
    unsigned int toc[MAX_TIC_TOCS_AVAILABLE];
};

/****************************************************/
// LOCAL STATIC STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

static struct Timer2 this;

/****************************************************/
// LOCAL MACROS
/****************************************************/

/****************************************************/
// LOCAL FUNCTIONS
/****************************************************/

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

// Timer2 init for Clear Timer on Compare Match (CTC) Mode
int timer2CTCInit()
{
    // enable Output Compare A Match Interrupt
	TIMSK2 |= (1 << OCIE2A);
    // set CTC-MODE: WGM02:0 = 2
	TCCR2A |= (1 << WGM21);
    // set OCR0A to 200. 200 * 0.5 us = 100 us
    //OCR2A = 199;  //for 115200 Baud
    OCR2A = 249;    //for 76800  Baud
    // set Clock Select Bits to clk/8 for 0.5 us counter clock
    TCCR2B |= (1 << CS21);
    // initialize .tic and .toc array
    for (uint8_t index = 0; index < MAX_TIC_TOCS_AVAILABLE; index++)
    {
        this.tic[index] = 0;
        this.toc[index] = 0;
    }
    // set timer2 period in nano seconds
    this.period = 500;
    // initialize timerSeconds
    this.seconds = 0;
    return 1;
}

// Save TCNT2 as start time
void timer2Tic(uint8_t number)
{
    if(number >= 0 && number < MAX_TIC_TOCS_AVAILABLE)
        this.tic[number] = TCNT2;
}

// Save TCNT2 as stop time
void timer2Toc(uint8_t number)
{
    if(number >= 0 && number < MAX_TIC_TOCS_AVAILABLE)
        this.toc[number] = TCNT2;
}

// Get tic toc durations for available tasks
// In case of an invalid task: return value = 0
unsigned long timer2GetTicTocTime(uint8_t task)
{
    if (task >= 0 && task < MAX_TIC_TOCS_AVAILABLE)
        return (this.toc[task] - this.tic[task]) * ((unsigned long) this.period);
    return 0;
}

// Return timer2 seconds
unsigned long timer2GetSeconds()
{
    return this.seconds;
}

// Increase timer2 seconds
void timer2IncrementSeconds()
{
    this.seconds++;
}