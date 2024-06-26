/*
 * File:            cmd.c
 * Author:          Thomas Jerman
 * Date Created:    20.12.2023
 *
 * Description:
 * Providing a command execution setup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "sfr328p.h"
#include "cli.h"
#include "cmd.h"
#include "timer1.h"
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

// Executes all commands defined
uint8_t cmdExecuteCommand(uint8_t *login_status)
{
    char *cmd = NULL, *param = NULL;
    char charCtrlKey;

    // add EEPROM variables on top of the already defined ones to get the next
    // valid address. This avoids address rearrangement of all predefined eeprom variables
    // static uint8_t EEMEM nextVariable;       //address = 0x04
    static uint8_t EEMEM statusBarFlag;         //address = 0x03
    static uint8_t EEMEM commandDetailsFlag;    //address = 0x02
    static uint8_t EEMEM commandHistoryFlag;    //address = 0x01
    static uint8_t EEMEM echoAllCommandsFlag;   //address = 0x00

    // Hand over statusBarFlag from EEPROM to cliSetStatusBarFlag
    cliSetStatusBarFlag(eeprom_read_byte(&statusBarFlag));

    // Echo all commands if not in terminal mode in VSC and UART_ISR_CHARACTER_ECHOING is not defined
    if (eeprom_read_byte(&echoAllCommandsFlag) == 1)
        cliPrintRcvdString();   
    
    // Print received command and parameters, returns num_of_params
    if (eeprom_read_byte(&commandDetailsFlag) == 1)
        cliPrintCmdDetails();

    // Check for received CTRL-Keys
    if ((charCtrlKey = cliGetCtrlKey()) != 0)
    {
        switch (charCtrlKey)
        {
            case CTRL_A:    cmdShowApplicationCommands();
                            break;

            case CTRL_D:    cmdShowDefaultCommands(*login_status);
                            break;

            case CTRL_L:    printf_P(PSTR(CLEAR_SCREEN));
                            break;

            case CTRL_U:    cmdSwitchUser(login_status, echoAllCommandsFlag);
                            break;

            case CTRL_Y:    if(*login_status)
                                cliPrintRingBuffer();

            default:        if(!*login_status)
                                printf_P(PSTR("Ctrl+%c not assigned\n"), 'A' + charCtrlKey - 1);
        }
    }

    // cliGetFirstToken() returns the first space separated string received, which is defined to be the command
    // for command comparison the command cstring pointer is stored in cmd
    else if ((cmd = cliGetFirstToken()) == NULL)
    {
        if (eeprom_read_byte(&commandHistoryFlag) == 1)
            cliPrintCmdHistory();
        return 1;
    }
   
    /*******************************************/
    // START OF DEFAULT COMMAND SECTION
    /*******************************************/ 

    // ac = application commands
    else if (strcmp(cmd, "ac") == 0)
    {
        cmdShowApplicationCommands();
    }   

    // cd = command details [-/0/1]
    else if (strcmp(cmd, "cd") == 0)
    {
        printf_P(PSTR("Command details"));
        cmdSetFlag(&commandDetailsFlag);
    }

    // clh = clear History
    else if (strcmp(cmd, "clh") == 0)
    {
        cliClearCmdHistory();
        printf_P(PSTR("Clearing history: done\n"));
    }

    // cls = clear Screen
    else if (strcmp(cmd, "cls") == 0)
    {
        printf_P(PSTR(CLEAR_SCREEN));
    }

    // dc = default commands
    else if (strcmp(cmd, "dc") == 0)
    {
        cmdShowDefaultCommands(*login_status);
    } 

    // ce = command echo [0/1]
    else if (strcmp(cmd, "ce") == 0)
    {
        printf_P(PSTR("Command echo"));
        cmdSetFlag(&echoAllCommandsFlag);
    }

    #ifdef UART_ISR_CHARACTER_ECHOING
    // ch = command history [-/0/1]    
    else if (strcmp(cmd, "ch") == 0)
    {
        printf_P(PSTR("Command history"));
        cmdSetFlag(&commandHistoryFlag);
    }
    #endif

    // sb = status bar
    else if (strcmp(cmd, "sb") == 0)
    {
        printf_P(PSTR("Status bar"));
        cmdSetFlag(&statusBarFlag);
        cliSetStatusBarFlag(eeprom_read_byte(&statusBarFlag));
    }

    // su = switch user
    else if (strcmp(cmd, "su") == 0)
    {
        cmdSwitchUser(login_status, echoAllCommandsFlag);
    }    

    /*******************************************/
    // START OF USER COMMAND SECTION
    /*******************************************/

    else if (strcmp(cmd, "incap") == 0)
    {
        #define SIZE 67
        volatile uint32_t captureArray[SIZE] = {0};
        printf_P(PSTR("Capturing ICP1 (PB0) ...\n"));

        timer1StartInputCapture(captureArray, SIZE);

        for (uint16_t i = 0; i < SIZE; i++)
        {
            while (captureArray[i] == 0);   //timer1.c runs ISR to set captureArray[i]
            if (i % 2 == 0) {
                printf_P(PSTR("%3d: L: %9.1f us"), i, ((float)captureArray[i]));
            } else {
                printf_P(PSTR(", H: %9.1f us\n"), ((float) captureArray[i]));
            }
        }
        printf_P(PSTR("\n"));
    }

    /*******************************************/
    // START OF SUPERUSER COMMAND SECTION
    /*******************************************/

    // cle = clear EEPROM [all/sfr/var]
    else if (strcmp(cmd, "cle") == 0 && *login_status == 1)
    {
        uint16_t address = 0, start_address = 0, end_address = 0;
        if ((param = cliGetNextToken()) != NULL)
        {
            // set address ranges for clearing EEPROM at all addresses
            if (strcmp(param, "all") == 0)
            {
                start_address = 0;
                end_address = EEPROM_ADDRESS_LIMIT;
            }
            // set address range for clearing variables stored in EEPROM:
            else if (strcmp(param, "var") == 0)
            {
                start_address = 0;
                end_address = SFR_IN_EEPROM_OFFSET - 1;
            }            
            // set address ranges for clearing SFR-values stored in EEPROM
            else if (strcmp(param, "sfr") == 0)
            {
                start_address = SFR_IN_EEPROM_OFFSET;
                end_address = EEPROM_ADDRESS_LIMIT;
            }
            else
                printf_P(PSTR("Wrong parameter: %s\n"), param);
            // erasing the specified memory ranges
            for(address = start_address; address <= end_address; address++)
            {
                if (eeprom_read_byte((uint8_t*) (uintptr_t) address) != 0xFF)
                {
                    eeprom_write_byte((uint8_t*) (uintptr_t) address, 0xFF);
                    printf_P(PSTR("EEPROM cleared at address 0x%03X to: 0xFF\n"), address);
                }
            }
            if (end_address != 0)
                printf_P(PSTR("Clearing EEPROM: done\n"), address);
        }
        else
        {
            printf_P(PSTR("Clear EEPROM\n"));
            printf_P(PSTR("Clearing EEPROM at all addresses:      \"cle all\", address range: [0x000, 0x%03X]\n"), EEPROM_ADDRESS_LIMIT);
            printf_P(PSTR("Clearing Variables written to EEPROM:  \"cle var\", address range: [0x000, 0x%03X]\n"), SFR_IN_EEPROM_OFFSET - 1);
            printf_P(PSTR("Clearing SFR-values written to EEPROM: \"cle sfr\", address range: [0x%03X, 0x%03X]\n"), SFR_IN_EEPROM_OFFSET, EEPROM_ADDRESS_LIMIT);
        }
    }

    // eep = eeprom access [ADDR] [-/VAL]
    else if (strcmp(cmd, "eep") == 0 && *login_status == 1)
    {
        uint16_t address = 0;
        uint16_t hexValue = 0;
        char readHexFailed = '\0';
        if ((param = cliGetNextToken()) != NULL)
        {
            if (sscanf(param, "%x%c", &address, &readHexFailed) == 1 && address <= EEPROM_ADDRESS_LIMIT)
                printf_P(PSTR("EEPROM reading at address 0x%03X: 0x%02X\n"), address, eeprom_read_byte((uint8_t*) (uintptr_t) address));
            else if (strcmp(param, "all") == 0)
            {
                for(address = 0; address <= EEPROM_ADDRESS_LIMIT; address++)
                {
                    hexValue = eeprom_read_byte((uint8_t*) (uintptr_t) address);
                    if (hexValue != 0xFF)
                        printf_P(PSTR("EEPROM value at address 0x%03X: 0x%02X\n"), address, hexValue);
                }
                printf_P(PSTR("Reading all written EEPROM values: done\n"), address);
            }
            else
                printf_P(PSTR("EEPROM address not valid: %s\n"), param);

            if ((param = cliGetNextToken()) != NULL && address <= EEPROM_ADDRESS_LIMIT)
            {
                if (sscanf(param, "%x%c", &hexValue, &readHexFailed) == 1 && hexValue <= 0xFF)
                {
                    printf_P(PSTR("EEPROM writing at address 0x%03X: 0x%02X\n"), address, (uint8_t) hexValue);
                    eeprom_write_byte((uint8_t*) (uintptr_t) address, (uint8_t)hexValue);                            
                }
                else
                    printf_P(PSTR("Wrong parameter: %s\n"), param);
            }
        }
        else
        {
            printf_P(PSTR("EEPROM access\n"));
            printf_P(PSTR("Reading all values written to EEPROM:  \"eep all\"\n"));
            printf_P(PSTR("Reading a single value from EEPROM:    \"eep [ADDR]\", address range: [0x0, 0x%03X]\n"), EEPROM_ADDRESS_LIMIT);
            printf_P(PSTR("Writing a single value to EEPROM:      \"eep [ADDR] [VAL]\", value range: [0x0, 0xFF]\n"));
        }
    }

    // sfr = special function register access [ADDR] [-/VAL]
    else if (strcmp(cmd, "sfr") == 0 && *login_status == 1)
    {
        uint16_t address = 0;
        uint16_t hexValue = 0;
        uint8_t intFlag = 0;
        char readHexFailed = '\0';
        if ((param = cliGetNextToken()) != NULL)
        {
            if (sscanf(param, "%x%c", &address, &readHexFailed) == 1 && address <= 0xFF)
            {
                printf_P(PSTR("SFR reading at address 0x%02X: "), address);
                if((address >= 0x84 && address <= 0x8A) || address == 0xC4)
                {
                    printf_P(PSTR("0x%02X\n"), cmdRead16BitRegister((uint8_t) address));
                    intFlag = 1;
                }
                else
                    printf_P(PSTR("0x%02X\n"), cmdRead8BitRegister((uint8_t) address));
            }
            else
                printf_P(PSTR("SFR address not valid: %s\n"), param);

            if ((param = cliGetNextToken()) != NULL && address <= 0xFF)
            {
                if (sscanf(param, "%x%c", &hexValue, &readHexFailed) == 1)
                {
                    printf_P(PSTR("SFR writing at address 0x%02X: "), address);
                    if (intFlag)
                    {
                        printf_P(PSTR("0x%04X "), hexValue);
                        _delay_ms(1);
                        cmdWrite16BitRegister((uint8_t) address, hexValue);
                    }
                    else
                    {
                        printf_P(PSTR("0x%02X "), (uint8_t) hexValue);
                        _delay_ms(1);
                        cmdWrite8BitRegister((uint8_t) address, (uint8_t) hexValue);               
                    }                  
                }
                else
                    printf_P(PSTR("Wrong parameter: %s\n"), param);
                
                if ((param = cliGetNextToken()) != NULL)
                {
                    if (strcmp(param, "wte") == 0)
                    {
                        printf_P(PSTR("writing to EEPROM at address: 0x%03X"), address + SFR_IN_EEPROM_OFFSET);
                        if(intFlag)
                            eeprom_write_word((uint16_t*) (uintptr_t) (address + SFR_IN_EEPROM_OFFSET), hexValue);
                        else
                            eeprom_write_byte((uint8_t*) (uintptr_t) (address + SFR_IN_EEPROM_OFFSET), (uint8_t)hexValue);
                    }
                }
                printf_P(PSTR("\n"));
            }
        }
        else
        {
            printf_P(PSTR("Special function register access\n"));
            printf_P(PSTR("Reading a Special Function Register:   \"sfr [ADDR]\", address range: [0x0, 0xFF]\n"));
            printf_P(PSTR("Writing a Special Function Register:   \"sfr [ADDR] [VAL]\", value range: [0x0, 0xFFFF]\n"));
            printf_P(PSTR("Writing the SFR-value also to EEPROM:  \"sfr [ADDR] [VAL] wte\", EEPROM-address: ADDR+0x%03X\n"), SFR_IN_EEPROM_OFFSET);
        }
    }    

    // rst = reset
    else if (strcmp(cmd, "rst") == 0)
    {
        printf_P(PSTR(CLEAR_SCREEN SHOW_CURSOR));
        wdt_enable(WDTO_15MS); // Enable the WDT and set its timeout to 15ms
        while(1); // Wait for the WDT to reset the microcontroller
    }

    // rb = ring buffer (holds the last command line inputs)
    else if (strcmp(cmd, "rb") == 0 && *login_status == 1)
    {
        cliPrintRingBuffer();
    }

    // Unknown Command
    else
    {
        //printf_P(PSTR("Unknown command: "));
        //printf_P(PSTR("[%s]\n"), cmd);
        if (eeprom_read_byte(&commandHistoryFlag) == 1)
            cliPrintCmdHistory();
        return 0;
    }

    if (eeprom_read_byte(&commandHistoryFlag) == 1)
        cliPrintCmdHistory();

    return 1;
}

