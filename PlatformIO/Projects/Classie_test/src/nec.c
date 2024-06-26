/*
 * File:            nec.c
 * Author:          Thomas Jerman
 * Date Created:    02.06.2024
 *
 * Description:
 * Providing functions to receive and send NEC-Protocoll signals
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "nec.h"
#include "timer1.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

// Define pulse and burst times in microseconds
#define LEADING_LOW_PULSE       9000
#define LEADING_SPACE           4500
#define TOLERANCE               1000
#define LOGIC_ONE_SPACE         1687
#define REPEAT_LOW_PULSE        2250

enum NecStates
{
    NEC_IDLE = 0,
    NEC_RECEIVING_LEADING_PULSE,
    NEC_RECEIVING_LEADING_SPACE,
    NEC_DECODING_DATA,
    NEC_END_OF_TRANSMISSION
};

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

struct Nec
{
    uint8_t state;
    uint8_t dataArrayIndex;
    uint8_t dataArray[NEC_DATA_ARRAY_SIZE];
    uint32_t captureArrayIndex;
    volatile uint32_t captureArray[NEC_CAPTURE_ARRAY_SIZE];
    uint32_t timeStamp;
};

/****************************************************/
// LOCAL STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

static struct Nec this =
{
    .state = NEC_IDLE,
    .dataArrayIndex = 0,
    .dataArray = {0},
    .captureArrayIndex = 0,
    .captureArray = {0},
    .timeStamp = 0
};

/****************************************************/
// LOCAL MACROS
/****************************************************/

/****************************************************/
// LOCAL FUNCTIONS
/****************************************************/

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

// Start receiving pulses of NEC remote controls
// using timer1StartInputCapture
void necStartReceiving()
{
    for (int i = 0; i < NEC_CAPTURE_ARRAY_SIZE; i++)
        this.captureArray[i] = 0;
    this.dataArrayIndex = 0;
    this.captureArrayIndex = 0;
    this.state = NEC_RECEIVING_LEADING_PULSE;
    timer1StartInputCapture(this.captureArray, NEC_CAPTURE_ARRAY_SIZE);
}

// Process received NEC pulses until the number of bytes
// defined in NEC_DATA_ARRAY_SIZE has been reveived
int8_t necProcessRxData()
{
    switch (this.state)
    {
        case NEC_IDLE:
            return 0;
        break;

        case NEC_RECEIVING_LEADING_PULSE:

            if (timer1GetCaptureArrayIndex() > 0)
            {
                if (this.captureArray[0] > (LEADING_LOW_PULSE - TOLERANCE) && this.captureArray[0] < (LEADING_LOW_PULSE + TOLERANCE))
                    this.state = NEC_RECEIVING_LEADING_SPACE;
                else
                    necStartReceiving();
            }
        break;

        case NEC_RECEIVING_LEADING_SPACE:

            if (timer1GetCaptureArrayIndex() > 1)
            {
                if (this.captureArray[1] > LEADING_SPACE - TOLERANCE && this.captureArray[1] < LEADING_SPACE + TOLERANCE)
                {
                    this.state = NEC_DECODING_DATA;
                    this.captureArrayIndex = 3;
                    this.timeStamp = timer1GetOverflowCounter();
                }
                else
                    necStartReceiving();
            }
        break;

        case NEC_DECODING_DATA:

            if (this.captureArray[this.captureArrayIndex] != 0)
            {
                this.dataArray[this.dataArrayIndex] >>= 1;

                if (this.captureArray[this.captureArrayIndex] > LOGIC_ONE_SPACE - TOLERANCE && this.captureArray[this.captureArrayIndex] < LOGIC_ONE_SPACE + TOLERANCE)
                    this.dataArray[this.dataArrayIndex] |= 0x80;

                this.captureArrayIndex += 2;
            }

            if (this.captureArrayIndex == (3 + (this.dataArrayIndex + 1) * 16) && this.dataArrayIndex < NEC_DATA_ARRAY_SIZE)
                this.dataArrayIndex++;

            if (this.dataArrayIndex == NEC_DATA_ARRAY_SIZE)
                this.state = NEC_END_OF_TRANSMISSION;

            if (timer1GetOverflowCounter() > this.timeStamp + NEC_DATA_ARRAY_SIZE)
                necStartReceiving();
        break;

        case NEC_END_OF_TRANSMISSION:
            return 1;
        break;
    }
    return 0;
}

// Returns processed data by index
uint8_t necGetProcessedRxData(uint8_t index)
{
    if(index >= NEC_CAPTURE_ARRAY_SIZE)
        return this.dataArray[index];
    else
        return 0;
}