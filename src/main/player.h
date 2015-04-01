#ifndef __PLAYER_H
#define __PLAYER_H

#include "datatypes.h"

void player_init();
void parsemsg(midiMsg *m);
void retrigger();

void note_on(unsigned char pitch, unsigned char velocity);
void note_off(unsigned char pitch);
void note_stop();

void control_change(unsigned char cmd, unsigned char value);
void pitch_bend(int value);

#endif
