#include <Arduino.h>
#include "wifi_manager.h"
#include "ota_manager.h"
#include "lcd_display.h"
#include "dht_sensor.h"
#include "pzem_driver.h"
#include "weather.h"
#include "webserver.h"
#include "timekeeper.h"
#include "config.h"

// Application-wide globals 
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

  wifi_begin();
  ota_setup();  // sets up ElegantOTA 
  timekeeper_begin();
  dht_begin();
  pzem_begin();
  weather_begin();

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
  lcd_process();
  wifi_update();
  timekeeper_update();

  static int currentScreen = 0;
  if (screenToggle) {
    screenToggle = false;
    currentScreen = (currentScreen + 1) % 4;
    lcd_clear();
  }

  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate >= 500) {
    lastDisplayUpdate = millis();

    switch (currentScreen) {
      case 0: timekeeper_display(); break;
      case 1: energy_display(); break;
      case 2: sensor_display(); break;
      case 3: device_info_display(); break;
    }
  }

  // periodic tasks
  static unsigned long lastWeather = 0;
  static bool initialWeatherFetched = false;
  if (!initialWeatherFetched && wifi_is_connected()) {
    weather_update();
    lastWeather = millis();
    initialWeatherFetched = true;
  }

  if (millis() - lastWeather >= Config::Weather::kRefreshIntervalMs) {
    lastWeather = millis();
    weather_update();
  }

  static unsigned long lastPzemUpdate = 0;
  if (millis() - lastPzemUpdate >= 1000) {
    lastPzemUpdate = millis();
    pzem_update();
  }

  delay(20);
}