// Shows the default commands
void cmdShowDefaultCommands(uint8_t superuser_flag)
{
    printf_P(PSTR(      HIDE_CURSOR
                        "Default commands\n"
                        "Enter any command (Cmd) without parameter for help or status information\n\n"
                        TXT_UNDERLINED "Cmd\tDescription\t\t\tParameter\t\tShortcut\n" TXT_RESET_FORMAT
                        "ac\tApplication commands\t\t-\t\t\tCtrl+A\n"
                        "cd\tCommand details\t\t\t[0/1]\t\t\t-\n"
                        "ce\tCommand echo\t\t\t[0/1]\t\t\t-\n"));

    #ifdef UART_ISR_CHARACTER_ECHOING                        
    printf_P(PSTR(      "ch\tCommand history\t\t\t[0/1]\t\t\t-\n"));
    #endif  

    printf_P(PSTR(      "clh\tClear history\t\t\t-\t\t\t-\n"
                        "cls\tClear screen\t\t\t-\t\t\tCtrl+L\n"
                        "dc\tDefault commands\t\t-\t\t\tCtrl+D\n"
                        "rst\tReset\t\t\t\t-\t\t\t-\n"
                        "sb\tStatus bar\t\t\t[0/1]\t\t\t-\n"
                        "su\tSwitch user\t\t\t-\t\t\tCtrl+U\n"));

    #ifdef SHOW_TERMINAL_SHORTCUTS
    printf_P(PSTR(                        
                        "-\tLoad previous command(s)\t-\t\t\tUp-Arrow\n"
                        "-\tDelete current input\t\t-\t\t\tDown-Arrow\n"
                        "-\tMove cursor right\t\t-\t\t\tRight-Arrow\n"
                        "-\tMove cursor left\t\t-\t\t\tLeft-Arrow\n"
                        "-\tSet cursor home\t\t\t-\t\t\tHome\n"
                        "-\tSet cursor to end\t\t-\t\t\tEnd\n"
                        "-\tDelete after cursor position\t-\t\t\tDel\n"
                        "-\tDelete before cursor position\t-\t\t\tBackspace\n"));
    #endif

    if (superuser_flag == 1)
        printf_P(PSTR(  TXT_UNDERLINED 
                        "\nSU-Cmd\tDescription\t\t\tParameter\t\tShortcut\n" TXT_RESET_FORMAT
                        "cle\tClear EEPROM\t\t\t[all/var/sfr]\n"
                        "eep\tEEPROM access\t\t\t[ADDR/all] [-/VAL]\n"
                        "rb\tRing buffer\t\t\t-\t\t\tCtrl+Y\n"
                        "sfr\tSFR access\t\t\t[ADDR] [-/VAL] [-/wte]\n"));

    printf_P(PSTR(SHOW_CURSOR));
}

