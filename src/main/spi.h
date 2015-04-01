#ifndef __SPI_H
#define __SPI_H

byte rx();
void tx(unsigned char d);
void spi_init();
void mmc_select();
void mmc_deselect();


#endif
