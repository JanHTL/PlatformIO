/*
 * File:            timer.h
 * Author:          
 * Date Created:    06.06.2024
 * Version: 1.0:    DD.MM.YYYY
 * Last Modified:   DD.MM.YYYY
 *
 * Description:
 * Providing functions to make use of Timer/Counter1
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * email
 */
#include <stdio.h>
#ifndef TEMPLATE_H_INCLUDED
#define TEMPLATE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// GLOBAL DEFINES
/****************************************************/

/****************************************************/
// GLOBAL STRUCT DEFINITION
/****************************************************/

/****************************************************/
// GLOBAL STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

/****************************************************/
// GLOBAL MACROS
/****************************************************/

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

//starts timer1 input captrue and stores high and low
uint8_t timer1StartInputCapture(volatile uint32_t *captureArray, volatile uint32_t captureArraySize);

//
volatile uint32_t timer1GetCaptureArrayIndex();

volatile uint32_t timer1GetOverflowCounter();

#ifdef __cplusplus
}
#endif

#endif