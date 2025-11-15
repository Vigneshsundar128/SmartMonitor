#include "weather.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

static float g_temp = NAN;
static int g_hum = 0;
static String g_desc = "Updating...";

// Replace with your full OpenWeatherMap or API URL (including key & query)
const char* WEATHER_URL = "YOUR_OPENWEATHERMAP_URL";

float weather_get_temp() { return g_temp; }
int weather_get_humidity() { return g_hum; }
const char* weather_get_desc() { return g_desc.c_str(); }

void weather_update() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  if (!http.begin(client, WEATHER_URL)) {
    Serial.println("Failed to begin HTTP");
    return;
  }
  int code = http.GET();
  if (code > 0) {
    String payload = http.getString();
    StaticJsonDocument<2048> doc;
    auto err = deserializeJson(doc, payload);
    if (!err) {
      JsonObject forecast = doc["list"][0];
      g_temp = forecast["main"]["temp"] | NAN;
      g_hum = forecast["main"]["humidity"] | 0;
      g_desc = forecast["weather"][0]["description"].as<String>();
      Serial.println("Weather updated");
    } else {
      Serial.print("JSON error: ");
      Serial.println(err.c_str());
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(http.errorToString(code));
  }
  http.end();
}
