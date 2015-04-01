#include <avr/sfr_defs.h>
#include "constants.h"

byte rx()
{
  SPDR = 0xFF;
  loop_until_bit_is_set(SPSR, SPIF);
  return SPDR;
}

void tx(unsigned char d)
{
  SPDR = d;
  loop_until_bit_is_set(SPSR, SPIF);
}

void spi_init()
{
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  
  SPCR = _BV(MSTR) | _BV(SPE);  // Master mode, SPI enable, clock speed MCU_XTAL/4
}


void mmc_select() {
  digitalWrite(MMC_CS, LOW);
}

void mmc_deselect() {
  digitalWrite(MMC_CS, HIGH);
}
