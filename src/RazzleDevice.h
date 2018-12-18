#ifndef _RazzleDevice_
#define _RazzleDevice_
#include "RazzleLeds.h"

const int MAX_SEGMENTS = 8;
#define CHIPSET         WS2811
#define LIGHT_SENSOR    (A0)

#ifdef ESP8266
#define BUTTON_PIN (D6)
#define BUTTON_POLARITY (LOW)
#define BUTTON_INPUT (INPUT_PULLUP)

const uint8_t LED_DATA_PIN0 = D2;
const uint8_t LED_DATA_PIN1 = D2;
const uint8_t LED_DATA_PIN2 = D2;
const uint8_t LED_DATA_PIN3 = D2;
const uint8_t LED_DATA_PIN4 = D2;
const uint8_t LED_DATA_PIN5 = D2;
const uint8_t LED_DATA_PIN6 = D2;
const uint8_t LED_DATA_PIN7 = D2;
#endif

#if defined(ESP32)
#define BUTTON_PIN (22)
#define BUTTON_POLARITY (HIGH)
#define BUTTON_INPUT (INPUT_PULLDOWN)

// esp32 doesn't use Dn for digital pin constants
#define D1 (0)
#define D4 (0)
#define D5 (0)

const uint8_t LED_DATA_PIN0 = 23;
const uint8_t LED_DATA_PIN1 = 16;
const uint8_t LED_DATA_PIN2 = 17;
const uint8_t LED_DATA_PIN3 = 19;
const uint8_t LED_DATA_PIN4 = 4;  // 5 is no good
const uint8_t LED_DATA_PIN5 = 18;
const uint8_t LED_DATA_PIN6 = 0;
const uint8_t LED_DATA_PIN7 = 0;

#endif



struct devInfo {
  const char* mac;
  const char* hostname;
  led_t width;
  led_t height;
  EOrder colorOrder;
  uint32_t powerSupplyMilliAmps;
  uint8_t defaultDayBrightness;
  uint8_t defaultNightBrightness;
  uint8_t relayPin;
  uint8_t pirPin;
  uint8_t matrixType;
  led_t segment[MAX_SEGMENTS];
};

// other devices
// 5C:CF:7F:10:4C:43 - Wemos D1 (full size)

const devInfo* getDevice();
led_t numPixels();
bool isRemote();
led_t maxSegmentLen();

#endif
