#include "temp_hum.hpp"

void TempHum::init() {
  // SHT35をソフトリセット
  SHT35.SoftReset();
  // 内蔵ヒーター 0:OFF 1:ON
  SHT35.Heater(0);
}

float TempHum::getTemperature() {
  SHT35.GetTempHum();
  return SHT35.Temperature();
}

float TempHum::getHumidity() {
  SHT35.GetTempHum();
  return SHT35.Humidity();
}
