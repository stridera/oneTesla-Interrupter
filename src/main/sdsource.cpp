#include <SdFat.h>
#include <stdlib.h>
#include "constants.h"
#include "sdsource.h"
#include "data.h"
#include "datatypes.h"
#include "player.h"
#include "util.h"
#include "lcd.h"

SdFat fs;
SdFile file;
char filename[100];

void sdsource_init() {
  sd = (sdsource*) malloc(sizeof(sdsource));
  sd->buf = (unsigned char*) calloc(FILE_BUFFER_SIZE * sizeof(char), sizeof(char));
  sd->next = (timedMidiMsg*) malloc(sizeof(timedMidiMsg));
  sd->p_msg = (midiMsg*) malloc(sizeof(midiMsg));
  sd->last_error[0] = '\0';
  sd->valid = 0;
  sd->file_count = 0;
  sd->dir_count = 0;

  sdsource_initcard();
}

void sdsource_initcard() {
  lcd_printhome("Checking SD Card");
  if (!fs.begin(MMC_CS, SPI_FULL_SPEED)) {
    if (fs.card()->errorCode()) { 
      // Can use sd.card()->errorData() to find out more information.
      // errorCode of 0x01; errorData of 0xFF is wiring error or no card.
      strcpy(sd->last_error, "No card found");
      return;
    }
    if (fs.vol()->fatType() == 0) {
      strcpy(sd->last_error, "Bad Partition");
      return;
    }
    if (fs.vwd()->isOpen()) {
      strcpy(sd->last_error, "Bad Root");
      return;
    }
    strcpy(sd->last_error, "Unknown error");
    return;    
  }

  for (;;) {
    int flag = sdsource_filenext(true);
    if (flag == 0) break;
    if (flag == 1) sd->file_count++;
    if (flag == 2) sd->dir_count++;
  }
  file.close();
  fs.vwd()->rewind();
  strcpy(filename, "");
  
  if (sd->file_count == 0) {
    strcpy(sd->last_error, "No valid files");
  }
  
  sd->valid = 1;
}

unsigned char sdsource_filenext(bool loadfile = false) {
  if (file.isOpen()) {
    file.close();
  }
  while (file.openNext(fs.vwd(), O_READ)) {
    file.getName(filename, sizeof(filename));
    int flag = check_and_get_filename();
    if (flag != 0) {
        if (loadfile && !file.isDir()) sdsource_loadfile();
        return flag;
    }
    file.close();
  }
  return 0;
}

unsigned char sdsource_fileprev(bool loadfile = false) {
  int currentIndex = file.dirIndex();
  int lastIndex = -1;
  int hasprev = 0;
  if (file.isOpen()) file.close();
  fs.vwd()->rewind();
  strcpy(filename, "");
  while (file.openNext(fs.vwd(), O_READ)) {
    if (file.dirIndex() == currentIndex) {
      if (lastIndex >= 0) {
        file.close();
        file.open(fs.vwd(), lastIndex, O_READ);
        file.getName(filename, sizeof(filename));
        if (check_and_get_filename() != 0) {
          if (loadfile && !file.isDir()) sdsource_loadfile();
        }
        hasprev = 1;
      } else {
        hasprev = 0;
      }
      break;
    }
    file.getName(filename, sizeof(filename));
    if (check_and_get_filename() != 0) {
      lastIndex = file.dirIndex();
    }
    file.close();
  }
  if (!hasprev) {
    file.open(fs.vwd(), currentIndex, O_READ);
    file.getName(filename, sizeof(filename));
    check_and_get_filename();
  }
  return hasprev;
}

unsigned char check_and_get_filename() {
  if (file.isHidden() || file.isSystem()) return 0;
  if (file.isDir()) {
    int i;
    for (i = 0; i < sizeof(filename); i++) {
      if (filename[i] == 0) break;
    }
    filename[i] = '/';
    filename[i + 1] = 0;
    return 2;
  }
  char *dot = strrchr(filename, '.');
  if (strcasecmp(dot, ".omd") != 0) return 0;
  *dot = '\0'; // Cut off the extension for nicer display
  return 1;
}

