#pragma once
#include <Arduino.h>

#define BTN_SCREEN_PIN 14
#define BTN_LED_PIN 2

extern volatile bool screenToggle;
extern volatile bool LCD_LED;

void lcd_init();
void lcd_clear();
void lcd_backlight(bool on);
void lcd_print(const char* s);
void lcd_set_cursor(uint8_t c, uint8_t r);
void lcd_write_custom(uint8_t id);
void lcd_printf(const char* fmt, ...);

void show_screen(int screen);
void device_info_display();
