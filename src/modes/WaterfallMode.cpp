#include "RazzleMode.h"

class WaterfallMode : public RazzleMode {
  public:
    virtual const char* name() { return "Waterfall"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 15; }
    virtual bool canRun() { return getDevice()->height >= 8; }
    virtual bool dither() { return false; }
    virtual bool interpolate() { return false; }
  private:

};

WaterfallMode theWaterfallMode;

// here we go
void WaterfallMode::draw(FastLED_NeoMatrix* m) {

  pixel_t h = m->height();
  pixel_t w = m->width();

  for (pixel_t x = 0; x <= w; x++) {
    for (pixel_t y = h-1; y > 0; y--) {
        m->drawPixelCRGB(x, y, m->getPixelCRGB(x, y-1));
    }
  }

  pixel_t y = 0;
  for (pixel_t x = 0; x <= w; x++) {
    m->drawPixelCRGB(x,y, white(random(10) == 0 ? 255 : 0));
  }
}
