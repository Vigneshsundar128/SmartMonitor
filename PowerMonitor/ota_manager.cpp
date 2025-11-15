#include "ota_manager.h"
#include <ElegantOTA.h>
#include <ESPAsyncWebServer.h>
extern AsyncWebServer server; // declared in webserver.cpp

void ota_setup() {
  // webserver.cpp creates server; we start ElegantOTA in webserver begin sequence.
  Serial.println("OTA setup done (handled by webserver module).");
}

void ota_loop() {
  // ElegantOTA has no per-loop processing when using Async version,
  // but keep this function in case of future requirements
}
