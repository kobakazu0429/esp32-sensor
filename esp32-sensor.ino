#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>

#include "debug.h"
#include "env.h"
#include "src/sensors/co2/co2.hpp"
#include "src/sensors/temp_hum/temp_hum.hpp"
#include "src/utils/time.hpp"

#define JSON String

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String serverName = "http://192.168.11.2:4000/esp32-sensor/";

CO2 co2;
TempHum th;

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

void logger(String msg) {
  client(jsonLogBuilder(getUnixTimeAsSeconds(), msg));
}

void setup() {
  Serial.begin(115200);

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

  co2.init();
  th.init();
  initializeTime();

  Serial.println("Preheating");  // Preheating, 3 minutes
  logger("Preheating");
  while (!co2.isReady() || !isReadyTime()) {
    Serial.print(".");
    logger(".");
    delay(5000);
  }

  Serial.println("SetupDone!");
  logger("SetupDone!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    auto carbonDioxide = co2.get();
    if (carbonDioxide >= 0) {
      auto temp = String(th.getTemperature()) + " [°C], ";
      auto hum = String(th.getHumidity()) + " [%], ";
      auto co2 = String(carbonDioxide) + " [ppm]";
      Serial.println(temp + hum + co2);

      String output =
          jsonBuilder(getUnixTimeAsSeconds(), carbonDioxide,
                      th.getTemperature() * 100, th.getHumidity() * 100);
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
