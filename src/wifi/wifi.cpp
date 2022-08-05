#include "wifi.hpp"

int WiFiInit() {
  return WiFi.begin(WIFI_SSID, WIFI_PASS);
}

bool isWiFiConnected() {
  return WiFi.status() != WL_CONNECTED;
}
