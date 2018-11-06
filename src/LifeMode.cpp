#include "RazzleMode.h"

class LifeMode : public RazzleMode {
  public:
    virtual const char* name() { return "Life"; }
    virtual void draw(CRGB* frame);
    virtual framerate_t fps() { return 1; };
  private:
};

LifeMode theLifeMode;

void LifeMode::draw(CRGB* frame) {
  int count = 0;
  static uint32_t lastDraw = 0;
  uint32_t now = millis()/100;
  if (now == lastDraw) {
    return;
  }

  lastDraw = now;

  static int iterations = 0;
  for (led_t i = 0; i < numPixels(); i++) {
    if (frame[i]) { count++; }
  };

  led_t seed_width = 3;
  if (count == 0 || iterations > 100) {
    fill_solid( frame, numPixels(), CRGB::Black);
    for (led_t i = ((numPixels()/2) - seed_width/2); i < ((numPixels()/2)+seed_width/2); i++) {
      if (random(2)) { frame[i] = CRGB::White; }
    }
    iterations = 0;
  } else {
    uint8_t rule = 110;
    CRGB temp[numPixels()];
    for (led_t i = 1; i < numPixels()-1; i++) {
      uint8_t cur_pattern = (frame[i-1]!=(CRGB)CRGB::Black)*4 + (frame[i]!=(CRGB)CRGB::Black)*2 + (frame[i+1]!=(CRGB)CRGB::Black);
      temp[i] = ((rule >> cur_pattern)&0x01) ? CRGB::White : CRGB::Black;
    }
    memmove( frame, temp, numPixels() * sizeof( CRGB) );
    iterations++;
  }
}
