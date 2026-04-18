#include "dht_sensor.h"
#include <DHT.h>
#include "lcd_display.h"
#include "weather.h" // to show internet weather strings

#define DHTTYPE DHT11
static const uint8_t DHTPin = D3;
static DHT dht(DHTPin, DHTTYPE);

static float g_temp = NAN;
static float g_hum = NAN;

void dht_begin() {
  dht.begin();
}

float dht_get_temperature() {
  g_temp = dht.readTemperature();
  return g_temp;
}

float dht_get_humidity() {
  g_hum = dht.readHumidity();
  return g_hum;
}

void sensor_display() {
  float localTemp = dht_get_temperature();
  float localHum  = dht_get_humidity();
  char line[21];
  const char* remoteDesc = weather_get_desc();

  lcd_write_line(0, "Weather        Ch,IN");

  snprintf(line, sizeof(line), "L%cT:%2d%cC H:%2d%%", 1, (int)localTemp, 223, (int)localHum);
  lcd_write_line(1, line);

  if (weather_has_data()) {
    snprintf(line, sizeof(line), "S%cT:%2d%cC H:%2d%%", 1, (int)weather_get_temp(), 223, (int)weather_get_humidity());
  } else {
    snprintf(line, sizeof(line), "S%cWeather Offline", 1);
  }
  lcd_write_line(2, line);

  lcd_write_line(3, String(char(1)) + remoteDesc);
}
