#ifndef _RazzleDevice_
#define _RazzleDevice_
#include "RazzleLeds.h"

struct devInfo {
  const char* mac;
  const char* hostname;
  led_t width;
  led_t height;
  EOrder colorOrder;
  uint32_t powerSupplyMilliAmps;
  uint8_t defaultDayBrightness;
  uint8_t defaultNightBrightness;
};

// other devices
// 5C:CF:7F:10:4C:43 - Wemos D1 (full size)

const devInfo* getDevice();
led_t numPixels();
bool isRemote();

#endif
