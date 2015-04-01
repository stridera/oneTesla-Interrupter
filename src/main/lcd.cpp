#include <LiquidCrystal.h>
#include "data.h"
#include "constants.h"
#include "lcd.h"

void lcd_init() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1, block);
  lcd.createChar(2, arrow);
}

void lcd_clear() {
  lcd.clear();
}

void lcd_setcursor(char row, char col) {
  lcd.setCursor(row, col);
}

void lcd_print(char* s) {
  lcd.print(s);
}

void lcd_print(char c) {
  lcd.print(c);
}

void lcd_print(int n) {
  lcd.print(n);
}

void lcd_print(long n) {
  lcd.print(n);
}

void lcd_print(unsigned char c) {
  lcd.print(c);
}

void lcd_print(unsigned int n) {
  lcd.print(n);
}

void lcd_print(unsigned long n) {
  lcd.print(n);
}

void lcd_printhome(char* s) {
  lcd_clear();
  lcd_setcursor(0, 0);
  lcd_print(s);
}

void lcd_printat(char col, char row, char* s) {
  lcd_setcursor(col, row);
  lcd_print(s);
}

void lcd_printat(char col, char row, char c) {
  lcd_setcursor(col, row);
  lcd_print(c);
}

void lcd_printat(char col, char row, unsigned int n) {
  lcd_setcursor(col, row);
  lcd_print(n);
}

void lcd_printat(char col, char row, unsigned long n) {
  lcd_setcursor(col, row);
  lcd_print(n);
}

void lcd_printat(char col, char row, char clr, char* s) {
  lcd_clrat(col, row, clr);
  lcd_setcursor(col, row);
  lcd_print(s);
}

void lcd_printat(char col, char row, char clr, char c) {
  lcd_clrat(col, row, clr);
  lcd_setcursor(col, row);
  lcd_print(c);
}

void lcd_printat(char col, char row, char clr, unsigned int n) {
  lcd_clrat(col, row, clr);
  lcd_setcursor(col, row);
  lcd_print(n);
}

void lcd_printat(char col, char row, char clr, unsigned long n) {
  lcd_clrat(col, row, clr);
  lcd_setcursor(col, row);
  lcd_print(n);
}

void lcd_clrat(char col, char row, char clr) {
  lcd_setcursor(col, row);
  for (int i = 0; i < clr; i++) {
   lcd_print(' ');
  }
} 
