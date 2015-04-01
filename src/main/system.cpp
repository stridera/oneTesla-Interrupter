/*
 * System dependent functions
 * These abstract away assorted registers
 * and ATMega-specific functions
 * Edit these first when attempting to port the code
 */
 
#include "system.h"
#include "constants.h"

void setupADC() {
  ADMUX =
    (1 << REFS0) |  // Use AVCC reference (connected to 5V)
    (1 << ADLAR) |  // Left-adjust the result, we don't need full-precision
    (1 << MUX0)  ;  // Select ADC1 (on-time pot)
                      
  ADCSRA =
    (1 << ADEN)  |  // Enable the ADC
    (1 << ADSC)  |  // Start free-running conversion
    (1 << ADATE) |  // Enable auto-triggering
    (1 << ADPS0) |  // Set prescaler to 128 for 16Mhz/128 = 125kHz SAR clock
    (1 << ADPS1) |
    (1 << ADPS2) ;
      
  ADCSRB = 0x00;    // Ensure that the auto-triggering source is in free-running mode
}

void setupTimers() {
  // Set up 16-bit Timer1
  TCCR1A = 0x00;          // CTC mode, hardware pins disconnected
  TCCR1B = (1 << WGM12);  // CTC mode, don't set prescaler yet
  TIMSK1 = 0x00;
  
  // Set up 16-bit Timer2
  TCCR2A = (1 << WGM21);   // CTC mode, hardware pins disconnected, don't set prescaler yet
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
}

void setupPins() {
  // Setup optical transmitter output pin
  DDRD  |=  (1 << 2);  
  
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DN, INPUT);
  pinMode(BTN_OK, INPUT);
}

byte readFreqPot() {
  ADMUX &= ~(1 << MUX0);  // Select the frequency pot (ADC0)
  _delay_us(300);
  return ADCH;
}

byte readPWPot() {
  ADMUX |= (1 << MUX0);  // Select the on-time pot (ADC1)
  _delay_us(300);
  return ADCH;
}
