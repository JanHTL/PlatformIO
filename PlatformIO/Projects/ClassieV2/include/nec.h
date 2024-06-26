/*
 * File:            nec.h
 * Author:          
 * Date Created:    02.06.2024
 * Version: 1.0:    DD.MM.YYYY
 * Last Modified:   DD.MM.YYYY
 *
 * Description:
 * Providing functions to receive and send NEC-Protocoll signals
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * email
 */

#ifndef NEC_H_INCLUDED
#define NEC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// GLOBAL DEFINES
/****************************************************/

#define NEC_CAPTURE_ARRAY_SIZE  67
#define NEC_DATA_ARRAY_SIZE      4

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

// Start receiving pulses of NEC remote controls
// using timer1StartInputCapture
void necStartReceiving();

// Process received NEC pulses until the number of bytes
// defined in NEC_DATA_ARRAY_SIZE has been reveived
int8_t necProcessRxData();

// Get processed data by index
// nax index: NEC_DATA_ARRAY_SIZE - 1
uint8_t necGetProcessedRxData(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif