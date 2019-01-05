#ifndef _RazzleMode_
#define _RazzleMode_

#include "RazzleLeds.h"
#include "FastLED_NeoMatrix.h"
#include "RazzleDevice.h"

class RazzleMode {
  public:
    RazzleMode();

    virtual void draw(CRGB* frame) {}  // must implement one of the draw methods
    virtual void draw(FastLED_NeoMatrix* matrix) { draw(matrix->getLeds()); }

    virtual const char* name() = 0;

    virtual void begin( uint16_t w, uint16_t h) {}
    virtual void idle() {}
    virtual void end() {}

    virtual bool wantsToRun() { return false; }
    virtual bool canRun() { return true; }
    virtual bool dither() { return true; }

    virtual framerate_t fps() { return 0; } // as fast as possible

    RazzleMode* next() { return _next; };
    void setNext(RazzleMode* next) { _next = next; }
    static RazzleMode* first() { return _first; }
    static RazzleMode* named(const char* find);

  private:
    RazzleMode* _next = nullptr;
    static RazzleMode* _first;
};

#endif
