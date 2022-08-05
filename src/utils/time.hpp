#pragma once

#define JST 3600 * 9
#define SUMMER_TIME 0

void initializeTime();
bool isReadyTime();
unsigned long getUnixTimeAsSeconds();
