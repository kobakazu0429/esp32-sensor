#include "client.hpp"

bool clientApiPost(String json) {
  if (isWiFiConnected()) {
    // debug("WiFi Disconnected");
    return false;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.POST(json);

  // debug(json);
  // debug(status_code);

  http.end();

  if (status_code != 200) {
    return false;
  }

  return true;
}
