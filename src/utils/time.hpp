#pragma once

#include <esp_sntp.h>
#include <time.h>

#include "../wifi/wifi.hpp"

#define JST 3600 * 9
#define SUMMER_TIME 0

bool initializeTime();
bool isReadyTime();
unsigned long getUnixTimeAsSeconds();
