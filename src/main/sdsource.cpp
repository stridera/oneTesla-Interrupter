#include <PFFS.h>
#include <stdlib.h>
#include "constants.h"
#include "data.h"
#include "datatypes.h"
#include "player.h"
#include "util.h"
#include "spi.h"
#include "lcd.h"

void sdsource_init() {
  spi_init();

  sd = (sdsource*) malloc(sizeof(sdsource));
  sd->fs = (FATFS*) malloc(sizeof(FATFS));
  sd->dir = (DIR*) malloc(sizeof(DIR));
  sd->buf = (unsigned char*) calloc(FILE_BUFFER_SIZE * sizeof(unsigned char), sizeof(unsigned char));
  sd->next = (timedMidiMsg*) malloc(sizeof(timedMidiMsg));
  sd->p_msg = (midiMsg*) malloc(sizeof(midiMsg));

  mmc_deselect();
  disk_attach_spi_functs(tx, rx, mmc_select, mmc_deselect);
  disk_initialize();
  pf_mount(sd->fs);
  pf_opendir(sd->dir, "/");
  sd->fs->flag &= ~FA_STREAM;
}

unsigned char sdsource_load(char* fname) {
  unsigned char err = pf_open(fname);
  memset(sd->buf, 0, FILE_BUFFER_SIZE);
  pf_read(sd->buf, FILE_BUFFER_SIZE, &(sd->read_count));
  sd->len = ((long) sd->buf[0] << 24) + ((long) sd->buf[1] << 16) + ((long) sd->buf[2] << 8) + (long) sd->buf[3];
  sd->buf_index = 4;
  sd->time = 0;
  sd->buf_index = read_msg(sd->buf, sd->buf_index, sd->next);
  return err;
}

void sdsource_run() {
  unsigned long lt = millis();
  unsigned long kt = 0;
  unsigned long ct = 0;
  unsigned long st = millis();
  for (;;) {
    unsigned long t = millis();
    unsigned char key = get_key();
    if (t - kt > 100) {
      if (key == btnSELECT) {
        note_stop();
        kt = t;
        return;
      } 
      else if (key == btnUP) {
        incvol(&lcd);
        kt = t;
      } 
      else if (key == btnDOWN) {
        decvol(&lcd);
        kt = t;
      }
    }

    if (t - ct > 1000) {
      unsigned int tsec = (int) ((t - st) / 1000);
      unsigned int lsec = (int) (sd->len / 1000);
      lcd_printat(2, 0, 13, tsec / 60);
      lcd_print(':');
      unsigned int tmp = tsec % 60;
      if (tmp < 10) lcd_print('0');
      lcd_print(tmp);
      lcd_print('/');
      lcd_print(lsec / 60);
      lcd_print(':');
      tmp = lsec % 60;
      if (tmp < 10) lcd_print('0');
      lcd_print(tmp);
      ct = t;
    }
    if (t - lt > sd->next->time) {
      sd->time += (t - lt);
      sd->p_msg->cmd = sd->next->cmd;
      sd->p_msg->db1 = sd->next->db1;
      sd->p_msg->db2 = sd->next->db2;
      parsemsg(sd->p_msg);
      sd->buf_index = read_msg(sd->buf, sd->buf_index, sd->next);
      if (sd->read_count == 0) return;
      lt = t;
    }
  }
}

