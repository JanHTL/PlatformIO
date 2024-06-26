/**
 * File:            cli.c
 * Author:          Thomas Jerman
 * Date Created:    20.05.2023
 *
 * Description:
 * Providing a Command Line Interface using the AVR-UART
 */

#include <stdio.h>
#include <string.h>

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "cli.h"

/****************************************************/
// LOCAL DEFINES
/****************************************************/

#define SIZE REC_CHAR_MAX + 1           // Do not set SIZE to more than REC_CHAR_MAX + 1
#define DELAY 255                       // Delay to slow down fast data transmitting of controller, 255 = max.

/****************************************************/
// LOCAL STRUCT DEFINITION
/****************************************************/

// Declaration of the command line interface struct
struct Cli
{
    int rcvIndex;                       // receive index
    int rcvIndexMax;                    // maximum receive index
    char rcvBuf[SIZE];                  // receive buffer
    char rcvChar;                       // received char
    int ringBufIndex;                   // ring buffer index
    char ringBuf[SIZE];                 // ring buffer
    char pwdChar;                       // password character
    char escSeqState;                   // escape sequence state machine
    char histBuf[SIZE];                 // history buffer
    int histIndex;                      // index to last processed cmd
    int lastCmdIndex;                   // index of last command
    char ctrlKey;                       // variable to save received CTRL-Key
    char *pCtrlKey;                     // pointer to handle CTRL-Keys
    unsigned char statusBarFlag;        // saves print status bar flag
    unsigned int lineFeedCounter;       // counts the number of sent line feeds
    unsigned char promptLength;         // length of prompt
    void (*cliStatusBarSetup)(void);    // function pointer to be set to the user implemented status bar setup function
};

/****************************************************/
// LOCAL STATIC STRUCT VARIABLE DECLARATION, INIT
/****************************************************/

static struct Cli this;

/****************************************************/
// LOCAL MACROS
/****************************************************/

#define moveCursorForward(t, o)         \
{                                       \
    sendWhenReady('\e');                \
    sendWhenReady('[');                 \
    sendWhenReady(t);                   \
    sendWhenReady(o);                   \
    sendWhenReady('C');                 \
}

#define moveCursorPreviousLine(t, o)    \
{                                       \
    sendWhenReady('\e');                \
    sendWhenReady('[');                 \
    sendWhenReady(t);                   \
    sendWhenReady(o);                   \
    sendWhenReady('F');                 \
}

#define clearUserInput()                \
{                                       \
    sendWhenReady('\e');                \
    sendWhenReady('[');                 \
    sendWhenReady('K');                 \
}

/****************************************************/
// LOCAL FUNCTIONS
/****************************************************/

// Define default status bar setup in case the user did not set a personal one
void defaultStatusBarSetup()
{
    printf_P(PSTR(TXT_WHITE_BRIGHT BCKGRND_RED "Status bar setup function not set! Call cliSetStatusBarSetupFunction(FUNCTION) in main.c \n"));
    printf_P(PSTR("to call your custom FUNCTION to setup the contents of the status bar to be displayed     \n" TXT_RESET_FORMAT));
}

// Configure standard output stream to use UART
int uartPutchar(char send_byte, FILE *stream)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    // Send character
    UDR0 = send_byte;
    // Count line feeds sent to control status bar printing
    if (send_byte == '\n')
        this.lineFeedCounter++;
    return 0;
}

// Restore UART buffer after command and parameters have been read
// Set all '\0' introduced by strtok back to ' '
void restoreBuffer()
{
    for (int i = 0; i < SIZE - 1; i++)
        if (this.rcvBuf[i] == '\0')
            this.rcvBuf[i] = ' ';
    this.rcvBuf[SIZE - 1] = '\n';
}

// Send byte if UART is available
void sendWhenReady(unsigned char send_byte)
{
    while (!(UCSR0A & (1 << UDRE0)));
            UDR0 = send_byte;
    for (volatile uint8_t i = 0; i < DELAY; i++);
}

// Set cursor on step right
void setCursorRight()
{
    sendWhenReady('\e');
    sendWhenReady('[');
    sendWhenReady('C');
}

// Set cursor on step left
void setCursorLeft()
{
    sendWhenReady('\e');
    sendWhenReady('[');
    sendWhenReady('D');
}

