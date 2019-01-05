#include "RazzleMode.h"
#include "math.h"

class VortexMode : public RazzleMode {
  public:
    virtual const char* name() { return "Vortex"; }

    virtual void draw(FastLED_NeoMatrix* m);

    virtual bool canRun() { return getDevice()->width > 7 && getDevice()->height > 7; }

    virtual void begin(uint16_t w, uint16_t h) {
      _centerX = w/2.0;
      _centerY = h/2.0;
      _points = 2* (w > h ? w : h);
      _radius = new float[_points];
      _theta = new float[_points];
      _maxRadius = sqrtf((float)w*w+(float)h*h);
      for (uint16_t i = 0; i < _points; i++) {
        _radius[i] = random(_maxRadius);
        _theta[i] = random(360) * 2.0 * PI / 360.0;
      }
    }

    virtual void end() {
      delete _radius;
      delete _theta;
    }

  private:
    float _centerX, _centerY;
    float* _radius;
    float* _theta;
    uint16_t _points;
    float _maxRadius;
};

VortexMode theVortexMode;

// here we go
void VortexMode::draw(FastLED_NeoMatrix* m) {

  m->fillScreen(LED_BLACK);

  for (uint16_t i = 0; i < _points; i++) {
    uint16_t x = sinf(_theta[i]) * _radius[i] + _centerX;
    uint16_t y = cosf(_theta[i]) * _radius[i] + _centerY;

    m->drawPixel(x, y, LED_WHITE_HIGH);
    _theta[i] += 2.0*PI/50/_radius[i];
    _radius[i] -= _maxRadius / 1000;
    if (_radius[i] < 0) {
      _radius[i] = _maxRadius;
      _theta[i] = random(360) * 2.0 * PI / 360.0;
    }
  }
}
