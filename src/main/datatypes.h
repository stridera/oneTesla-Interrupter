#ifndef __DATATYPES_H
#define __DATATYPES_H

#include <SdFat.h>

typedef struct {
  unsigned char pitch;
  unsigned char velocity;
  unsigned long period;
  unsigned long on_time;
} note;

typedef struct {
  unsigned char cmd;
  unsigned char db1;
  unsigned char db2;
} midiMsg;

typedef struct {
  unsigned int time;
  unsigned char cmd;
  unsigned char db1;
  unsigned char db2;
} timedMidiMsg;

typedef struct {
  unsigned char last_status;
  unsigned char remaining;
  unsigned char total;
  unsigned char *buf;
  unsigned char buf_idx;
} serialsource;

typedef struct {
  int read_count;
  timedMidiMsg *next;
  midiMsg *p_msg;

  char last_error[17];  
  unsigned char valid;
  unsigned int file_count, dir_count;
  unsigned char *buf;
  unsigned char *msgbuf;
  unsigned int buf_index;
  unsigned long len;
  unsigned long time;
} sdsource;
#endif
