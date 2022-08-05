#include "json.hpp"

String jsonBuilder(unsigned long unixtime,                    //
                   int carbonDioxide, int carbonDioxideGain,  //
                   int temperature, int temperatureGain,      //
                   int humidity, int humidityGain             //
) {
  StaticJsonDocument<256> doc;
  doc["unixtime"] = unixtime;
  doc["type"] = "sensor";

  JsonObject data = doc.createNestedObject("data");

  JsonObject data_carbon_dioxide = data.createNestedObject("carbon_dioxide");
  data_carbon_dioxide["value"] = carbonDioxide;
  data_carbon_dioxide["gain"] = carbonDioxideGain;

  JsonObject data_temperature = data.createNestedObject("temperature");
  data_temperature["value"] = temperature;
  data_temperature["gain"] = temperatureGain;

  JsonObject data_humidity = data.createNestedObject("humidity");
  data_humidity["value"] = humidity;
  data_humidity["gain"] = humidityGain;

  String output;
  serializeJson(doc, output);

  return output;
}

String jsonLogBuilder(unsigned long unixtime, String msg) {
  StaticJsonDocument<256> doc;
  doc["unixtime"] = unixtime;
  doc["type"] = "log";
  doc["data"] = msg;

  String output;
  serializeJson(doc, output);

  return output;
}
