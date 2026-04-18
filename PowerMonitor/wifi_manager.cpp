#include "wifi_manager.h"

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASS";
static unsigned long lastReconnectAttempt = 0;
static const unsigned long WIFI_RETRY_INTERVAL_MS = 10000;
static bool g_wasConnected = false;
static wl_status_t g_lastLoggedStatus = WL_IDLE_STATUS;

void wifi_begin() {
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  lastReconnectAttempt = millis();
  Serial.println("WiFi connection started");
}

void wifi_update() {
  wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    if (!g_wasConnected) {
      Serial.print("Connected. IP: ");
      Serial.println(WiFi.localIP());
      g_wasConnected = true;
    }
    return;
  }

  if (g_wasConnected || status != g_lastLoggedStatus) {
    Serial.print("WiFi unavailable, status=");
    Serial.println(status);
    g_wasConnected = false;
    g_lastLoggedStatus = status;
  }

  if (millis() - lastReconnectAttempt >= WIFI_RETRY_INTERVAL_MS) {
    Serial.println("Retrying WiFi...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    lastReconnectAttempt = millis();
  }
}

bool wifi_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

IPAddress wifi_get_local_ip() {
  return WiFi.localIP();
}

String wifi_get_mac() {
  return WiFi.macAddress();
}