/****************************************************/
// GLOBAL FUNCTIONS
/****************************************************/

// Initialize UART with custom bitrate.
// Using 8 bit data and 1 stop bit and no parity
void cliInit(uint32_t bps)
{
    // disable global interrupt
    cli(); // is equivalent to  SREG |= SREG_I;

    /****************************************************/
    // UART-SETUP
    /****************************************************/

    // Configure stdout to be connected to UART
    static FILE uartStdout = FDEV_SETUP_STREAM(uartPutchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &uartStdout;
    
    // set baud rate
    if (bps <= 115200)
        UBRR0 = (uint16_t)(((F_CPU / (bps * 8UL))) - 1);
    else
        UBRR0 = 0x10;       //115200 with U2X0 set

    // set double baudrate bit
    UCSR0A |= (1 << U2X0);

    // set asynchronous mode
    UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00));

    // set EVEN paritiy 
    //UCSR0C |= (1 << UPM01);

    // set 2 stop bit
    //UCSR0C |= (1 << USBS0);

    // set 8 data bits
    UCSR0C |= ((1 << UCSZ01) | (1 << UCSZ00));

    // enable transmitter and receiver
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    /****************************************************/
    // STRUCT INITIALIZATION
    /****************************************************/

    // set UART.rcvBuf to be zero
    memset(this.rcvBuf, 0, SIZE);
    memset(this.histBuf, 0, SIZE);
    memset(this.ringBuf, 0, SIZE);

    // set struct variables
    this.rcvIndex = 0;
    this.ringBufIndex = 0;
    this.rcvIndexMax = 0;
    this.pwdChar = '\0';
    this.escSeqState = 0;
    this.ctrlKey = 0;
    this.pCtrlKey = NULL;
    this.histIndex = SIZE - 1;
    this.lastCmdIndex = SIZE - 1;
    this.statusBarFlag = 0;     // saves print status bar flag
    this.lineFeedCounter = 0;   // counts the number of sent line feeds
    this.promptLength = 0;      // length of prompt
    this.cliStatusBarSetup = defaultStatusBarSetup;

    // enable global interrupt
    sei(); // is equivalent to  SREG |= SREG_I;
    // delay printing the first output
    _delay_ms(100);
}

// Get first token (spaced separated substring) from reveived string
char *cliGetFirstToken()
{
    return strtok(this.rcvBuf, " \r\n");
}

// Get next token on each consecutive call of this function, which
// processes strings under quotes as one single string
// Make sure to check if param != NULL after each function call of cliGetNextToken()
char *cliGetNextToken()
{
    char *p = strtok(NULL, " \r\n"); // tokenize on next space character
    if (*p == '"')                   // check for a quote character
    {
        *strchr(p++, '\0') = ' '; // restore space character
        p = strtok(p, "\"\r\n");  // tokenize on next quote character
    }
    if (p < this.rcvBuf + SIZE)
        return p;
    else
        return NULL;
}

// Print and process received command and all parameters available
unsigned char cliPrintCmdDetails()
{
    #ifdef RCV_BUFFER_PRINTING

    int i = 0;
    if (this.rcvIndex != 0 && this.rcvBuf[0] >= ' ')
    {
            printf_P(PSTR("Received bytes:"));
        while(i <= SIZE - 1 && this.rcvBuf[i] != '\0')
        {
            if(i % 8 == 0)
                printf_P(PSTR("\n"));
            printf_P(PSTR("0x%02X "), (unsigned char)this.rcvBuf[i]);
            i++;
        }
        if (i == SIZE -1 && this.rcvBuf[i-1] != '\n' && this.rcvBuf[i] == '\0')
            printf_P(PSTR("0x0A"));
        printf_P(PSTR("\n"));
    }

    #endif  //RCV_BUF_PRINTING

    unsigned char paramCount = 0;
    if (this.rcvIndex != 0 && this.rcvBuf[0] >= ' ')
    {
        char *p;
        if ((p = cliGetFirstToken()) != NULL)
            printf_P(PSTR("Command: [%s]\n"), p);
        else
            return 0;
        do
        {
            if ((p = cliGetNextToken()) != NULL && p < this.rcvBuf + SIZE - 1)
                printf_P(PSTR("Param#%X: [%s]\n"), ++paramCount, p);
        } while (p != NULL);
        restoreBuffer();
    }
    return paramCount;
}

