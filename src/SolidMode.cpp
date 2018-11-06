#include "RazzleMode.h"

class SolidMode : public RazzleMode {
  public:
    SolidMode(const char* name, CRGB color) : _name(name), _color(color) {}
    virtual const char* name() { return _name; }
    virtual void draw(CRGB* frame) {
      fill_solid(frame, numPixels(), _color);
    }

  protected:
    const char* _name;
    CRGB _color;
};

SolidMode SolidOff("Off", CRGB::Black);
SolidMode SolidOn("On", CRGB::White);
SolidMode SolidGrey80("Grey80", 0x80808080);
SolidMode SolidGrey40("Grey40", 0x40404040);
SolidMode SolidGrey20("Grey20", 0x20202020);
SolidMode SolidGrey10("Grey10", 0x10101010);
SolidMode SolidGrey08("Grey08", 0x08080808);
SolidMode SolidGrey04("Grey04", 0x04040404);
SolidMode SolidGrey02("Grey02", 0x02020202);
SolidMode SolidGrey01("Grey01", 0x01010101);
