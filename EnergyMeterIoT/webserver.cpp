#include "webserver.h"
#include "wifi_manager.h"
#include "pzem_driver.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "webpage.h"

AsyncWebServer server(80); // single global server used by OTA and web UI

// index_html moved inline for readability (you can keep your original big HTML)
extern const char index_html[]; // forward if you want to move to PROGMEM

void webserver_begin() {
  // serve main page (replace with the large HTML from original if desired)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // simple processor - you can replicate your original processor() behavior
    request->send_P(200, "text/html", index_html, processor);
  });

  // data route serves JSON updated by pzem_driver
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"voltage\":\"" + String(isnan(pzem_get_voltage()) ? "Error" : String(pzem_get_voltage(), 1)) + "\",";
    json += "\"current\":\"" + String(isnan(pzem_get_current()) ? "Error" : String(pzem_get_current(), 2)) + "\",";
    json += "\"power\":\"" + String(isnan(pzem_get_power()) ? "Error" : String(pzem_get_power(), 1)) + "\",";
    json += "\"energy\":\"" + String(isnan(pzem_get_energy()) ? "Error" : String(pzem_get_energy(), 3)) + "\",";
    json += "\"frequency\":\"" + String(isnan(pzem_get_frequency()) ? "Error" : String(pzem_get_frequency(), 1)) + "\",";
    json += "\"pf\":\"" + String(isnan(pzem_get_pf()) ? "Error" : String(pzem_get_pf(), 2)) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}
