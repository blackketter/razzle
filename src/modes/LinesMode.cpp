#include "RazzleMode.h"

class LinesMode : public RazzleMode {
  public:
    virtual const char* name() { return "Lines"; }
    virtual bool canRun() { return getDevice()->width > 2 && getDevice()->height > 2; }
    virtual framerate_t fps() { return 1; }

    virtual void draw(FastLED_NeoMatrix* m) {
      uint16_t w = getDevice()->width;
      uint16_t h = getDevice()->height;
      uint16_t x = _lastx;
      uint16_t y = _lasty;
      _lastx = random(w);
      _lasty = random(h);
      m->drawLine(x, y, _lastx, _lasty, _color++);
    };
  private:
    uint16_t _lastx, _lasty;
    uint16_t _color;
};

LinesMode theLinesMode;




