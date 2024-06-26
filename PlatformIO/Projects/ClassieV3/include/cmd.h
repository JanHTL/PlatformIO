/*
 * File:            cmd.h
 * Author:          Thomas Jerman
 * Date Created:    20.12.2023
 * Version: 1.0:    10.03.2024
 * Last Modified:   06.04.2024
 *
 * Description:
 * Providing a command execution setup
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * jr@htl-kaindorf.at
 */

#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// DEFINES
/****************************************************/

//#define SHOW_TERMINAL_SHORTCUTS

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

// Executes all commands defined
uint8_t cmdExecuteCommand(uint8_t *login_status);

// Shows the default commands
void cmdShowDefaultCommands(uint8_t superuser_flag);

// Shows the application commands
void cmdShowApplicationCommands();

// Sets a flag to store 0/1 information in EEPROM
void cmdSetFlag(uint8_t *variable);

// Reads 8 Bit SFR
uint8_t cmdRead8BitRegister(uint8_t address);

// Writes 8 Bit SFR
void cmdWrite8BitRegister(uint8_t address, uint8_t byte);

// Reads 16 Bit SFR
uint16_t cmdRead16BitRegister(uint8_t address);

// Writes 16 Bit SFR
void cmdWrite16BitRegister(uint8_t address, uint16_t value);

// Updates special function register at "address" from EEPROM
uint8_t cmdUpdateSfrFromEEPROM(uint8_t address);

// Updates all special function registers from EEPROM
void cmdUpdateAllSfrFromEEPROM();

// Switches user from standard to superuser or vice versa
void cmdSwitchUser(uint8_t *login_status, uint8_t echoAllCommandsFlag);

#ifdef __cplusplus
}
#endif

#endif