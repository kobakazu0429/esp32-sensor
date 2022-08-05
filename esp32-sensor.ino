#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Mhz19.h>
#include <WiFi.h>
#include <Wire.h>
// #include <esp_sntp.h>
// #include <time.h>

#include "debug.h"
#include "env.h"
#include "src/AE_SHT35/AE_SHT35.h"
#include "src/utils/time.hpp"

// #define JST 3600 * 9

#define JSON String

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String serverName = "http://192.168.11.2:4000/esp32-sensor/";

Mhz19 sensor;
AE_SHT35 SHT35 = AE_SHT35(0x45);

bool client(JSON json) {
  if (WiFi.status() != WL_CONNECTED) {
    // debug("WiFi Disconnected");
    return false;
  }

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.POST(json);

  // debug(json);
  // debug(status_code);

  http.end();
  return true;
}

String jsonBuilder(unsigned long unixtime, int carbonDioxide, int temperature,
                   int humidity) {
  StaticJsonDocument<256> doc;
  doc["unixtime"] = unixtime;
  doc["type"] = "sensor";

  JsonObject data = doc.createNestedObject("data");

  JsonObject data_carbon_dioxide = data.createNestedObject("carbon_dioxide");
  data_carbon_dioxide["value"] = carbonDioxide;
  data_carbon_dioxide["gain"] = 1;

  JsonObject data_temperature = data.createNestedObject("temperature");
  data_temperature["value"] = temperature;
  data_temperature["gain"] = 100;

  JsonObject data_humidity = data.createNestedObject("humidity");
  data_humidity["value"] = humidity;
  data_humidity["gain"] = 100;

  String output;
  serializeJson(doc, output);
  // debug(output);

  return output;
}

String jsonLogBuilder(unsigned long unixtime, String msg) {
  StaticJsonDocument<256> doc;
  doc["unixtime"] = unixtime;
  doc["type"] = "log";
  doc["data"] = msg;

  String output;
  serializeJson(doc, output);
  // debug(output);

  return output;
}

// unsigned long getUnixTimeAsSeconds() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     // Serial.println("Failed to obtain time");
//     return 0;
//   }
//   time(&now);
//   // debug(now);
//   return now;
// }

void logger(String msg) {
  client(jsonLogBuilder(getUnixTimeAsSeconds(), msg));
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  logger("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    logger(".");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  logger("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  logger(String(WiFi.localIP()));

  // API仕様上NTPサーバは3個まで指定できるようですが、実際には最初の1個だけが有効だと思われます。
  // https://garretlab.web.fc2.com/arduino/esp32/reference/cores/time/configTime.html
  // configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  // Serial.print("[NTP Svr] Connecting.");

  // while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
  //   Serial.print(".");
  //   delay(1000);  // １秒毎にリトライ
  // }

  initializeTime();

  sensor.begin(&Serial2);
  sensor.setMeasuringRange(Mhz19MeasuringRange::Ppm_5000);
  sensor.enableAutoBaseCalibration();

  // SHT35をソフトリセット
  SHT35.SoftReset();
  // 内蔵ヒーター 0:OFF 1:ON
  SHT35.Heater(0);

  Serial.println("Preheating");  // Preheating, 3 minutes
  logger("Preheating");          // Preheating, 3 minutes
  while (!sensor.isReady()) {
    Serial.print(".");
    logger(".");
    delay(5000);
  }

  Serial.println("SetupDone!");
  logger("SetupDone!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    SHT35.GetTempHum();

    auto carbonDioxide = sensor.getCarbonDioxide();
    if (carbonDioxide >= 0) {
      auto temp = String(SHT35.Temperature()) + " [°C], ";
      auto hum = String(SHT35.Humidity()) + " [%], ";
      auto co2 = String(carbonDioxide) + " [ppm]";
      Serial.println(temp + hum + co2);

      String output =
          jsonBuilder(getUnixTimeAsSeconds(), carbonDioxide,
                      SHT35.Temperature() * 100, SHT35.Humidity() * 100);
      Serial.println(output);
      client(output);

    } else {
      Serial.println("carbonDioxide < 0");
      logger("carbonDioxide-0");
    }

  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(1000);
}
