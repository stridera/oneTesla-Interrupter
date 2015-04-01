#ifndef __DATATYPES_H
#define __DATATYPES_H

#include <PFFS.h>

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
  FATFS* fs;
  DIR* dir;
  WORD read_count;
  timedMidiMsg *next;
  midiMsg *p_msg;
  
  unsigned char *buf;
  unsigned char *msgbuf;
  unsigned int buf_index;
  unsigned long len;
  unsigned long time;
} sdsource;
#endif
