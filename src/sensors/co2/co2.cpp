#include "co2.hpp"

void CO2::init() {
  Serial2.begin(9600);
  sensor.begin(&Serial2);
  sensor.setMeasuringRange(Mhz19MeasuringRange::Ppm_5000);
  sensor.enableAutoBaseCalibration();
}

bool CO2::isReady() {
  return sensor.isReady();
}

int CO2::get() {
  return sensor.getCarbonDioxide();
}
