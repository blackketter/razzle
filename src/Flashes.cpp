#include "RazzleMode.h"

class FlashesMode : public RazzleMode {
  public:
    virtual const char* name() { return "Flashes"; }
    virtual void draw(CRGB* frame) {
      fill_solid( frame, numPixels(), CRGB::Black);
      frame[random(numPixels())] = CRGB::White;
    };
  private:
};

FlashesMode theFlashesMode;



