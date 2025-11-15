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

  lcd_set_cursor(0,0);
  lcd_print("Weather");
  lcd_set_cursor(15,0);
  lcd_print("Ch,IN");

  // local
  lcd_set_cursor(0,1);
  lcd_print("L");
  lcd_write_custom(1);
  lcd_printf("Temp:%d%cC Hum:%d%%", (int)localTemp, 223, (int)localHum);

  // internet (from weather module)
  lcd_set_cursor(0,2);
  lcd_print("S");
  lcd_write_custom(1);
  lcd_printf("Temp:%d%cC Hum:%d%%", (int)weather_get_temp(), (char)223, (int)weather_get_humidity());

  // description
  lcd_set_cursor(0,3);
  lcd_write_custom(1);
  lcd_print(weather_get_desc());
}
