#include "PFFS.h"

void PFFS_attach_spi_functs(void (*tx)(unsigned char), unsigned char (*rx)(void), void (*sel)(void), void (*desel)(void)) {
	disk_attach_spi_functs(tx, rx, sel, desel);
}

void PFFS_initialize() {
	disk_initialize();
}
