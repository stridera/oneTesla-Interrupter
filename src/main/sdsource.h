#ifndef __SD_SOURCE
#define __SD_SOURCE

#include "data.h"

void sdsource_init();
void sdsource_run();
void sdsource_initcard();
unsigned char sdsource_filenext(bool loadfile);
void sdsource_playfile();
unsigned char check_and_get_filename();
unsigned int read_msg(unsigned char *track, unsigned int index, timedMidiMsg* msg);
void sdsource_loadfile();

#endif