// Print command history
void cliPrintCmdHistory()
{
    #ifdef UART_ISR_CHARACTER_ECHOING
    #ifdef HIST_BUFFER_PRINTING

    int j = 0;
    uint8_t offset = 0;
    char histBufChar;
    if (this.histIndex != this.lastCmdIndex)
    {
        printf_P(PSTR("Command history:  "));
        while(j < SIZE)
        {
            if (++j + this.histIndex > SIZE - 1)
                offset = SIZE;
            histBufChar = this.histBuf[j + this.histIndex - offset];
            if (histBufChar >= ' ')
                printf_P(PSTR("%c"), histBufChar);
            else if (histBufChar == '\n')
                printf_P(PSTR(", "));
        }
        // delete output of last printf
        printf_P(PSTR("\b\b  \n"));
    }

    #endif  //HIST_BUFFER_PRINTING
    #endif  //UART_ISR_CHARACTER_ECHOING
}

// Reset UART to enable new data reception and
// print UART prompt to let the user know a new command can be sent
void cliPrintPrompt(const char promptFormating[], const char prompt[], int promptLevel)
{
    memset(this.rcvBuf, 0, SIZE);
    this.rcvIndex = UDR0; // read buffered UDR0 to empty it once
    this.rcvIndex = UDR0; // read buffered UDR0 to empty it twice
    this.rcvIndex = 0;
    this.rcvIndexMax = 0;
    this.rcvChar = 0;
    this.ctrlKey = 0;
    this.pCtrlKey = NULL;
    this.escSeqState = 0;
    if (cliGetStatusBarFlag() == 1 && promptLevel == 0)
    {
        this.lineFeedCounter = 0;
        this.cliStatusBarSetup();
    }
    this.promptLength = strlen(prompt);
    printf_P(PSTR("%s%s" TXT_RESET_FORMAT SHOW_CURSOR), promptFormating, prompt);
    UCSR0B |= (1 << RXEN0); // enable the UART receiver
}

// Enable UART password character '*'
void cliEnablePwdChar()
{
    this.pwdChar = '*';
}

// Disable UART password character '*'
void cliDisablePwdChar()
{
    this.pwdChar = '\0';
}

// Hand over pointer to facilitate access to received CTRL-Characters
// and arrow keys sent using escape sequences
// Return value:    1: pointer set successfully
//                  0: NUll pointer not accepted
int cliEnableCtrlKeys(char *pCtrlKey)
{
    if(pCtrlKey)
    {
        this.pCtrlKey = pCtrlKey;
        UCSR0B |= (1 << RXEN0);
        return 1;
    }
    printf_P(PSTR("Error: null pointer not accepted to enable CTRL-Keys.\n"));
    return 0;
}

// Get a supported CTRL-Keys if received
char cliGetCtrlKey()
{
    return this.ctrlKey;
}

// Clear Command History
void cliClearCmdHistory()
{
    memset(this.histBuf, 0, SIZE);
    this.histIndex = SIZE - 1;
    this.lastCmdIndex = SIZE - 1;
}

// Hide Cursor
void cliHideCursor()
{
    sendWhenReady('\e');
    sendWhenReady('[');
    sendWhenReady('?');
    sendWhenReady('2');
    sendWhenReady('5');
    sendWhenReady('l');
}

// Show Cursor
void cliShowCursor()
{
    sendWhenReady('\e');
    sendWhenReady('[');
    sendWhenReady('?');
    sendWhenReady('2');
    sendWhenReady('5');
    sendWhenReady('h');
}

// Set status bar flag
void cliSetStatusBarFlag(unsigned char statusBarFlagNew)
{
    if(statusBarFlagNew == 1)
        this.statusBarFlag = 1;
    else
        this.statusBarFlag = 0;
}

// Get status bar flag
unsigned char cliGetStatusBarFlag()
{
    return this.statusBarFlag;
}

