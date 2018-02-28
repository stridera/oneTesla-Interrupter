#ifndef __UTIL_H
#define __UTIL_H
#include "datatypes.h"
#include <LiquidCrystal.h>

//note handlng
void note_kill(note *n);
void make_note(note *n, unsigned char pitch, unsigned char velocity);
//frequency utilities
unsigned int get_on_time(long freq);
unsigned long get_freq(unsigned char note);
//keypresses
unsigned char get_key();
//volume bar
void incvol(LiquidCrystal *lcd);
void decvol(LiquidCrystal *lcd);
//file utils
unsigned char chk_ext(char* fname);
//some debugging utilities
void print_hex_buf(LiquidCrystal *lcd, unsigned char* buf, unsigned char count);
void print_tmsg(LiquidCrystal *lcd, timedMidiMsg *m);
void print_byte(LiquidCrystal *lcd, unsigned char x);

#endif
