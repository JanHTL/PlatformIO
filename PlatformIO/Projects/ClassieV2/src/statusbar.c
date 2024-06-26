/*
 * File:            statusbar.c
 * Author:          Thomas Jerman
 * Date Created:    07.04.2024
 *
 * Description:
 * Providing a function to set up a status bar
 */

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "cli.h"
#include "cmd.h"
#include "timer2.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

/****************************************************/
// LOCAL STATIC STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

/****************************************************/
// LOCAL MACROS
/****************************************************/

/****************************************************/
// LOCAL FUNCTIONS
/****************************************************/

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

// Status bar setup
void statusBar()
{
    printf_P(PSTR(TXT_COLOR_REVERSE));
    // Setup any number of status bar lines to be stored and printed from program memory
    // ***********************************************************************************
    printf_P(PSTR("Session time: %02d:%02d:%02d                                                         \n"),
        (int)(timer2GetSeconds() / 3600), (int)((timer2GetSeconds() / 60) % 60), (int)(timer2GetSeconds() % 60));
    printf_P(PSTR("TASK0: %2lu %% | TASK1: %2lu %% | TASK2: %2lu %% | TASK3: %2lu %% of 125 us task time used \n"),
        (timer2GetTicTocTime(0) / 1250) + 1, (timer2GetTicTocTime(1) / 1250) + 1, (timer2GetTicTocTime(2) / 1250) + 1,(timer2GetTicTocTime(3) / 1250) + 1);
    printf_P(PSTR("TASK4: %2lu %% | TASK5: %2lu %% | TASK6: %2lu %% | TASK7: %2lu %% of 125 us task time used \n"),
        (timer2GetTicTocTime(4) / 1250) + 1, (timer2GetTicTocTime(5) / 1250) + 1, (timer2GetTicTocTime(6) / 1250) + 1,(timer2GetTicTocTime(7) / 1250) + 1);
    // ***********************************************************************************
    printf_P(PSTR(TXT_RESET_FORMAT));
}