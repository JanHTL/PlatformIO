/*
 * File:            sfr328p.h
 * Author:          Thomas Jerman
 * Date Created:    DD.MM.YYYY
 * Version: 1.0:    DD.MM.YYYY
 * Last Modified:   DD.MM.YYYY
 *
 * Description:
 * Providing special funciton register addresses
 *
 * License:
 * This code is released under Creative Commons Legal Code CC0 1.0 Universal
 *
 * Contact:
 * jr@htl-kaindorf.at
 */

#ifndef REG328P_H_INCLDUED
#define REG328P_H_INCLDUED

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
// GLOBAL DEFINES
/****************************************************/

/* Registers and associated bit numbers */

#define SFR_IN_EEPROM_OFFSET                0x300
#define EEPROM_ADDRESS_LIMIT                0x3FF

#define PINB_ADDR       ((volatile uint8_t*) 0x23)
#define PINB_ADDR       ((volatile uint8_t*) 0x23)
#define DDRB_ADDR       ((volatile uint8_t*) 0x24)
#define PORTB_ADDR      ((volatile uint8_t*) 0x25)
#define PINC_ADDR       ((volatile uint8_t*) 0x26)
#define DDRC_ADDR       ((volatile uint8_t*) 0x27)
#define PORTC_ADDR      ((volatile uint8_t*) 0x28)
#define PIND_ADDR       ((volatile uint8_t*) 0x29)
#define DDRD_ADDR       ((volatile uint8_t*) 0x2A)
#define PORTD_ADDR      ((volatile uint8_t*) 0x2B)

#define TIFR0_ADDR      ((volatile uint8_t*) 0x35)
#define TIFR1_ADDR      ((volatile uint8_t*) 0x36)
#define TIFR2_ADDR      ((volatile uint8_t*) 0x77)

#define PCIFR_ADDR      ((volatile uint8_t*) 0x3B)
#define EIFR_ADDR       ((volatile uint8_t*) 0x3C)
#define EIMSK_ADDR      ((volatile uint8_t*) 0x3D)
#define GPIOR0_ADDR     ((volatile uint8_t*) 0x3E)
#define EECR_ADDR       ((volatile uint8_t*) 0x3F)
#define EEDR_ADDR       ((volatile uint8_t*) 0x40)
#define EEAR_ADDR       ((volatile uint8_t*) 0x41)
#define EEARL_ADDR      ((volatile uint8_t*) 0x41)
#define EEARH_ADDR      ((volatile uint8_t*) 0x42)
#define GTCCR_ADDR      ((volatile uint8_t*) 0x43)
#define TCCR0A_ADDR     ((volatile uint8_t*) 0x44)
#define TCCR0B_ADDR     ((volatile uint8_t*) 0x45)
#define TCNT0_ADDR      ((volatile uint8_t*) 0x46)
#define OCR0A_ADDR      ((volatile uint8_t*) 0x47)
#define OCR0B_ADDR      ((volatile uint8_t*) 0x48)

#define GPIOR1_ADDR     ((volatile uint8_t*) 0x4A)
#define GPIOR2_ADDR     ((volatile uint8_t*) 0x4B)
#define SPCR_ADDR       ((volatile uint8_t*) 0x4C)
#define SPSR_ADDR       ((volatile uint8_t*) 0x4D)
#define SPDR_ADDR       ((volatile uint8_t*) 0x4E)

#define ACSR_ADDR       ((volatile uint8_t*) 0x50)

#define SMCR_ADDR       ((volatile uint8_t*) 0x53)
#define MCUSR_ADDR      ((volatile uint8_t*) 0x54)
#define MCUCR_ADDR      ((volatile uint8_t*) 0x55)

#define SPMCSR_ADDR     ((volatile uint8_t*) 0x57)

#define SPL_ADDR        ((volatile uint8_t*) 0x5D)
#define SPLL_ADDR       ((volatile uint8_t*) 0x5D)
#define SPLH_ADDR       ((volatile uint8_t*) 0x5E)

