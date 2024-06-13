/**
 * File:            main.cpp
 * Author:          Thomas Jerman
 * Date Created:    20.05.2023
 *
 * Description:
 * CLASSIE Shell - Command Line And Synchronous Software Instruction Execution - A Microcontroller Shell
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
#include "sfr328p.h"
#include "statusbar.h"

#define STANDARD_PROMPT     "AVR>"
#define SUPERUSER_PROMPT    "SU@AVR>"

// main-Function
int main()
{
    uint8_t logged_in = 0;
    uint32_t seconds = 1;

    // CLI INITIALISATION
	cliInit(76800);                 // Initialize UART
    cliSetStatusBar(statusBar);     // Set application's status bar print function
    cmdUpdateAllSfrFromEEPROM();    // Update all SFRs with values stored in EEPROM

    // WELCOME TEXT
	printf_P(PSTR(CLEAR_SCREEN TXT_RESET_FORMAT TXT_GREEN "Robotic Nano Command Line Interface" TXT_RESET_FORMAT "\n"));
    printf_P(PSTR("Compiled on " __DATE__ " at " __TIME__ "\n"));
    printf_P(PSTR("Press Ctrl+D or enter \"dc\" to list " TXT_UNDERLINED "d" TXT_RESET_FORMAT "efault " TXT_UNDERLINED "c" TXT_RESET_FORMAT "ommands\n"));

    // CONTROLLER INITIALISATION
    timer2CTCInit();                // Timer2 init with a cycle time of 125 us used for task execution within its ISR(TIMER2_COMPA_vect)
    cmdUpdateAllSfrFromEEPROM();    // Update all SFR with values stored in EEPROM
        
    // SYSTEM PROMPT
    cmdExecuteCommand(&logged_in);  // Call cmdExecuteCommand to load printStatusBarFlag
    cliPrintPrompt(TXT_GREEN, STANDARD_PROMPT, 0); // Print UART prompt to show, that the ISR-driven UART interface is available
   
    DDRB |= (1 << PB5); // Set PB5 as output
    
    while (1)
	{  
        if (cliProcessRxData())     // If cliProcessRxData() returns 1, a received command line can be processed
		{
			cmdExecuteCommand(&logged_in);   // Executes all CLI-commands

			if (logged_in)
				cliPrintPrompt(TXT_BOLD TXT_GREEN, SUPERUSER_PROMPT, MAIN_LEVEL);   // Print UART prompt to show, that the ISR-driven UART interface is available
			else
				cliPrintPrompt(TXT_GREEN, STANDARD_PROMPT, MAIN_LEVEL);    // Print UART prompt to show, that the ISR-driven UART interface is available
		}
        
        if (cliGetStatusBarFlag() == 1 && seconds != timer2GetSeconds())
        {
            seconds = timer2GetSeconds();
            cliPrintStatusBar(HIDE_CURSOR_ON);
        }
	}
}

// TIMER2 ISR
ISR(TIMER2_COMPA_vect)
{
    static unsigned char timeSlot = 0; 
    static unsigned int milliSecondCounter = 0;

    timer2Tic(timeSlot);

    if (UCSR0A & (1 << RXC0))   // UART-polling
        cliReceiveByte(UDR0);
    
    switch(timeSlot & 0x07)
    {
        case 0: // timeSlot 0                   

                // Counting seconds
                if (milliSecondCounter == 1000)
                {
                    PORTB &= ~(1 << PB5);
                    timer2IncrementSeconds();
                    milliSecondCounter = 0;
                }
                if(milliSecondCounter == 250)
                    PORTB |= (1 << PB5);
                milliSecondCounter++;
                break;

        case 1: //timeSlot 1
                break;

        case 2: //timeSlot 2
                break;

        case 3: //timeSlot 3     
                break;

        case 4: //timeSlot 4     
                break;

        case 5: //timeSlot 5     
                break;

        case 6: //timeSlot 6      
                break;

        case 7: //timeSlot 7       
                break;                          
    }
    timer2Toc(timeSlot++);
    // reset the Output Compare Flag 2 A if
    // set during the execution of this ISR
    TIFR2 |= (1 << OCF2A);  
}