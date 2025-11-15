// main.cpp - entry point, high-level orchestration
#include <Arduino.h>
#include "wifi_manager.h"
#include "ota_manager.h"
#include "lcd_display.h"
#include "dht_sensor.h"
#include "pzem_driver.h"
#include "weather.h"
#include "webserver.h"
#include "timekeeper.h"

// Application-wide globals (kept minimal; drivers expose accessors)
extern volatile bool screenToggle;
extern volatile bool LCD_LED;

void IRAM_ATTR onScreenButton(); // forward (implemented in lcd_display)
void IRAM_ATTR onBacklightButton();

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize subsystems
  lcd_init();
  lcd_clear();

  wifi_begin(); // starts WiFi and blocks until connected
  ota_setup();  // sets up ElegantOTA + webserver hooks (uses AsyncWebServer)
  timekeeper_begin();
  dht_begin();
  pzem_begin();

  // Attach interrupts (pins defined in lcd_display)
  pinMode(BTN_SCREEN_PIN, INPUT_PULLUP);
  pinMode(BTN_LED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_SCREEN_PIN), onScreenButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_LED_PIN), onBacklightButton, FALLING);

  show_screen(0);
  webserver_begin(); // starts webserver routes
}

void loop() {
  ota_loop();
  timekeeper_update();

  static int currentScreen = 0;
  if (screenToggle) {
    screenToggle = false;
    currentScreen = (currentScreen + 1) % 4;
    lcd_clear();
  }

  // update the currently visible screen
  switch (currentScreen) {
    case 0: timekeeper_display(); break;
    case 1: energy_display(); break;
    case 2: sensor_display(); break;
    case 3: device_info_display(); break;
  }

  // periodic tasks
  static unsigned long lastWeather = 0;
  if (millis() - lastWeather >= 600000) { // 10 minutes
    lastWeather = millis();
    weather_update();
  }

  // update energy readings frequently
  pzem_update();

  delay(1000);
}
