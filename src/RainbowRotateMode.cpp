#include "RazzleMode.h"

class RainbowRotateMode : public RazzleMode {
  public:
    virtual const char* name() { return "RainbowRotate"; }
    virtual void draw(CRGB* frame) {
      fill_rainbow(frame, numPixels(), _cycle++);
    };
  private:
    uint8_t _cycle;
};

RainbowRotateMode theRainbowRotateMode;

