#include "datatypes.h"
#include "data.h"
#include "util.h"
#include "constants.h"
#include "lcd.h"
#include <Arduino.h>
#include <SdFat.h>

void note_kill(note *n) {
  n->pitch = 0;
}

void make_note(note *n, unsigned char pitch, unsigned char velocity) {
  n->pitch = pitch;
  unsigned long freq = get_freq(pitch);
  n->period = 1000000 / freq;
  n->on_time = get_on_time(freq);
  n->velocity = map_velocity(velocity);
}

unsigned long get_freq(unsigned char pitch) {
  int n = pitch - 57;
  if (n >= 0) {
    return pow2[n / 12] * base[n % 12];
  } else {
    int a = (-(n+ 1)) / 12 + 1;
    int r = 12 * a + n;
    return base[r] / pow2[a];
  }
}

unsigned int get_on_time(long freq) {
  int index = freq / 100;
  if (index > ON_TIME_ARRAY_LENGTH - 1) {return 0;}
  else {return LOOKUP_TABLE_SCALE * on_times[index];}
}

unsigned char get_key()
{
#ifndef DEBUG
 if (!digitalRead(BTN_UP)) return btnUP;
 if (!digitalRead(BTN_DN)) return btnDOWN;
 if (!digitalRead(BTN_OK)) return btnSELECT;
 return btnNONE;
#else
 int adc_key_in = analogRead(0);
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT; 
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;  
 return btnNONE;
#endif
}

void incvol(LiquidCrystal *lcd) {
  if (volindex == 16) return;
  lcd_printat(volindex, 1, (char) (1));
  volindex++;
}

void decvol(LiquidCrystal *lcd) {
  if (volindex == 0) return;
  lcd_printat(volindex - 1, 1, ' ');
  volindex--;
}

void print_hex_buf(LiquidCrystal *lcd, unsigned char* buf, unsigned char count) {
  for (char i = 0; i < count; i++) {
    char h = (buf[i] & 0xf0) >> 4;
    char l = buf[i] & 0x0f;
    char ht = chex[h];
    char lt = chex[l];
    lcd->print(ht);
    lcd->print(lt);
    lcd->print(' ');
  }
}

void print_byte(LiquidCrystal *lcd, unsigned char x) {
  char h = (x & 0xf0) >> 4;
  char l = x & 0x0f;
  char ht = chex[h];
  char lt = chex[l];
  lcd->print(ht);
  lcd->print(lt);
  lcd->print(' ');
}  

void print_tmsg(LiquidCrystal *lcd, timedMidiMsg *m) {
  lcd->print((int) (m->time));
  lcd->print(" ");
  if (m->cmd == 0x80) lcd->print("Off ");
  if (m->cmd == 0x90) lcd->print("On ");
  print_byte(lcd, m->db1);
  print_byte(lcd, m->db2);
}
