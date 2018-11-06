#include "RazzleMode.h"

class BreathingMode : public RazzleMode {
  public:
    virtual const char* name() { return "Breathing"; }
    virtual void draw(CRGB* frame) {

      uint32_t brightness = sin16((millis()*10)%65536)+32768; // approx 13s cycle time
      uint8_t scaled = (brightness * brightness)/(65536L*256);

      CRGB c;
      c.setRGB(scaled, scaled, scaled);
      fill_solid( frame, numPixels(), c);
    }
  private:
};

BreathingMode theBreathingMode;