#define WDTCSR_ADDR     ((volatile uint8_t*) 0x60)
#define CLKPR_ADDR      ((volatile uint8_t*) 0x61)
#define PRR_ADDR        ((volatile uint8_t*) 0x64)
#define OSCCAL_ADDR     ((volatile uint8_t*) 0x66)
#define PCICR_ADDR      ((volatile uint8_t*) 0x68)
#define EICRA_ADDR      ((volatile uint8_t*) 0x69)
#define PCMSK0_ADDR     ((volatile uint8_t*) 0x6B)
#define PCMSK1_ADDR     ((volatile uint8_t*) 0x6C)
#define PCMSK2_ADDR     ((volatile uint8_t*) 0x6D)
#define TIMSK0_ADDR     ((volatile uint8_t*) 0x6E)
#define TIMSK1_ADDR     ((volatile uint8_t*) 0x6F)
#define TIMSK2_ADDR     ((volatile uint8_t*) 0x70)

#define ADC_ADDR        ((volatile uint8_t*) 0x78)
#define ADCL_ADDR       ((volatile uint8_t*) 0x78)
#define ADCH_ADDR       ((volatile uint8_t*) 0x79)
#define ADCSRA_ADDR     ((volatile uint8_t*) 0x7A)
#define ADCSRB_ADDR     ((volatile uint8_t*) 0x7B)
#define ADMUX_ADDR      ((volatile uint8_t*) 0x7C)
#define DIDR0_ADDR      ((volatile uint8_t*) 0x7E)
#define DIDR1_ADDR      ((volatile uint8_t*) 0x7F)
#define TCCR1A_ADDR     ((volatile uint8_t*) 0x80)
#define TCCR1B_ADDR     ((volatile uint8_t*) 0x81)
#define TCCR1C_ADDR     ((volatile uint8_t*) 0x82)
#define TCNT1_ADDR      ((volatile uint8_t*) 0x84)
#define TCNT1L_ADDR     ((volatile uint8_t*) 0x84)
#define TCNT1H_ADDR     ((volatile uint8_t*) 0x85)
#define ICR1_ADDR       ((volatile uint8_t*) 0x86)
#define ICR1L_ADDR      ((volatile uint8_t*) 0x86)
#define ICR1H_ADDR      ((volatile uint8_t*) 0x87)
#define OCR1A_ADDR      ((volatile uint8_t*) 0x88)
#define OCR1AL_ADDR     ((volatile uint8_t*) 0x88)
#define OCR1AH_ADDR     ((volatile uint8_t*) 0x89)
#define OCR1B_ADDR      ((volatile uint8_t*) 0x8A)
#define OCR1BL_ADDR     ((volatile uint8_t*) 0x8A)
#define OCR1BH_ADDR     ((volatile uint8_t*) 0x8B)
#define TCCR2A_ADDR     ((volatile uint8_t*) 0xB0)
#define TCCR2B_ADDR     ((volatile uint8_t*) 0xB1)
#define TCNT2_ADDR      ((volatile uint8_t*) 0xB2)
#define OCR2A_ADDR      ((volatile uint8_t*) 0xB3)
#define OCR2B_ADDR      ((volatile uint8_t*) 0xB4)
#define ASSR_ADDR       ((volatile uint8_t*) 0xB6)
#define TWBR_ADDR       ((volatile uint8_t*) 0xB8)
#define TWSR_ADDR       ((volatile uint8_t*) 0xB9)
#define TWAR_ADDR       ((volatile uint8_t*) 0xBA)
#define TWDR_ADDR       ((volatile uint8_t*) 0xBB)
#define TWCR_ADDR       ((volatile uint8_t*) 0xBC)
#define TWAMR_ADDR      ((volatile uint8_t*) 0xBD)
#define UCSR0A_ADDR     ((volatile uint8_t*) 0xC0)
#define UCSR0B_ADDR     ((volatile uint8_t*) 0xC1)
#define UCSR0C_ADDR     ((volatile uint8_t*) 0xC2)
#define UBRR0_ADDR      ((volatile uint8_t*) 0xC4)
#define UBRR0L_ADDR     ((volatile uint8_t*) 0xC4)
#define UBRR0H_ADDR     ((volatile uint8_t*) 0xC5)
#define UDR0_ADDR       ((volatile uint8_t*) 0xC6)

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

#ifdef __cplusplus
}
#endif

#endif