#ifndef _RazzleLeds_
#define _RazzleLeds_

#define FASTLED_ALLOW_INTERRUPTS 0

// suppress extraneous warnings
#define FASTLED_INTERNAL

#include "FastLED.h"

#include "Razzle.h"

#include "WiFiConsole.h"

typedef uint16_t led_t;
typedef uint16_t framerate_t;

bool setLEDMode(const char* newmode);
void setNextLEDMode();
void setNextLEDModeSet();
bool shouldAutoSwitch();
const char* getLEDMode();
uint32_t lastModeSwitch();

uint8_t getBrightness();  // depends on whether it's day or night
uint8_t getNightBrightness();
uint8_t getDayBrightness();
void setBrightness(uint8_t day, uint8_t night);

bool isDay();

void  setupLeds();
void  loopLeds();

extern WiFiConsole console;

uint32_t white(uint8_t y);


#endif
