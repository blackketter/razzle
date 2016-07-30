#include "FastLED.h"

enum modes {
  FIRSTMODE,
  FIRE = FIRSTMODE,
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
  END,
  OFF,
  ON,
};

void  setLedMode(int newmode);
int   getLedMode();
uint32_t lastModeSwitch();

void  setupLeds(EOrder order, int led_count);
void  loopLeds();