// Shows the application commands
void cmdShowApplicationCommands()
{
    printf_P(PSTR(      HIDE_CURSOR
                        "Application commands\n"
                        "Enter any command (Cmd) without parameter for help or status information\n\n"
                        TXT_UNDERLINED "Cmd\tDescription\t\t\tParameter\t\tShortcut\n" TXT_RESET_FORMAT SHOW_CURSOR));
}

// Sets a flag to store 0/1 information in EEPROM
void cmdSetFlag(uint8_t *variable)
{
    uint8_t flag = eeprom_read_byte(variable);
    char *param = cliGetNextToken();
    if (*param == '1' || (*param != '0' && flag == 1))
    {
        if (flag != 1)
        {
            eeprom_write_byte(variable, 1);
            printf_P(PSTR(" changed"));
        }
        printf_P(PSTR(": on\n"));
    }
    else if (*param == '0' || flag == 0xFF)
    {
        if (flag != 0xFF)
        {
            eeprom_write_byte(variable, 0xFF);
            printf_P(PSTR(" changed"));
        }
        printf_P(PSTR(": off\n"));
    }
    else
        printf_P(PSTR("\n"));
}

// Reads 8 Bit SFR
uint8_t cmdRead8BitRegister(uint8_t address)
{
    // Convert the address to pointer: volatile uint8_t *
    volatile uint8_t *regPtr = (volatile uint8_t *)(uintptr_t)address;
    // Dereference the pointer to read the value from the register
    return *regPtr;
}

