#pragma once
#include <ESP8266WiFi.h>

void wifi_begin();
void wifi_update();
bool wifi_is_connected();
IPAddress wifi_get_local_ip();
String wifi_get_mac();
