#ifndef _RazzleLeds_
#define _RazzleLeds_

#include "FastLED.h"
#include "WiFiConsole.h"

typedef uint16_t led_t;

enum modes {
  FIRSTMODE,
  COLORS = FIRSTMODE,
  COPS = COLORS,
//  AMBIENT,
  FIRE,
  LIFE,
  BREATHING,
  WAVE,
  FLASHES,
  ZIP,
//  RAINBOW,
  RAINBOWROTATE,
  NOISE,
  WHITENOISE,
  WHITE,
  ENDCOLORS,

  WHITES,
  OFF = WHITES,
  ON,
  HALF,
  GREY80,
  GREY40,
  GREY20,
  GREY10,

  ENDWHITES,
  LASTMODE = ENDWHITES,
};

typedef uint16_t ledmode_t;

void  setLedMode(ledmode_t newmode);
ledmode_t   getLedMode();
uint32_t lastModeSwitch();

void  setupLeds(EOrder order, led_t led_count, uint32_t milliAmpsMax);
void  loopLeds();

extern WiFiConsole console;

#endif