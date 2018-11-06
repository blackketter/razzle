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


class CopsMode : public RazzleMode {
  public:
    virtual const char* name() { return "Cops"; }
    virtual void draw(CRGB* frame);
  private:
};

CopsMode theCopsMode;


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

void CopsMode::draw(CRGB* frame) {
	if (numPixels() < COPS_LEDS) { return; }

  static frameIndex_t frameIndex = 0;
  static millis_t frameTime = 0;
  static const animationFrame* animationFrames = copsAnimation;

  bool drawFrame = false;

  millis_t now = millis();
  if (lastModeSwitch() > frameTime) {
    // restart animation
    frameTime = now;
    frameIndex = 0;
    drawFrame = true;
  } else if (now > (frameTime + animationFrames[frameIndex].delay )) {
    frameIndex++;
    frameTime = now;
    drawFrame = true;
  }

  if (drawFrame) {
    if (animationFrames[frameIndex].delay < 0) {
      frameIndex = 0;
    }

    fill_solid( &(frame[animationFrames[frameIndex].offset]), animationFrames[frameIndex].len, animationFrames[frameIndex].color);

  }
}