// Set the function pointer to the user implemented status bar setup function
void cliSetStatusBar(void (*userStatusBarSetup)(void))
{
    if (userStatusBarSetup != NULL)
        this.cliStatusBarSetup = userStatusBarSetup;
    else
        this.cliStatusBarSetup = defaultStatusBarSetup;
}

// Update status bar in terminal
void cliPrintStatusBar(unsigned char hideCursor)
{
    unsigned char lineFeedsTotal = this.lineFeedCounter;
    if (lineFeedsTotal)
    {
        if (hideCursor)
            cliHideCursor();
        // Move cursor numOflineFeeds up and set it to its beginning
        moveCursorPreviousLine('0' + lineFeedsTotal / 10, '0' + lineFeedsTotal % 10);
        // Reset UART.lineFeedCounter to count how many line feeds are printed by cliPrintStatusBarLines and afterwards
        // until cliPrintStatusBar is called again. This way all postPromptLineFeeds can be counted as well
        this.lineFeedCounter = 0;
        // Print status bar lines
        this.cliStatusBarSetup();
        // Move cursor to next line(s)
        while (this.lineFeedCounter < lineFeedsTotal)
            printf("\n"); 
        // Move cursor horizontally to where it was before printing the status bar
        moveCursorForward('0' + ((this.rcvIndex + this.promptLength) / 10), '0' + ((this.rcvIndex + this.promptLength) % 10));
        if (hideCursor)
            cliShowCursor();
    }
    else this.lineFeedCounter = 0;
}

// printf UART.rcvBuf
void cliPrintRcvdString()
{
    if (this.rcvBuf[SIZE - 1] == '\n')
    {
        this.rcvBuf[SIZE - 1] = '\0';
        printf_P(PSTR("%s\n"), this.rcvBuf);
    }
    else
        printf_P(PSTR("%s"), this.rcvBuf);
}

// Print ring buffer
void cliPrintRingBuffer()
{
    printf_P(PSTR("Ring buffer\n"));
    for(int i = 0; i <= REC_CHAR_MAX; i++)
    {
        if (this.ringBuf[i] == 0x1B)
            printf_P(PSTR("%02d:0x1B ESC\n"), i);
        else if (this.ringBuf[i] == DELETE)
            printf_P(PSTR("%02d:0x7F DELETE\n"), i);
        else if (this.ringBuf[i] == '\n')
            printf_P(PSTR("%02d:0x0A LINE FEED\n"), i);
        else if (this.ringBuf[i] == '\r')
            printf_P(PSTR("%02d:0x0D CARRIAGE RETURN\n"), i);            
        else if (this.ringBuf[i] == CTRL_A)
            printf_P(PSTR("%02d:0x01 Ctrl+A\n"), i);
        else if (this.ringBuf[i] == CTRL_C)
            printf_P(PSTR("%02d:0x01 Ctrl+C\n"), i);
        else if (this.ringBuf[i] == CTRL_D)
            printf_P(PSTR("%02d:0x01 Ctrl+D\n"), i);
        else if (this.ringBuf[i] == CTRL_L)
            printf_P(PSTR("%02d:0x01 Ctrl+L\n"), i);
        else if (this.ringBuf[i] == CTRL_U)
            printf_P(PSTR("%02d:0x01 Ctrl+U\n"), i);
        else if (this.ringBuf[i] == CTRL_X)
            printf_P(PSTR("%02d:0x01 Ctrl+X\n"), i);
        else if (this.ringBuf[i] == CTRL_Y)
            printf_P(PSTR("%02d:0x01 Ctrl+Y\n"), i);
        else if (this.ringBuf[i] == CTRL_Z)
            printf_P(PSTR("%02d:0x01 Ctrl+Z\n"), i);                                                
        else
            printf_P(PSTR("%02d:0x%02X %c\n"), i, this.ringBuf[i], this.ringBuf[i]);
    }  
}

// Receive serial data dn store it in a ring buffer
void cliReceiveByte(char charRcvd)
{
    this.ringBuf[this.ringBufIndex++ & REC_CHAR_MAX] = charRcvd;
    this.ringBuf[this.ringBufIndex & REC_CHAR_MAX] = '\0';
} 

