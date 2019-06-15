#include "RazzleMode.h"

class LinesMode : public RazzleMode {
  public:
    virtual const char* name() { return "Lines"; }
    virtual bool canRun() { return getDevice()->width > 2 && getDevice()->height > 2; }
    virtual framerate_t fps() { return 5; }

    virtual void draw(FastLED_NeoMatrix* m) {
      uint16_t w = m->width();
      uint16_t h = m->height();
      uint16_t x = _lastx;
      uint16_t y = _lasty;
      _lastx = random(w);
      _lasty = random(h);
      CRGB rgb = ColorFromPalette( HeatColors_p, _color++);
      m->drawLine(x, y, _lastx, _lasty, FastLED_NeoMatrix::Color(rgb));
    };
  private:
    uint16_t _lastx, _lasty;
    uint8_t _color;
};

LinesMode theLinesMode;




