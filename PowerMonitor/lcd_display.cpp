#include "lcd_display.h"
#include <LiquidCrystal_I2C.h>
#include <stdarg.h>
#include <ESP8266WiFi.h> 

LiquidCrystal_I2C lcd(0x27,20,4);

volatile bool screenToggle = false;
volatile bool LCD_LED = false;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 500;

byte rightArrow[8] = {0b00000,0b00100,0b00010,0b11111,0b00010,0b00100,0b00000,0b00000};
byte clockSymbol[8] = {0b00000,0b01110,0b10101,0b10111,0b10001,0b01110,0b00000,0b00000};
byte smilesymbol[8] = {0b00000,0b01010,0b01010,0b00000,0b00000,0b10001,0b01110,0b00000};
byte lightningSymbol[8] = {0b00100,0b01100,0b11110,0b00110,0b01100,0b11100,0b01000,0b00000};

void lcd_init() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, rightArrow);
  lcd.createChar(2, clockSymbol);
  lcd.createChar(3, smilesymbol);
  lcd.createChar(4, lightningSymbol);
}

void lcd_clear() { lcd.clear(); }
void lcd_backlight(bool on) { if (on) lcd.backlight(); else lcd.noBacklight(); }
void lcd_print(const char* s) { lcd.print(s); }
void lcd_set_cursor(uint8_t c, uint8_t r) { lcd.setCursor(c,r); }
void lcd_write_custom(uint8_t id) { lcd.write(byte(id)); }

void lcd_printf(const char* fmt, ...) {
  char buf[64];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  lcd.print(buf);
}

// Interrupt handlers
void IRAM_ATTR onScreenButton() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    screenToggle = true;
    lastInterruptTime = currentTime;
  }
}

void IRAM_ATTR onBacklightButton() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    LCD_LED = !LCD_LED;
    if (LCD_LED) lcd.backlight(); else lcd.noBacklight();
    lastInterruptTime = currentTime;
  }
}

void show_screen(int screen) {
  // placeholder: actual screen drawing is handled in modules
  switch (screen) {
    case 0: // time screen drawn by timekeeper
      break;
    case 1: // energy drawn by pzem module
      break;
    case 2: // sensor drawn by dht module
      break;
    case 3: // device info
      device_info_display();
      break;
  }
}

void device_info_display() {
  lcd.setCursor(4,0);
  lcd.print("Device INFO");
  lcd.setCursor(0,1);
  lcd.print("IP:");
  lcd.setCursor(3,1);
  lcd.print(WiFi.localIP().toString());
  lcd.setCursor(0,2);
  lcd.print("ID:");
  lcd.setCursor(3,2);
  lcd.print(WiFi.macAddress());
  lcd.setCursor(0,3);
  lcd.print("Firmware ver 2.0");
}
