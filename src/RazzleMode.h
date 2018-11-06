#ifndef _RazzleMode_
#define _RazzleMode_

#include "FastLED.h"
#include "RazzleDevice.h"
#include "RazzleLeds.h"

class RazzleMode {
  public:
    RazzleMode();
    virtual void draw(CRGB* frame) = 0;
    virtual const char* name() = 0;

    virtual void begin() {};
    virtual void idle() {};
    virtual void end() {};

    virtual bool wantsToRun() { return false; }
    virtual bool canRun() { return true; }

    virtual framerate_t fps() { return 1000; } // as fast as possible

    RazzleMode* next() { return _next; };
    void setNext(RazzleMode* next) { _next = next; }
    static RazzleMode* first() { return _first; }
    static RazzleMode* named(const char* find);
  private:
    RazzleMode* _next = nullptr;
    static RazzleMode* _first;
};

#endif
