// #pragma once
#include <Mhz19.h>

class CO2 {
  public:
  void init();
  bool isReady();
  int get();

  private:
  Mhz19 sensor;
};
