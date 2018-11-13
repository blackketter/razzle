#include "RazzleMode.h"

class ZipperMode : public RazzleMode {
  public:
    virtual const char* name() { return "Zipper"; }
    virtual void draw(CRGB* frame) {
      _zipper = _zipper % numPixels();
      fill_solid( frame, numPixels(), CRGB::Black);
      frame[_zipper++] = CRGB::White;
    };
  private:
    led_t _zipper = 0;
};

ZipperMode theZipperMode;
