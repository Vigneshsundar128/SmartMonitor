#include "wifi_manager.h"

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASS";

void wifi_begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Simple blocking connect with LCD feedback handled elsewhere
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

IPAddress wifi_get_local_ip() {
  return WiFi.localIP();
}

String wifi_get_mac() {
  return WiFi.macAddress();
}
