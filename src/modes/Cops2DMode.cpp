#include "RazzleMode.h"
#include "Clock.h"

typedef uint16_t frameIndex_t;

const millis_t ANIMATION2D_END = -1;

typedef struct {
  uint16_t color;
  uint8_t x;  // these are percentages
  uint8_t y;
  uint8_t w;
  uint8_t h;
  millis_t delay;
} Animation2DFrame;

typedef struct {
} Animation2DState;

class Animation2DMode : public RazzleMode {
  public:
    Animation2DMode(const char* name, const Animation2DFrame* frames) : _name(name), _Animation2DFrames(frames) {}
    virtual const char* name() { return _name; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual bool canRun() { return strcmp(getDevice()->hostname, "RazzleString") == 0; }
  private:
   frameIndex_t _frameIndex = 0;
   const char* _name;
   millis_t _frameTime = 0;
   const Animation2DFrame* _Animation2DFrames;
};

void Animation2DMode::draw(FastLED_NeoMatrix* m) {

  bool drawFrame = false;

  millis_t now = Uptime::millis();
  if (lastModeSwitch() > _frameTime) {
    // restart animation
    _frameTime = now;
    _frameIndex = 0;
    drawFrame = true;
  } else if (now > (_frameTime + _Animation2DFrames[_frameIndex].delay )) {
    _frameIndex++;
    _frameTime = now;
    drawFrame = true;
  }

  if (drawFrame) {
    if (_Animation2DFrames[_frameIndex].delay < 0) {
      _frameIndex = 0;
    }
    uint16_t x = (uint32_t)_Animation2DFrames[_frameIndex].x * m->width() / 100;
    uint16_t y = (uint32_t)_Animation2DFrames[_frameIndex].y * m->height() / 100;
    uint16_t w = (uint32_t)_Animation2DFrames[_frameIndex].w * m->width() / 100;
    uint16_t h = (uint32_t)_Animation2DFrames[_frameIndex].h * m->height() / 100;
    if (w==0) { w = 1; }
    if (h==0) { h = 1; }

    m->fillRect(x, y, w, h, _Animation2DFrames[_frameIndex].color);
  }
}

const Animation2DFrame copsAnimation2D[] = {
  { LED_WHITE_HIGH, 0,0,50,100, 30},
  { LED_BLACK, 0,0,50,100, 160},
  { LED_WHITE_HIGH, 0,0,50,100, 30},
  { LED_BLACK, 0,0,50,100, 160},
  { LED_WHITE_HIGH, 0,0,50,100, 30},
  { LED_BLACK, 0,0,50,100, 160},

  { LED_BLUE_HIGH, 0,0,50,100, 320},

  { LED_WHITE_HIGH, 51,0,50,100, 30},
  { LED_BLACK, 51,0,50,100, 160},
  { LED_WHITE_HIGH, 51,0,50,100, 30},
  { LED_BLACK, 51,0,50,100, 160},
  { LED_WHITE_HIGH, 51,0,50,100, 30},
  { LED_BLACK, 51,0,50,100, 160},

  { LED_RED_HIGH, 51,0,50,100, 320},

  { 0, 0,0,0,0, ANIMATION2D_END}
};

Animation2DMode theCops2DMode("Cops",copsAnimation2D);
