/*
 * Backend timers
 * These functions do all the work
 * That produces sparks
 */
 
#ifndef __TIMERS_H

#include "datatypes.h"

void startTimer1();
void startTimer2();
void setTimer1f(unsigned int fr);
void ISR_backend(note* n);

#define __TIMERS_H
#endif
