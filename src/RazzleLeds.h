#ifndef _RazzleLeds_
#define _RazzleLeds_

#define FASTLED_ALLOW_INTERRUPTS 0
#include "Razzle.h"

#include "FastLED.h"
#include "WiFiConsole.h"

typedef uint16_t led_t;
typedef uint16_t framerate_t;

void  setLedMode(const char* newmode);
const char* getLedMode();
uint32_t lastModeSwitch();

uint8_t getBrightness();  // depends on whether it's day or night
uint8_t getNightBrightness();
uint8_t getDayBrightness();
void setBrightness(uint8_t day, uint8_t night);

bool isDay();

void  setupLeds(EOrder order, led_t led_count, uint32_t milliAmpsMax);
void  loopLeds();

extern WiFiConsole console;

uint32_t white(uint8_t y);


#endif
