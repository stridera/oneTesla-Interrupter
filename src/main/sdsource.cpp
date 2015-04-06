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
char filename[256];

void sdsource_init() {
  sd = (sdsource*) malloc(sizeof(sdsource));
  sd->buf = (unsigned char*) calloc(FILE_BUFFER_SIZE * sizeof(unsigned char), sizeof(unsigned char));
  sd->next = (timedMidiMsg*) malloc(sizeof(timedMidiMsg));
  sd->p_msg = (midiMsg*) malloc(sizeof(midiMsg));
  sd->last_error[0] = '\0';
  sd->valid = 0;
  sd->file_count = 0;

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
      strcpy(sd->last_error, "Inv Partition");
      return;
    }
    if (fs.vwd()->isOpen()) {
      strcpy(sd->last_error, "Inv Root");
      return;
    }
    strcpy(sd->last_error, "Unknown error");
    return;    
  }

  lcd_printhome("Loading...");

  while(sdsource_filenext()) {  
    sd->file_count++;
  }
  file.close();
  fs.vwd()->rewind();
  
  if (sd->file_count == 0) {
    strcpy(sd->last_error, "No valid files");
  }
 
  sd->valid = 1;
}

unsigned char sdsource_filenext() {
  if (file.isOpen()) {
    file.close();
  }
  while (file.openNext(fs.vwd(), O_READ)) {
    file.getName(filename, sizeof(filename));
    if (check_and_get_filename()) {
      return 1;
    }
    file.close();
  }
  return 0;
}

unsigned char sdsource_fileprev() {
  // dir size is 32.
  uint16_t index = file.curPosition()/32;
  if (index < 2) return false;
  // position to possible previous file location.
  index -= 2;
  do {
    if (file.open(&file, index, O_READ)) {
      file.getName(filename, sizeof(filename));
      if (check_and_get_filename()) {
        return 1;
      }
      file.close();
    }    
  } while (index-- > 0);
  return 0;
}

unsigned char check_and_get_filename() {
  char *dot = strrchr(filename, '.');
  if (strcasecmp(dot, ".omd") != 0) return 0;
  *dot = '\0'; // Cut off the extension for nicer display
  return 1;
}

void sdsource_showfilename(int pos = 0)
{
  int fn_len = strlen(filename) - 16;
  int start = 0;
  if (fn_len < 0)
    fn_len = 0;
  else
    start = (pos % (fn_len + 6)) - 3;  
    
  // Pause for 3 beats at the beginning and end of the scroll
  if (start < 0)
    start = 0;
  else if (start >= fn_len)
    start = fn_len;
  
  lcd_printhome(&filename[start]);
  lcd_setcursor(0, 1);
  lcd_print("Size: ");
  lcd_print(file.fileSize());
  lcd_print("b");
}

void sdsource_run() {
  unsigned long kt = millis();
  int filepos = 0;
  
  sdsource_filenext();
  sdsource_showfilename(0);
  
  for (;;) {
    unsigned long t = millis();
    unsigned char key = get_key();
    
    if (t - kt > 600) {
      sdsource_showfilename(filepos++);
      kt = t;
    }      
    
    if (key == btnDOWN) {
      delay(150);
      if (!sdsource_filenext()) {
        file.close();
        fs.vwd()->rewind();
        sdsource_filenext();
      }
      filepos = 0; kt = t;
      delay(300);
    }
    
    if (key == btnUP) {
      delay(150);
      sdsource_fileprev();
      filepos = 0; kt = t;
      delay(300);
    }
  }
}

unsigned char sdsource_load(char* fname) {
  unsigned char err = 0;//pf_open(fname);
  memset(sd->buf, 0, FILE_BUFFER_SIZE);
 // pf_read(sd->buf, FILE_BUFFER_SIZE, &(sd->read_count));
  sd->len = ((long) sd->buf[0] << 24) + ((long) sd->buf[1] << 16) + ((long) sd->buf[2] << 8) + (long) sd->buf[3];
  sd->buf_index = 4;
  sd->time = 0;
  sd->buf_index = read_msg(sd->buf, sd->buf_index, sd->next);
  return err;
}

void sdsource_playfile() {
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
      //if (sd->read_count == 0) return;
      lt = t;
    }
  }
}

