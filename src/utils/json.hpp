#pragma once

#include <ArduinoJson.h>

String jsonBuilder(unsigned long unixtime,                    //
                   int carbonDioxide, int carbonDioxideGain,  //
                   int temperature, int temperatureGain,      //
                   int humidity, int humidityGain             //
);
String jsonLogBuilder(unsigned long unixtime, String msg);
