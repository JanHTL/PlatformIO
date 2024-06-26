/*
 * File:            cli.h
 * Author:          Thomas Jerman
 * Date Created:    20.05.2023
 * Version: 1.0:    10.03.2024
 * Last Modified:   10.03.2024
 *
 * Description:
 * Providing a Command Line Interface using the AVR-UART
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * jr@htl-kaindorf.at
 */

#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// GLOBAL DEFINES
/****************************************************/

#define UART_ISR_CHARACTER_ECHOING
#define RCV_BUFFER_PRINTING
#define HIST_BUFFER_PRINTING
#define CURSOR_HIDING

#define REC_CHAR_MAX                0x1F    // only 0x1F or 0x3F possible

// Supported ASCII CTRL-Characters by Serial Monitor (VSC) and PuTTY, source: https://www.physics.udel.edu/~watson/scen103/ascii.html
#define CTRL_A                      0x01    // Start of heading
#define CTRL_C                      0x03    // End of text
#define CTRL_D                      0x04    // End of xmit
#define CTRL_L                      0x0C    // Form feed
#define CTRL_U                      0x15    // Neg acknowledge
#define CTRL_X                      0x18    // Cancel
#define CTRL_Y                      0x19    // End of Medium
#define CTRL_Z                      0x1A    // Substitutem 

// Not Supported ASCII CTRL-Characters by Serial Monitor (VSC), but supported by PuTTY
// #define CTRL_B                   0x02    // Start of text (also used by VSC)
// #define CTRL_F                   0x06    // Acknowledge (also used bei VSC)
// #define CTRL_G                   0x07    // BELL (also used by VSC)
// #define CTRL_H                   0x08    // Backspace (DO NOT USE AS SENDER, INTERFERES WITH PROMPT)
// #define CTRL_J                   0x0A    // Line Feed (not sent by Serial Monitor of VSC)
// #define CTRL_K                   0x0B    // Vertical tab
// #define CTRL_M                   0x0D    // Carriage feed (acts as ENTER)
// #define CTRL_N                   0x0E    // Shift out (also used by VSC)
// #define CTRL_O                   0x0F    // SHift in (also used by VSC)
// #define CTRL_P                   0x10    // Data line escape (also used by VSC)
// #define CTRL_Q                   0x11    // Device control 1 (also used by VSC)
// #define CTRL_R                   0x12    // Device control 2 (also used by VSC)
// #define CTRL_S                   0x13    // Device control 3 (also used by VSC)
// #define CTRL_T                   0x14    // Device control 4 (also used by VSC)
// #define CTRL_V                   0x16    // Synchronous idle (also used by VSC)
// #define CTRL_W                   0x17    // End of xmit block (also used by VSC)

// Not Supported ASCII CTRL-Characters by both Serial Monitor (VSC) AND PuTTY
// #define CTRL_E                   0x05    // Enquiry (also used bei VSC and PUTTY)
// #define CTRL_I                   0x09    // Horizontal tab (not sent by Serial Monitor of VSC and PUTTY)
// #define CTRL_ESC                 0x1B    // Escape, CTRL-[ (not sent by Serial Monitor of VSC)
// #define CTRL_FS                  0x1C    // File separator, CTRL-\ (not sent by Serial Monitor of VSC)
// #define CTRL_GS                  0x1D    // Group separator, CTRL-] (not sent by Serial Monitor of VSC)
// #define CTRL_RS                  0x1E    // Record separator, CTRL-^ (not sent by Serial Monitor of VSC)
// #define CTRL_US                  0x1F    // Unit seperator, CTRL-_ (not sent by Serial Monitor of VSC)

// Keys transmitted as escape sequences
#define UP_ARROW_KEY                0x41
#define DOWN_ARROW_KEY              0x42
#define RIGHT_ARROW_KEY             0x43
#define LEFT_ARROW_KEY              0x44
#define END                         0x46
#define POS1                        0x48
#define DEL                         0x33
#define END_ISO8850                 0x34
#define POS1_ISO8850                0x31

// Additional defines
#define BELL                        0x07    // to be sent from controller to terminal
#define BACKSPACE                   0x08    // to be sent from controller to terminal
#define ESCAPE                      0x1B
#define DELETE                      0x7F    // to be received from controller