void sdsource_showfilename(int pos = 0) {
  int fn_len = strlen(filename) - 16;
  int start = 0;
  if (fn_len < 0){
    fn_len = 0;
  } else {
    start = (pos % (fn_len + 6)) - 3;
  }
    
  // Pause for 3 beats at the beginning and end of the scroll
  if (start < 0) {
    start = 0;
  } else if (start >= fn_len) {
    start = fn_len;
  }
  
  lcd_printhome(&filename[start]);
  lcd_setcursor(0, 1);
  if (!file.isDir()) {
    unsigned int lsec = (int) (sd->len / 1000);
    lcd_print(lsec / 60);
    lcd_print(':');
    unsigned int tmp = lsec % 60;
    if (tmp < 10) lcd_print('0');
    lcd_print(tmp);
  } else {
    lcd_print("Folder");
  }
}

void sdsource_loadfile() {
  memset(sd->buf, 0, FILE_BUFFER_SIZE);
  sd->read_count = file.read(sd->buf, FILE_BUFFER_SIZE);
  sd->len = ((long) sd->buf[0] << 24) + ((long) sd->buf[1] << 16) + ((long) sd->buf[2] << 8) + (long) sd->buf[3];
  sd->buf_index = 4;
  sd->time = 0;
  sd->buf_index = read_msg(sd->buf, sd->buf_index, sd->next);
}

void sdsource_run() {
  unsigned long kt = millis();
  int filepos = 0;
  
  sdsource_filenext(true);
  sdsource_showfilename(0);
  
  for (;;) {
    unsigned long t = millis();
    unsigned char key = get_key();
    
    if (t - kt > 600) {
      sdsource_showfilename(filepos++);
      kt = t;
    }      
    
    if (key == btnSELECT) {
      delay(150);
      if (file.isDir()) {
        if (fs.vwd()->isRoot()) {
          fs.chdir(filename);
          sdsource_filenext(true);
          if (file.isOpen()) {
            filepos = 0; kt = t;
            sdsource_showfilename(filepos);
          } else {
            fs.vwd()->getName(filename, sizeof(filename));
            int vwdindex = fs.vwd()->dirIndex();
            fs.chdir("/");
            file.close();
            file.open(fs.vwd(), vwdindex, O_READ);
            file.getName(filename, sizeof(filename));
            check_and_get_filename();
          }
        } else {
          lcd_clear();
          lcd_printhome("Can't switch!");
          delay(500);
        }
      } else {
        sdsource_playfile();
      }
      delay(300);
    }
    
    if (key == btnDOWN) {
      delay(150);
      if (sdsource_filenext(true) == 0) {
        file.close();
        fs.vwd()->rewind();
        sdsource_filenext(true);
      }
      filepos = 0; kt = t;
      sdsource_showfilename(filepos);
    }
    
    if (key == btnUP) {
      int tt = millis();
      int hasprev = sdsource_fileprev(true);
      if (!hasprev && !fs.vwd()->isRoot()) {
        fs.vwd()->getName(filename, sizeof(filename));
        int vwdindex = fs.vwd()->dirIndex();
        fs.chdir("/");
        file.close();
        file.open(fs.vwd(), vwdindex, O_READ);
        file.getName(filename, sizeof(filename));
        check_and_get_filename();
      }
      int dt = millis() - tt;
      int dd = 150 - dt;
      if (dd < 0) dd = 0;
      delay(dd);
      filepos = 0; kt = t;
      sdsource_showfilename(filepos);
    }
  }
}

unsigned int read_msg(unsigned char *track, unsigned int index, timedMidiMsg* msg) {
  if (index >= sd->read_count) {
    sd->read_count = file.read(sd->buf, FILE_BUFFER_SIZE);
    if (sd->read_count == 0) return 0;
    index = 0;
  }
  msg->time = ((((unsigned int) track[index]) << 7) + (track[index + 1] & 0x7f));
  msg->cmd = ((track[index + 1] & 0x80) >> 7) ? 0x91 : 0x81;
  msg->db1 = track[index + 2];
  msg->db2 = track[index + 3];
  return index + 4;
}

void sdsource_playfile() {
  lcd_printhome("\2 ");
  for (int i = 0; i < volindex; i++) {
    lcd_printat(i, 1, (char) (1));
  }
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
        file.rewind();
        sd->buf_index = sd->read_count = 0;
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
      if (sd->read_count == 0) {
        note_stop();
        file.rewind();
        return;
      }
      lt = t;
    }
  }
}

