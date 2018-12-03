#include "ClockMode.h"

class OneDClockMode : public ClockMode {
  public:
    virtual const char* name() { return "1dClock"; }
    virtual void draw(CRGB* leds);
    virtual framerate_t fps() { return 1; }
    virtual bool canRun() { return  getDevice()->height >= 12 && clock.hasBeenSet(); }
  private:

};

OneDClockMode theOneDClockMode;

void OneDClockMode::draw(CRGB* leds) {

}
