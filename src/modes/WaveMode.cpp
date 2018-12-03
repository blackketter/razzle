#include "RazzleMode.h"

class WaveMode : public RazzleMode {
  public:
    virtual const char* name() { return "Wave"; }
    virtual void draw(CRGB* frame) {
      led_t pixels = numPixels();
      for (led_t i = 0; i < pixels; i++) {
        uint8_t y = (sin16(((int32_t)i * 65536/300 * (_freqout)) / pixels + _waveoff) + 32767) >> 8;
        frame[i] = white(y);
      }
      _freqout++;
      _waveoff += 100;

    };
  private:
    uint16_t _waveoff = 0;
    uint32_t _freqout = 1;
};

WaveMode theWaveMode;



