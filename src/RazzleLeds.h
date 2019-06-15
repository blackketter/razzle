#ifndef _RazzleLeds_
#define _RazzleLeds_

#define FASTLED_ALLOW_INTERRUPTS 0

// suppress extraneous warnings
#define FASTLED_INTERNAL

#include "FastLED.h"

#include "Razzle.h"

#include "WiFiConsole.h"

typedef uint16_t led_t;
typedef float framerate_t;
typedef int16_t pixel_t;

bool setLEDMode(const char* newmode);
const char* getLEDMode();
bool isLEDMode(const char* ismode);

void setNextLEDMode(bool allowWants = false);
void setNextLEDModeSet();

bool shouldAutoSwitch();

millis_t lastModeSwitch();
void resetLastModeSwitch();

uint8_t getBrightness();  // depends on whether it's day or night
uint8_t getNightBrightness();
uint8_t getDayBrightness();
void setBrightness(uint8_t day, uint8_t night);

bool isDay();

void  setupLeds();
void  loopLeds();

extern WiFiConsole console;

CRGB white(uint8_t y);
#endif
