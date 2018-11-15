#ifndef _RazzleDevice_
#define _RazzleDevice_
#include "RazzleLeds.h"

const int MAX_SEGMENTS = 8;

#ifdef ESP8266
const uint8_t LED_DATA_PIN0 = D2;
const uint8_t LED_DATA_PIN1 = D0;
const uint8_t LED_DATA_PIN2 = D0;
const uint8_t LED_DATA_PIN3 = D0;
const uint8_t LED_DATA_PIN4 = D0;
const uint8_t LED_DATA_PIN5 = D0;
const uint8_t LED_DATA_PIN6 = D0;
const uint8_t LED_DATA_PIN7 = D0;
#else
const uint8_t LED_DATA_PIN0 = 23;
const uint8_t LED_DATA_PIN1 = 16;
const uint8_t LED_DATA_PIN2 = 17;
const uint8_t LED_DATA_PIN3 = 19;
const uint8_t LED_DATA_PIN4 = 4;  // 5 is no good
const uint8_t LED_DATA_PIN5 = 18;
const uint8_t LED_DATA_PIN6 = 0;
const uint8_t LED_DATA_PIN7 = 0;

#endif

#define CHIPSET         WS2811
#define LIGHT_SENSOR    (A0)

struct devInfo {
  const char* mac;
  const char* hostname;
  led_t width;
  led_t height;
  EOrder colorOrder;
  uint32_t powerSupplyMilliAmps;
  uint8_t defaultDayBrightness;
  uint8_t defaultNightBrightness;
  led_t segment[MAX_SEGMENTS];
};

// other devices
// 5C:CF:7F:10:4C:43 - Wemos D1 (full size)

const devInfo* getDevice();
led_t numPixels();
bool isRemote();

#endif
