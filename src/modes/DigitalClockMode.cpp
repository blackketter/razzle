#include "ClockMode.h"

class DigitalClockMode : public ClockMode {
  public:
    virtual const char* name() { return "Digital"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 0; }
    virtual bool canRun() { return getDevice()->width > 10 && getDevice()->height > 10 && clock.hasBeenSet(); }
    virtual bool dither() { return false; }
  private:

};

DigitalClockMode theDigitalClockMode;

// here we go
void DigitalClockMode::draw(FastLED_NeoMatrix* m) {

  m->fillScreen(LED_BLACK);

  pixel_t centerX = m->width()/2;
  pixel_t centerY = m->height()/2;

  int size = m->width()/10;

  CRGB c = ColorFromPalette( HeatColors_p, random(16,240));
  if ( clock.hourFormat12() > 9) {
    m->drawChar(0, 0, (clock.hourFormat12() / 10)+'0', m->Color(c), LED_BLACK, size);
  }

  char d = (clock.hourFormat12() % 10) + '0';
  if (d == '0') d = 'O';
  c = ColorFromPalette( HeatColors_p, random(16,240));
  m->drawChar(centerX, 0, d, m->Color(c), LED_BLACK, size);

  d = (clock.minute()/10)+'0';
  if (d == '0') d = 'O';
  c = ColorFromPalette( HeatColors_p, random(16,240));
  m->drawChar(0, centerY, d, m->Color(c), LED_BLACK, size);

  d = (clock.minute()%10)+'0';
  if (d == '0') d = 'O';
  c = ColorFromPalette( HeatColors_p, random(16,240));
  m->drawChar(centerX, centerY, d, m->Color(c), LED_BLACK, size);
}
