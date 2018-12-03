#ifndef _CLOCKMODE_
#define _CLOCKMODE_
#include "RazzleMode.h"

class ClockMode : public RazzleMode {
  public:
    virtual bool wantsToRun() { return clock.hour() > 22 || clock.hour() < 7; }
  private:

};


#endif
