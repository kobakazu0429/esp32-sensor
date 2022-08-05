// #pragma once
#include "../../AE_SHT35/AE_SHT35.h"

class TempHum {
  public:
  void init();
  float getTemperature();
  float getHumidity();

  private:
  AE_SHT35 SHT35 = AE_SHT35(0x45);
};
