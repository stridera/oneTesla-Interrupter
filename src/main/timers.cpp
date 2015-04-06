#include "timers.h"
#include "constants.h"
#include "data.h"
#include "system.h"
#include "datatypes.h"
#include "lcd.h"
 
void startTimer1() {
  long ticks = CPU_CLK * note1->period / 8;
  
  if (ticks <= TIMER1_MAX) setPrescaler1_8();
  else if ((ticks /= 8) <= TIMER1_MAX) setPrescaler1_64();
  else if ((ticks /= 4) <= TIMER1_MAX) setPrescaler1_256();
  ticks--;
   
  setTimer1Compare(ticks);
  
  if (note1->on_time != 0) engageISR1();
}

void startTimer2() {
  long ticks = CPU_CLK * note2->period;
  
  if (ticks <= TIMER2_MAX) setPrescaler2_1();
  else if ((ticks /= 8) <= TIMER2_MAX) setPrescaler2_8();
  else if ((ticks /= 4) <= TIMER2_MAX) setPrescaler2_32();
  else if ((ticks /= 2) <= TIMER2_MAX) setPrescaler2_64();
  else if ((ticks /= 2) <= TIMER2_MAX) setPrescaler2_128();
  else if ((ticks /= 2) <= TIMER2_MAX) setPrescaler2_256();
  else if ((ticks /= 4) <= TIMER2_MAX) setPrescaler2_1024();
  
  ticks--;
  
  setTimer2Compare(ticks);
  if (note2->on_time != 0) engageISR2();
}

void setTimer1f(unsigned int fr) {
  unsigned long period = 1000000UL / fr;
  long ticks = CPU_CLK * period / 8;
  
  if (ticks <= TIMER1_MAX) setPrescaler1_8();
  else if ((ticks /= 8) <= TIMER1_MAX) setPrescaler1_64();
  else if ((ticks /= 4) <= TIMER1_MAX) setPrescaler1_256();
  
  ticks--;
   
  setTimer1Compare(ticks);
}


ISR (TIMER1_COMPA_vect, ISR_BLOCK) {
  ISR_backend(note1);
  clearCompareFlag2();
}

ISR(TIMER2_COMPA_vect, ISR_BLOCK) {
  ISR_backend(note2);
  clearCompareFlag1();
}

void ISR_backend(note* n) {
  long val = (volindex << 4);
  long tmp = val * (long) (n->on_time);
  int t = (int) (tmp >> 8);
  tmp = (long) (t) * (long) (n->velocity);
  t = (int) (tmp >> 8);
  if (t < MIN_ON_TIME) t = MIN_ON_TIME;
  ledOn();
  delayMicroseconds(t);
  ledOff();
  delayMicroseconds(t);
}