// Command line interface state machine to be used with a terminal programme
unsigned char cliProcessRxData()
{
    static unsigned int rcvIndex = 0;
    if (this.ringBuf[rcvIndex & REC_CHAR_MAX] != '\0')
    { 
        #ifdef UART_ISR_CHARACTER_ECHOING
            #ifdef CURSOR_HIDING
                static unsigned char hideCursorFlag = 0;
            #endif
        #endif
        //STEP A - CHARACTER RECEIVING
        // save received character
        this.rcvChar = this.ringBuf[rcvIndex++ & REC_CHAR_MAX];

        //change UART.charRcvd to \n in case it is \r
        this.rcvChar = this.rcvChar != '\r' ? this.rcvChar : '\n';
        
        #ifdef UART_ISR_CHARACTER_ECHOING

        // check supported CTRL-characters
        if(this.rcvChar < ESCAPE)
        {
            // only allow supported ASCII CTRL-Characters by Serial Monitor (VSC) and PuTTY
            if (this.rcvChar == CTRL_A || this.rcvChar == CTRL_C || this.rcvChar == CTRL_D ||
                this.rcvChar == CTRL_L || this.rcvChar == CTRL_U || this.rcvChar == CTRL_X ||
                this.rcvChar == CTRL_Y || this.rcvChar == CTRL_Z)
                {
                    // handle Ctrl-keys during command execution
                    if(this.pCtrlKey)
                        *this.pCtrlKey = this.rcvChar;
                    else
                    {
                        // handle Ctrl-key to run trigger command execution and stop receiving
                        this.ctrlKey = this.rcvChar;
                        this.lineFeedCounter++;
                        UDR0 = '\n';
                    }
                    return 1;
                }
        }

        // Suppress any non-printable character except the new line character
        // by ending the if-else-if structure here, so no further code is being executed in the ISR
        if(this.rcvChar != '\n' && this.rcvChar != ESCAPE && this.rcvChar <= 0x1F)
            return 0;

        //STEP B - HANDLING DELETE-KEY
        // character deletion on backspace key
        else if (this.rcvChar == DELETE && !this.pCtrlKey)
        {
            // delete characters if cursor is not at position zero
            if (this.rcvIndex > 0)
            {
                // delete last character in command line by moving back
                // the cursor to overwrite existing terminal character(s)
                UDR0 = BACKSPACE;
                // delete last character in UART.rcvBuf
                if (this.rcvIndex == this.rcvIndexMax)
                {
                    sendWhenReady(' ');                     // overwrite last character to the right
                    sendWhenReady(BACKSPACE);               // move cursor left again
                    this.rcvBuf[--this.rcvIndex] = '\0';
                    this.rcvIndexMax--;
                }
                // delete character within UART.rcvBuf
                else
                {
                    #ifdef CURSOR_HIDING
                    if (!this.pCtrlKey)
                    {    //hideCursor(hideCursorFlag);
                        cliHideCursor();
                        hideCursorFlag = 1;
                    }
                    #endif
                    // go back to the position of the character to be deleted and copy
                    // all following characters one position to the left
                    for (uint8_t i = --this.rcvIndex; i < this.rcvIndexMax; i++)
                        sendWhenReady(this.rcvBuf[i] = this.rcvBuf[i + 1]);
                    this.rcvIndexMax--;
                    // overwrite last character in command line with a space character
                    sendWhenReady(' ');
                    // set back cursor in command line where it was before overwritting characters
                    for (uint8_t i = this.rcvIndex; i < this.rcvIndexMax + 1; i++)
                        setCursorLeft();
                }
            }
        }

        //STEP C - HANDLING MULTI-BYTE ESCAPE SEQUENCES
        // catch escape sequences (Arrow-Keys, POS1, DEL-KEY (ENTF) and END Key)
        else if (this.rcvChar == 0x1B)
                this.escSeqState = 1;
        else if ((this.rcvChar == 0x4F || this.rcvChar == 0x5B) && this.escSeqState == 1)
                this.escSeqState = 2;
        else if (this.rcvChar >= 0x31 && this.rcvChar <= 0x4F && this.escSeqState == 2)
        {
            // Save the received key, when UART.pCtrlKey is set
            if (this.pCtrlKey)
                *this.pCtrlKey = this.rcvChar;
            else
            {
                // Save type of key for later handling of OS1, DEL-KEY (ENTF) and END Keys
                this.escSeqState = this.rcvChar;
                // Arrow-Key handling if no password character is set
                if(this.pwdChar == '\0')
                {
                    if (this.rcvChar >= UP_ARROW_KEY)
                    {
                        #ifdef CURSOR_HIDING
                            if (!this.pCtrlKey)
                            {
                                //hideCursor(hideCursorFlag);
                                cliHideCursor();
                                hideCursorFlag = 1;
                            }
                        #endif

                        // overwrite current command line with space characters on UP- and DOWN Arrow-Key
                        if (this.rcvChar == UP_ARROW_KEY || this.rcvChar == DOWN_ARROW_KEY)
                        {
                            if(this.rcvIndexMax)
                            {                        
                                // move cursor to the very left input position
                                while(this.rcvIndex)
                                {
                                    setCursorLeft();
                                    this.rcvIndex--;
                                }

                                // delete all input characters shown in terminal
                                clearUserInput();

                                // overwrite all characters stored in UART.rcvBuf
                                while (this.rcvIndex < this.rcvIndexMax)
                                    this.rcvBuf[this.rcvIndex++] = '\0';

                                // restore empty buffer variable states
                                this.rcvIndex = 0;
                                this.rcvIndexMax = 0;

                                // reset UART.lastCommandIndex to last entered command
                                if (this.rcvChar == DOWN_ARROW_KEY)
                                    this.lastCmdIndex = this.histIndex + 1 > SIZE - 1 ? this.histIndex + 1 - (SIZE) : this.histIndex + 1;
                            }
                        }
                        // restore previous command line entered to the console on "arrow up" = 0x41
                        if (this.rcvChar == UP_ARROW_KEY)
                        {
                            int8_t histIndex = this.lastCmdIndex;
                            int8_t counter = 0;           
                            while(this.histBuf[histIndex] != 0x0A && counter++ <= SIZE)
                            {
                                if(this.histBuf[histIndex] != 0x00)
                                {
                                    this.rcvBuf[this.rcvIndex++] = this.histBuf[histIndex];
                                    sendWhenReady(this.histBuf[histIndex]);
                                    this.rcvIndexMax++;
                                }
                                histIndex = ++histIndex > SIZE - 1 ? histIndex - (SIZE) : histIndex;
                            }
                            this.lastCmdIndex = ++histIndex > SIZE - 1 ? histIndex - (SIZE) : histIndex;                        
                        }
                        // echo RIGHT-Arrow-Key
                        if (this.rcvChar == RIGHT_ARROW_KEY)
                            if (this.rcvBuf[this.rcvIndex] != '\0')
                                setCursorRight(this.rcvIndex++);
                        // echo LEFT-Arrow-Key
                        if (this.rcvChar == LEFT_ARROW_KEY)
                            if (this.rcvIndex)
                                setCursorLeft(this.rcvIndex--);
                        // handle END-Key for Serial Monitor in VSC
                        if(this.escSeqState == END)
                            while(this.rcvIndex < this.rcvIndexMax)
                            {
                                setCursorRight();
                                this.rcvIndex++;
                            }
                        // handle POS1-Key for Serial Monitor in VSC
                        if(this.rcvChar == POS1)
                            while(this.rcvIndex)
                            {
                                setCursorLeft();
                                this.rcvIndex--;
                            }
                    }
                }
            }
        }
        // suppress all CTRL+ARROW-KEYS from VSC
        else if (this.rcvChar == 0x3B && this.escSeqState == 0x31)
                this.escSeqState = 0x3B;
        else if (this.rcvChar == 0x32 && this.escSeqState == 0x3B)
                this.escSeqState = 0x32;
        else if (this.rcvChar >= 0x41 && this.rcvChar <= 0x48 && this.escSeqState == 0x32)
                return 0;                
        else if (this.rcvChar == 0x35 && this.escSeqState == 0x3B)
                this.escSeqState = 0x3C;
        else if (this.rcvChar >= 0x41 && this.rcvChar <= 0x44 && this.escSeqState == 0x3C)
                return 0;
        // suppress CTRL+ENTF from VSC
        else if (this.rcvChar == 0x3B && this.escSeqState == 0x33)
                this.escSeqState = 0x3D;
        else if (this.rcvChar == 0x35 && this.escSeqState == 0x3D)
                return 0;
        // suppress CTRL+PageUp from VSC
        else if (this.rcvChar == 0x3B && this.escSeqState == 0x35)
                this.escSeqState = 0x3E;
        else if (this.rcvChar == 0x35 && this.escSeqState == 0x3E)
                return 0;
        // suppress CTRL+PageDown from VSC
        else if (this.rcvChar == 0x3B && this.escSeqState == 0x36)
                this.escSeqState = 0x3F;
        else if (this.rcvChar == 0x35 && this.escSeqState == 0x3F)
                return 0;
        // suppress CTRL+END from VSC
        else if (this.escSeqState == 0x3C && this.rcvChar == 0x46)
                return 0;   
        // suppress CTRL+POS1 from VSC
        else if (this.escSeqState == 0x3C && this.rcvChar == 0x48)
                return 0;
        // handle POS1, DEL-KEY (ENTF) and END Key
        else if (this.rcvChar == 0x7E && this.escSeqState != 0)
        {
            if(!this.pCtrlKey)
            {
                // only handle keys if no password character is set
                if(this.pwdChar == '\0')
                {
                    #ifdef CURSOR_HIDING
                    if (!this.pCtrlKey)
                    {
                        //hideCursor(hideCursorFlag);
                        cliHideCursor();
                        hideCursorFlag = 1;
                    }
                    #endif

                    // handle POS1-Key
                    if(this.escSeqState == POS1_ISO8850)
                        while(this.rcvIndex)
                            setCursorLeft(this.rcvIndex--);
                    // handle DEL-Key
                    if(this.escSeqState == DEL)
                    {
                        if (this.rcvIndex < this.rcvIndexMax && this.rcvChar != '\n')
                        {
                            for (int8_t i = this.rcvIndex; i < this.rcvIndexMax; i++)
                                sendWhenReady(this.rcvBuf[i] = this.rcvBuf[i + 1]);
                            sendWhenReady(' ');
                            this.rcvIndexMax--;
                            for (int8_t i = this.rcvIndex; i <= this.rcvIndexMax; i++)
                                setCursorLeft();
                        }
                    }
                    // handle END-Key
                    else if(this.escSeqState == END_ISO8850)
                        while(this.rcvIndex < this.rcvIndexMax)
                        {
                            setCursorRight();
                            this.rcvIndex++;
                        }
                }
            }
        }

        //STEP D - PROCESS STANDARD CHARACTER RECEIVING
        // standard character receiving
        else if (this.rcvIndexMax < SIZE - 1 && !this.pCtrlKey)
        {
            // insert characters within UART.rcvBuf if UART.rcvIndex < UART.rvcIndexMax
            if (this.rcvIndex < this.rcvIndexMax && this.rcvChar != '\n')
            {
                for (int8_t i = this.rcvIndexMax; i >= this.rcvIndex; i--)
                    this.rcvBuf[i + 1] = this.rcvBuf[i];
                this.rcvIndexMax++;
            }
            // password masking
            if (this.pwdChar != '\0' && this.rcvChar != '\n')
                UDR0 = this.pwdChar;
            // echo every character to sender
            else
            {
                if (this.rcvChar == '\n')
                {
                    cliHideCursor();
                    this.lineFeedCounter++;
                }
                UDR0 = this.rcvChar;
            }
            // store command line end character after all received characters in UART.rcvBuf
            if (this.rcvChar == '\n')
                this.rcvBuf[this.rcvIndexMax] = this.rcvChar;
            else
            {
                // update command line due to character insertion within UART.rcvBuf
                if (this.rcvIndex < this.rcvIndexMax)
                {   
                    #ifdef CURSOR_HIDING
                    if (!this.pCtrlKey)
                    {
                        //hideCursor(hideCursorFlag);
                        cliHideCursor();
                        hideCursorFlag = 1;
                    }
                    #endif

                    // rewrite all characters to the right of the inserted character
                    for (int8_t i = this.rcvIndex + 1; i <= this.rcvIndexMax; i++)
                        sendWhenReady(this.rcvBuf[i]);
                    // set cursor left again to position it where it was before
                    for (int8_t i = this.rcvIndex + 1; i < this.rcvIndexMax; i++)
                        setCursorLeft();
                    this.rcvBuf[this.rcvIndex++] = this.rcvChar;
                }
                else
                {
                    // add normally received character to UART.rcvBuf and increase indices
                    this.rcvBuf[this.rcvIndex++] = this.rcvChar;
                    this.rcvIndexMax++;
                }
            }
        }
        
        //STEP E - HANDLING LINE ENDING (NEW-LINE) ON FULL RECEIVE BUFFER 
        // line-end character returning when hitting enter but receive buffer is full
        else if (this.rcvChar == '\n' && !this.pCtrlKey)
        {
            cliHideCursor();
            UDR0 = '\n';
            this.lineFeedCounter++;
            this.rcvBuf[SIZE - 1] = '\n';
        }
        
        //STEP F - CLOSING DATA RECEIVING ON NEW-LINE CHARACTER
        // disable receive interrupt after receiving '\n'
        if (this.rcvChar == '\n' && !this.pCtrlKey)
        {
            int searchResult = 0;
            int compareIndex, index = 0;        
            if(this.rcvIndex != 0)
            {
                // compare current command only with last added command in histBuf
                compareIndex = this.histIndex + 1 > SIZE - 1 ? this.histIndex + 1 - (SIZE) : this.histIndex + 1;
                if (this.histBuf[compareIndex] == this.rcvBuf[0])
                {
                    do
                    {
                        index++;
                        compareIndex = compareIndex + index > SIZE - 1 ? compareIndex - (SIZE) : compareIndex;
                    } while (index <= this.rcvIndex && this.histBuf[compareIndex + index] == this.rcvBuf[index]);
                    if (index == this.rcvIndex + 1)
                        searchResult = 1;
                }

                this.lastCmdIndex = this.histIndex + 1 > SIZE - 1 ? this.histIndex + 1 - (SIZE) : this.histIndex + 1;
                if (searchResult == 0)
                {
                    // copy the received string into UART.histBuf, if no pwdChar is set
                    if (this.pwdChar == '\0' && this.rcvIndexMax >= 1)           //UART.rvcIndexMax > 1
                    {         
                        for (int8_t counter = 0; counter <= this.rcvIndexMax; counter++)
                        {
                            this.histBuf[this.histIndex--] = this.rcvBuf[this.rcvIndexMax - counter];
                            this.histIndex = this.histIndex < 0 ? this.histIndex + SIZE : this.histIndex;
                        }
                        this.histIndex = this.histIndex < 0 ? this.histIndex + SIZE : this.histIndex;

                        // delete partly overwritten commands in UART.histBuf
                        if (this.histBuf[this.histIndex] != 0x00 && this.histBuf[this.histIndex] != 0x0A)
                        {
                            int index = this.histIndex;
                            for(int8_t counter = 0; this.histBuf[index] != 0x0A && counter < SIZE; counter++)
                            {
                                this.histBuf[index--] = 0x00;
                                index = index < 0 ? index + SIZE : index;        
                            }
                        }
                        // set index for last added command
                        this.lastCmdIndex = this.histIndex + 1 > SIZE - 1 ? this.histIndex + 1 - (SIZE) : this.histIndex + 1;
                    }
                }
            }
            UCSR0B &= ~(1 << RXEN0); // disable the UART receiver
            return 1;
        }

        #ifdef CURSOR_HIDING
        if (hideCursorFlag == 1)
        {
            //showCursor(hideCursorFlag);
            cliShowCursor();
            hideCursorFlag = 0;
        }
        #endif

        #else

        /*
        // Standard UART data receiving without ISR-echoing
        if (((UART.rcvBuf[UART.rcvIndex] = UART.charRcvd) == '\n' || ++UART.rcvIndex == SIZE - 2))
        {
            UCSR0B &= ~(1 << RXCIE0);       // disable Receive Interrupt Enable
            if(UART.rcvIndex > 1)
            {
                UART.rcvBuf[SIZE - 2] = '\n';   // close receive string
                UART.rcvBuf[SIZE - 1] = '\0';   // close receive string     
            }
        }
        */

        #endif //UART_ISR_CHARACTER_ECHOING
    }
    return 0;
}