// Writes 8 Bit SFR
void cmdWrite8BitRegister(uint8_t address, uint8_t byte)
{
    // Convert the address to pointer: volatile uint8_t *
    volatile uint8_t *regPtr = (volatile uint8_t *)(uintptr_t)address;
    // Assign byte to dereferenced register pointer
    *regPtr = byte;
}

// Reads 16 Bit SFR
uint16_t cmdRead16BitRegister(uint8_t address)
{
    // Convert the base address to pointer: volatile uint16_t *
    volatile uint16_t *regPtr = (volatile uint16_t *)((uintptr_t)address);
    // Read the value from the register
    return *regPtr;
}

// WriteSFR16
void cmdWrite16BitRegister(uint8_t address, uint16_t value)
{
    // Convert the base address to pointer: volatile uint16_t *
    volatile uint16_t *regPtr = (volatile uint16_t *)((uintptr_t)address);
    // Write the value to the register
    *regPtr = value;
}

// Updates special function register at "address" from EEPROM
uint8_t cmdUpdateSfrFromEEPROM(uint8_t address)
{
    uint8_t *reg = (uint8_t *) (uintptr_t) address;
    const uint8_t *eepromPtr = (const uint8_t *) (uintptr_t) (address + SFR_IN_EEPROM_OFFSET);
    uint8_t eepromValue  = eeprom_read_byte (eepromPtr);

    if (eepromValue != 0xFF)
    {
        printf_P(PSTR("Value 0x%02X loaded from EEPROM, updating SFR at address: 0x%02X\n"), eepromValue, address);
        *reg = eepromValue;
        return eepromValue;
    }
    else
        return *reg;
}

