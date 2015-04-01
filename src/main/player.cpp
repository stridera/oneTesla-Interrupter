#include "player.h"
#include "util.h"
#include "data.h"
#include "system.h"
#include "timers.h"
#include "constants.h"
#include "lcd.h"

#include <stdlib.h>

void note_on(unsigned char pitch, unsigned char value) {
  if (value == 0) {
    note_off(pitch);
    return;
  }
  
  if (note1->pitch == pitch) {
    make_note(note1, pitch, value);
    retrigger();
    return;
  } else if (note2->pitch == pitch) {
    make_note(note2, pitch, value);
    retrigger();
    return;
  }
  
  if (note1->pitch == 0) {
    make_note(note1, pitch, value);
    retrigger();
    return;
  } else if (note2->pitch == 0) {
    make_note(note2, pitch, value);
    retrigger();
    return;
  } else {
    memcpy(note1, note2, sizeof(note));
    make_note(note2, pitch, value);
    retrigger();
    return;
  }
}

void note_off(unsigned char pitch) {
  if (note1->pitch == pitch) {
    note1->pitch = 0;
    retrigger();
    return;
  } else if (note2->pitch == pitch) {
    note2->pitch = 0;
    retrigger();
    return;
  }
}

void note_stop() {
  note1->pitch = 0;
  note2->pitch = 0;
  retrigger();
}

void control_change(unsigned char cmd, unsigned char value) {
  if (cmd == 0x78 || cmd == 0x79 || cmd == 0x7B || cmd == 0x7C) {
    note_stop();
  }
}

void retrigger() {
  if (note1->pitch > 0) {
    startTimer1();
  } else {
    disengageISR1();
  }
  if (note2->pitch > 0 && note2->period < TIMER_2_PERIOD_MAX) {
    startTimer2();
  } else {
    disengageISR2();
  }
}

void parsemsg(midiMsg *m) {
  unsigned char channel = m->cmd & 0x0f;
  if (channel != 1 && channel != 0) return;
  if (m->cmd == 0xfc) {
    note_stop();
    return;
  }
  unsigned char cmd = (m->cmd & 0xf0) >> 4;
  if (cmd == 0x09) {
    note_on(m->db1, m->db2);
  } else if (cmd == 0x08) {
    note_off(m->db1);
  } else if (cmd == 0x0b) {
    control_change(m->db1, m->db2);
  }
}

void player_init() {
  note1 = (note*) malloc(sizeof(note));
  note2 = (note*) malloc(sizeof(note));
  note1->pitch = 0;
  note2->pitch = 0;
}
