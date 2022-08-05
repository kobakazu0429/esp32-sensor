#include "client.hpp"

bool clientApiPost(String json) {
  if (!isWiFiConnected()) {
    return false;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.POST(json);

  http.end();

  if (status_code != 200) {
    return false;
  }

  return true;
}
