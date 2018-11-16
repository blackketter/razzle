#include "RazzleMode.h"

class AnalogClockMode : public RazzleMode {
  public:
    virtual const char* name() { return "Analog"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 1; }
    virtual bool canRun() { return getDevice()->width > 7 && getDevice()->height > 7 && clock.hasBeenSet(); }
    virtual bool wantsToRun() { return clock.hour() > 22 || clock.hour() < 7; }
    virtual bool dither() { return false; }
  private:

};

AnalogClockMode theAnalogClockMode;

// here we go
void AnalogClockMode::draw(FastLED_NeoMatrix* m) {

  m->fillScreen(LED_BLACK);

  pixel_t centerX = m->width()/2;
  pixel_t centerY = m->height()/2;
  pixel_t r = (centerX > centerY ? centerY : centerX);
  // draw face circle
  // m->drawCircle(centerX, centerY, r, LED_WHITE_HIGH);

  float len;
  pixel_t endX, endY;
  float hour, minute, second;
  second = (float)clock.second() + (float)clock.fracMillis()/1000.0;
  minute = clock.minute() + second/60.0;

  hour = clock.hour();
    if (hour > 11) {
    hour = hour - 12;
  }
  hour = hour+minute/60.0;

  // draw minute hand
  len = r;
  endX = sin(minute / 60.0 * M_PI * 2.0)*len;
  endY = -cos(minute / 60.0 * M_PI * 2.0)*len;
  m->drawLine(centerX, centerY, centerX+endX, centerY+endY, LED_ORANGE_HIGH);

  // draw hour hand
  len = (r*3)/4;
  endX = sin(hour / 12.0 * M_PI * 2.0)*len;
  endY = -cos(hour / 12.0 * M_PI * 2.0)*len;
  m->drawLine(centerX, centerY, centerX+endX, centerY+endY, LED_RED_HIGH);

  // draw second hand
  len = r;
  endX = sin(second / 60.0 * M_PI * 2.0)*len;
  endY = -cos(second / 60.0 * M_PI * 2.0)*len;
    m->drawPixel(centerX+endX, centerY+endY, LED_WHITE_HIGH);
//  m->drawLine(centerX, centerY, centerX+endX, centerY+endY, LED_WHITE_HIGH);


  // draw am/pm indicator
  if (!clock.isAM()) {
    m->drawPixel(m->width()-1, m->height()-1, LED_RED_HIGH);
  }
}
