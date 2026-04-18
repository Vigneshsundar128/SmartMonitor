#include "weather.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "wifi_manager.h"

static float g_temp = NAN;
static int g_hum = 0;
static String g_desc = "No weather data";
static bool g_hasWeatherData = false;

// Replace with your full OpenWeatherMap or API URL
const char* WEATHER_URL = "YOUR_OPENWEATHERMAP_URL";

float weather_get_temp() { return g_temp; }
int weather_get_humidity() { return g_hum; }
const char* weather_get_desc() { return g_desc.c_str(); }
bool weather_has_data() { return g_hasWeatherData; }

void weather_begin() {
  g_temp = NAN;
  g_hum = 0;
  g_desc = "No weather data";
  g_hasWeatherData = false;
}

void weather_update() {
  if (!wifi_is_connected()) {
    Serial.println("Skipping weather update: WiFi disconnected");
    return;
  }

  if (String(WEATHER_URL) == "YOUR_OPENWEATHERMAP_URL") {
    Serial.println("Skipping weather update: WEATHER_URL not configured");
    return;
  }

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
      g_hasWeatherData = true;
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