// Updates all special function registers from EEPROM
void cmdUpdateAllSfrFromEEPROM()
{
    uint8_t sfr_address = 0xFF;
    while(sfr_address--)
        cmdUpdateSfrFromEEPROM(sfr_address);
    cmdUpdateSfrFromEEPROM(0);
}

// Switches user from standard to superuser or vice versa
void cmdSwitchUser(uint8_t *login_status, uint8_t echoAllCommandsFlag)
{
    char *cmd;
    unsigned long seconds_change = 0;
    printf_P(PSTR("Switch user\n"));
    if (*login_status == 0)
    {
        cliEnablePwdChar();
        cliPrintPrompt(NULL, "Password>", CMD_LEVEL);
        while (!cliProcessRxData())
        {
            if (cliGetStatusBarFlag() == 1 && seconds_change != timer2GetSeconds())
            {
                seconds_change = timer2GetSeconds();
                cliPrintStatusBar(HIDE_CURSOR_ON);
            }
        }

        if((cmd = cliGetFirstToken()) != NULL)
        {
            if (echoAllCommandsFlag == 1)
            {
                for (int length = strlen(cmd); length; length--)
                    printf_P(PSTR("*"));
                printf_P(PSTR("\n"));
            }

            if (strcmp(cmd, "bhme20") == 0)
            {
                *login_status = 1;
                printf_P(PSTR("Press Ctrl+D or enter \"dc\" as " TXT_GREEN_BRIGHT TXT_BOLD "superuser" TXT_RESET_FORMAT " for " TXT_WHITE_BRIGHT "extended " TXT_RESET_FORMAT TXT_UNDERLINED "d" TXT_RESET_FORMAT "efault " TXT_UNDERLINED "c" TXT_RESET_FORMAT "ommands\n"));
            }
            else
                printf_P(PSTR(TXT_RED_BRIGHT "Denied" TXT_RESET_FORMAT "\n"));
        }
        else
            printf_P(PSTR(TXT_RED_BRIGHT "Denied" TXT_RESET_FORMAT "\n"));
        cliDisablePwdChar();
    }
    else
        *login_status = 0;
}