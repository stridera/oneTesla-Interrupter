/*
 * ATMega328 SD card interrupter
 * Copyright 2015 oneTesla, LLC
 * See README.txt for licensing information
 */
 
#include "constants.h"
#include "data.h"
#include "datatypes.h"
#include "sdsource.h"
#include "serialsource.h"
#include "player.h"
#include "system.h"
#include "util.h"
#include "timers.h"
#include "lcd.h"

#include <LiquidCrystal.h>
#include <PFFS.h>
#include <SPI.h>

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

note *note1, *note2;
midiMsg *last_message;
serialsource *serial;
sdsource *sd;
FILINFO f;
char volindex, menuindex = 0;
int ffreq = 20;

void fixedLoop();

void setup() {
 setupTimers();
 setupPins();
 player_init();
 sdsource_init();
 serialsource_init();
 lcd_init();
 unsigned char err = pf_readdir(sd->dir, &f);
 pf_readdir(sd->dir, (FILINFO*) (0));
 if (err) {
   lcd_printhome("No card found");
 } else {
   lcd_printhome("Fixed Mode");
   unsigned char n = 20;
 }
}

void loop() {
  unsigned char key = get_key();
  if (key == btnDOWN) {
    if (menuindex == 0) {
      lcd_printhome("Live Mode");
      menuindex++;
    } else {
      menuindex = 2;
      pf_readdir(sd->dir, &f);
      if (f.fname[0] == (char) (0)) {
        pf_readdir(sd->dir, (FILINFO*) (0));
        lcd_printhome("Fixed Mode");
        menuindex = 0;
      } else {
        lcd_printhome(f.fname);
        lcd_printat(0, 1, f.fsize);
      }
    }
    delay(180);
  }
  if (key == btnSELECT) {
    if (menuindex == 0) {
      delay(150);
      fixedLoop();
      delay(300);
    } else if (menuindex == 1) {
      delay(150);
      lcd_printhome("Live Mode ON");
      lcd_setcursor(0, 1);
      for (int i = 0; i < volindex; i++) {
        lcd_print((char) (1));
      }
      serialsource_run();
      lcd_printhome("Live Mode");
      delay(300);
    } else if (!chk_ext(f.fname)) {
      lcd_printhome("Invalid file");
    } else {
      sdsource_load(f.fname);
      lcd_printhome("\2 ");
      lcd_setcursor(0, 1);
      for (int i = 0; i < volindex; i++) {
        lcd_print((char) (1));
      }
      delay(300);
      sdsource_run();
      lcd_printhome(f.fname);
      lcd_printat(0, 1, f.fsize);
      delay(300);
    }
  }
}

void fixedLoop() {
  lcd_printhome("[Freq: ");
  lcd_print(ffreq);
  lcd_print("Hz");
  lcd_printat(15, 0, ']');
  lcd_setcursor(0, 1);
  for (int i = 0; i < volindex; i++) {
    lcd_print((char) (1));
  }
  char mode = 1;
  unsigned long elapsed = 0;
  unsigned int uinc, dinc = 1;
  note1->velocity = 127;
  note1->on_time = get_on_time(ffreq);
  setTimer1f(ffreq);
  engageISR1();
  for (;;) {
    unsigned char key = get_key();
    if (elapsed > 100000) {
      elapsed = 0;
      if (key != btnUP) uinc = 1;
      if (key != btnDOWN) dinc = 1;
      if (key == btnSELECT) {
        mode = !mode;
        if (mode) {
          lcd_printhome("[Freq: ");
          lcd_print(ffreq);
          lcd_print("Hz");
          lcd_printat(15, 0, ']');
          lcd_setcursor(0, 1);
          for (int i = 0; i < volindex; i++) {
            lcd_print((char) (1));
          }
        } else {
          lcd_printhome("Freq: ");
          lcd_print(ffreq);
          lcd_print("Hz");
          lcd_setcursor(0, 1);
          for (int i = 0; i < volindex; i++) {
            lcd.print((char) (1));
          }
          lcd_printat(0, 1, '[');
          lcd_printat(15, 1, ']');
        }
      }
      if (key == btnUP) {
        if (!mode) {
          lcd_setcursor(0, 1);
          incvol(&lcd);
          lcd_printat(0, 1, '[');
          lcd_printat(15, 1, ']');
        } else {
          ffreq += uinc;
          uinc += 5;
          if (ffreq > 1000) ffreq = 1000;
          lcd_printat(7, 0, 7, (unsigned int) ffreq);
          lcd_print("Hz");
          note1->on_time = get_on_time(ffreq);
          setTimer1f(ffreq);
        }
      }
      if (key == btnDOWN) {
        if (!mode) {
          lcd_setcursor(0, 1);
          decvol(&lcd);
          lcd_printat(0, 1, '[');
          lcd_printat(15, 1, ']');
        } else {
          ffreq -= dinc;
          dinc += 5;
          if (ffreq < 1) ffreq = 1;
          lcd_printat(7, 0, 7, (unsigned int) ffreq);
          lcd_print("Hz");
          note1->on_time = get_on_time(ffreq);
          setTimer1f(ffreq);;
        }
      }
    }
    delayMicroseconds(1000);
    elapsed += 1000;
  }
}

int main() {
  init();
  setup();
  for(;;) {
    loop();
  }
  return 0;
}

