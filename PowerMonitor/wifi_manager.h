#pragma once
#include <ESP8266WiFi.h>

void wifi_begin();
IPAddress wifi_get_local_ip();
String wifi_get_mac();
