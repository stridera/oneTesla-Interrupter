#ifndef __SYSTEM_H

#include <Arduino.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
#include <avr/delay.h>

//datatypes go here; on Arduino,
//this is also defined in Arduino.h
#define byte unsigned char

//limits and constants go here
#define TIMER1_MAX BITS_16
#define TIMER2_MAX BITS_8
#define CPU_CLK 16 //MHz

//macros go here - use for
//performance-intensive routines
//or one-liners
#define ledOn() digitalWrite(2, HIGH)//PORTD |= (1 << 2)
#define ledOff() digitalWrite(2, LOW)//PORTD &= ~(1 << 2)
#define MIDI_MODE (PIND & (1 << 3))
#define setTimer1Compare(a) (OCR1A = (a))
#define setTimer2Compare(a) (OCR2A = (a))

#define setPrescaler1_8() TCCR1B = (1 << CS11) | (1 << WGM12)
#define setPrescaler1_64() TCCR1B = (1 << CS11) | (1 << CS10) | (1 << WGM12)
#define setPrescaler1_256() TCCR1B = (1 << CS12) | (1 << WGM12)
#define setPrescaler2_1() TCCR2B = (1 << CS20)
#define setPrescaler2_8() TCCR2B = (1 << CS21)
#define setPrescaler2_32() TCCR2B = (1 << CS21) | (1 << CS20)
#define setPrescaler2_64() TCCR2B = (1 << CS22)
#define setPrescaler2_128() TCCR2B = (1 << CS22) | (1 << CS20)
#define setPrescaler2_256() TCCR2B = (1 << CS22) | (1 << CS21)
#define setPrescaler2_1024() TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20)

#define engageISR1() TIMSK1 |= (1 << OCIE1A)
#define engageISR2() TIMSK2 |= (1 << OCIE2A)
#define disengageISR1() TIMSK1 &= ~(1 << OCIE1A)
#define disengageISR2() TIMSK2 &= ~(1 << OCIE2A)

#define clearCompareFlag1() TIFR1 &= ~(1 << OCF1A)
#define clearCompareFlag2() TIFR2 &= ~(1 << OCF2A)

//function prototypes - use for
//setup functions
void setupADC(void);
void setupTimers(void);
void setupPins(void);
byte readFreqPot();
byte readPWPot();

#define __SYSTEM_H
#endif
