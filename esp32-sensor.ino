#include <Arduino.h>
#include <Wire.h>

#include "src/client/client.hpp"
#include "src/sensors/co2/co2.hpp"
#include "src/sensors/temp_hum/temp_hum.hpp"
#include "src/utils/json.hpp"
#include "src/utils/time.hpp"
#include "src/wifi/wifi.hpp"

void logger(String msg) {
  if (msg.equals(".")) {
    Serial.print(msg);
  } else {
    Serial.println(msg);
  }

  clientApiPost(jsonLogBuilder(getUnixTimeAsSeconds(), msg));
}

// -----

CO2 co2;
TempHum th;

void setup() {
  Serial.begin(115200);

  logger("Connecting");

  WiFiInit();
  co2.init();
  th.init();
  while (!isWiFiConnected()) {
    logger(".");
    delay(500);
  }

  logger("Connected to WiFi network");

  initializeTime();
  logger("Start: CO2 sensor preheat and time config");
  // Preheating, 3 minutes
  while (!co2.isReady() || !isReadyTime()) {
    logger(".");
    delay(2000);
  }

  logger("Setup Done !");
}

void loop() {
  if (!isWiFiConnected()) {
    logger("WiFi Disconnected");
    delay(1000);
    return;
  }

  int carbonDioxide = co2.get();
  if (carbonDioxide < 0) {
    logger("carbonDioxide < 0");
    delay(1000);
    return;
  }

  String sensorValuesJson = jsonBuilder(getUnixTimeAsSeconds(),          //
                                        carbonDioxide, 1,                //
                                        th.getTemperature() * 100, 100,  //
                                        th.getHumidity() * 100, 100      //
  );
  logger(sensorValuesJson);
  clientApiPost(sensorValuesJson);
  delay(1000);
}