// SGR (Select Graphic Rendition) parameters, source: https://en.wikipedia.org/wiki/ANSI_escape_code#SGR
#define TXT_RESET_FORMAT            "\e[0m"
#define TXT_BOLD                    "\e[1m"
#define TXT_DIM                     "\e[2m"
#define TXT_ITALIC                  "\e[3m"
#define TXT_UNDERLINED              "\e[4m"
#define TXT_BLINK                   "\e[5m"
#define TXT_BLINK_RAPIDLY           "\e[6m"
#define TXT_COLOR_REVERSE           "\e[7m"
#define TXT_HIDDEN                  "\e[8m"
#define TXT_STRIKED_OUT             "\e[9m"
#define TXT_UNDERLINED_TWICE        "\e[21m"
#define TXT_BLACK                   "\e[30m"
#define TXT_RED                     "\e[31m"
#define TXT_GREEN                   "\e[32m"
#define TXT_YELLOW                  "\e[33m"
#define TXT_BLUE                    "\e[34m"
#define TXT_MAGENTA                 "\e[35m"
#define TXT_CYAN                    "\e[36m"
#define TXT_WHITE                   "\e[37m"
#define BCKGRND_BLACK               "\e[40m"
#define BCKGRND_RED                 "\e[41m"
#define BCKGRND_GREEN               "\e[42m"
#define BCKGRND_YELLOW              "\e[43m"
#define BCKGRND_BLUE                "\e[44m"
#define BCKGRND_MAGENTA             "\e[45m"
#define BCKGRND_CYAN                "\e[46m"
#define BCKGRND_WHITE               "\e[47m"
#define TXT_FRAMED                  "\e[51m"
#define TXT_BLACK_BRIGHT            "\e[90m"
#define TXT_RED_BRIGHT              "\e[91m"
#define TXT_GREEN_BRIGHT            "\e[92m"
#define TXT_YELLOW_BRIGHT           "\e[93m"
#define TXT_BLUE_BRIGHT             "\e[94m"
#define TXT_MAGENTA_BRIGHT          "\e[95m"
#define TXT_CYAN_BRIGHT             "\e[96m"
#define TXT_WHITE_BRIGHT            "\e[97m"
#define BCKGRND_BLACK_BRIGHT        "\e[100m"
#define BCKGRND_RED_BRIGHT          "\e[101m"
#define BCKGRND_GREEN_BRIGHT        "\e[102m"
#define BCKGRND_YELLOW_BRIGHT       "\e[103m"
#define BCKGRND_BLUE_BRIGHT         "\e[104m"
#define BCKGRND_MAGENTA_BRIGHT      "\e[105m"
#define BCKGRND_CYAN_BRIGHT         "\e[106m"
#define BCKGRND_WHITE_BRIGHT        "\e[107m"
#define SHOW_CURSOR                 "\e[?25h"
#define HIDE_CURSOR                 "\e[?25l"
#define CURSOR_TO_UPPER_LEFT_POS    "\e[H"
#define CLEAR_SCREEN                "\e[2J\e[H" // Clear screen and move Curosor to upper left position

#define HIDE_CURSOR_ON              1
#define HIDE_CURSOR_OFF             0

#define MAIN_LEVEL                  0
#define CMD_LEVEL                   1

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

// Initialize UART with custom bitrate. Partiy set to EVEN,
// using 8 bit data and 2 stop bit for timing considerations
void cliInit(uint32_t bps);

// Get first token (spaced separated substring) from reveived string
char *cliGetFirstToken();

// Get next token on each consecutive call of this function, which
// processes strings under quotes as one single string
// Make sure to check if param != NULL after each function call of cliGetNextToken()
char *cliGetNextToken();

// Print and process received command and all parameters available
unsigned char cliPrintCmdDetails();

// Print current command history
void cliPrintCmdHistory();

// Reset UART to enable new data reception and
// print UART prompt to let the user know a new command can be sent
void cliPrintPrompt(const char promptFormatting[], const char prompt[], int promptLevel);

// set UART password character in the range from 'space' to '~'
void cliEnablePwdChar();

// set UART password character in the range from 'space' to '~'
void cliDisablePwdChar();

// Hand over pointer to facilitate access to received CTRL-Characters
// and arrow keys sent using escape sequences
// Return value:    1: pointer set successfully
//                  0: NUll pointer not accepted
int cliEnableCtrlKeys(char *pCtrlKey);

// Get a supported CTRL-Keys if received
char cliGetCtrlKey();

// Clear Command History
void cliClearCmdHistory();

// Hide Cursor
void cliHideCursor();

// Show Cursor
void cliShowCursor();

// Set status bar flag
void cliSetStatusBarFlag(unsigned char printStatusBarFlag);

// Get status bar flag
unsigned char cliGetStatusBarFlag();

// Set the function pointer to the user implemented status bar setup function
void cliSetStatusBar(void (*userStatusBarSetup)(void));

// Update status bar in terminal
void cliPrintStatusBar(unsigned char hideCursor);

// printf UART.rcvBuf
void cliPrintRcvdString();

// Print ring buffer
void cliPrintRingBuffer();

// Receive serial data dn store it in a ring buffer
void cliReceiveByte(char charRcvd);

// Command line interface state machine to be used with a terminal programme
unsigned char cliProcessRxData();

#ifdef __cplusplus
}
#endif

#endif // UART_H_INCLUDED