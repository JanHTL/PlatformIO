/*
 * File:            timer2.h
 * Author:          Thomas Jerman
 * Date Created:    07.04.2024
 * Version: 1.0:    DD.MM.2024
 * Last Modified:   DD.MM.2024
 *
 * Description:
 * Providing timer functions using T/CNT2
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * jr@htl-kaindorf.at
 */

#ifndef TIMER2_INCLUDED
#define TIMER2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// GLOBAL DEFINES
/****************************************************/

#define MAX_TIC_TOCS_AVAILABLE  8

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

// Timer2 init for Clear Timer on Compare Match (CTC) Mode
int timer2CTCInit();

// Save TCNT of timer2 as start time
void timer2Tic(uint8_t number);

// Save TCNT of timer2 as stop time
void timer2Toc(uint8_t number);

// Get tic toc durations for available tasks
// In case of an invalid task: return value = 0
unsigned long timer2GetTicTocTime(uint8_t task);

// Return pointer to seconds count
unsigned long timer2GetSeconds();

// Increase this.timerSeconds
void timer2IncrementSeconds();

#ifdef __cplusplus
}
#endif

#endif