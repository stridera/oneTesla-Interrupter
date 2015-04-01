#ifndef __PFFS_H
#define __PFFS_H

extern "C" {
	#include "diskio.h"
	#include "pff.h"
	#include "integer.h"
}

void PFFS_attach_spi_functs(void (*tx)(unsigned char), unsigned char (*rx)(void), void (*sel)(void), void(*desel)(void));
void PFFS_initialize();

#endif
