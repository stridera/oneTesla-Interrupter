#ifndef __SERIALSOURCE_H
#define __SERIALSOURCE_H

#define BAUDRATE 31250
#define BAUD_PRESCALER (((F_CPU / (BAUDRATE * 16UL))) - 1)

void USART_init();

void serialsource_init();
void serialsource_run();
#endif
