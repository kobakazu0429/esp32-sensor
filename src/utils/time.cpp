#include "time.hpp"

#include <Arduino.h>
#include <esp_sntp.h>
#include <time.h>

void initializeTime() {
  // API仕様上NTPサーバは3個まで指定できるようですが、実際には最初の1個だけが有効だと思われます。
  // https://garretlab.web.fc2.com/arduino/esp32/reference/cores/time/configTime.html
  configTime(JST, SUMMER_TIME, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  // Serial.print("[NTP Svr] Connecting.");

  // while (isInitializedTime()) {
  //   Serial.print(".");
  //   delay(1000);  // １秒毎にリトライ
  // }
}

bool isInitializedTime() {
  return sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET;
}

unsigned long getUnixTimeAsSeconds() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // Serial.println("Failed to obtain time");
    return 0;
  }
  time(&now);
  // debug(now);
  return now;
}
