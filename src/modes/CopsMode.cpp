#include "RazzleMode.h"
#include "Clock.h"

typedef uint16_t frameIndex_t;

const millis_t ANIMATION_END = -1;

typedef struct {
  CRGB color;
  uint8_t offset;
  uint8_t len;
  millis_t delay;
} animationFrame;

typedef struct {
} animationState;

class AnimationMode : public RazzleMode {
  public:
    AnimationMode(const char* name, const animationFrame* frames, led_t minLeds) : _name(name), _animationFrames(frames), _minLeds(minLeds) {}
    virtual const char* name() { return _name; }
    virtual void draw(CRGB* frame);
    virtual bool canRun() { return numPixels() >= _minLeds; }
  private:
   frameIndex_t _frameIndex = 0;
   const char* _name;
   millis_t _frameTime = 0;
   const animationFrame* _animationFrames;
   led_t _minLeds;
};

void AnimationMode::draw(CRGB* frame) {

  bool drawFrame = false;

  millis_t now = Uptime::millis();
  if (lastModeSwitch() > _frameTime) {
    // restart animation
    _frameTime = now;
    _frameIndex = 0;
    drawFrame = true;
  } else if (now > (_frameTime + _animationFrames[_frameIndex].delay )) {
    _frameIndex++;
    _frameTime = now;
    drawFrame = true;
  }

  if (drawFrame) {
    if (_animationFrames[_frameIndex].delay < 0) {
      _frameIndex = 0;
    }

    fill_solid( &(frame[_animationFrames[_frameIndex].offset]), _animationFrames[_frameIndex].len, _animationFrames[_frameIndex].color);

  }
}

const uint8_t COPS_LEDS = 50;
const uint8_t MIDDLE_LED = COPS_LEDS/2;
const uint8_t SEG_LEN = 6;

const animationFrame copsAnimation[] = {
  { CRGB::White, MIDDLE_LED-SEG_LEN, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED-SEG_LEN, SEG_LEN, 160},
  { CRGB::White, MIDDLE_LED-SEG_LEN, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED-SEG_LEN, SEG_LEN, 160},
  { CRGB::White, MIDDLE_LED-SEG_LEN, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED-SEG_LEN, SEG_LEN, 160},

  { CRGB::Blue, MIDDLE_LED-SEG_LEN, SEG_LEN, 320},

  { CRGB::White, MIDDLE_LED, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED, SEG_LEN, 160},
  { CRGB::White, MIDDLE_LED, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED, SEG_LEN, 160},
  { CRGB::White, MIDDLE_LED, SEG_LEN, 30},
  { CRGB::Black, MIDDLE_LED, SEG_LEN, 160},

  { CRGB::Red, MIDDLE_LED, SEG_LEN, 320},

  { 0, 0, 0, ANIMATION_END}
};

AnimationMode theCopsMode("Cops",copsAnimation,COPS_LEDS);
