#include "RazzleMode.h"

RazzleMode::RazzleMode() {
  RazzleMode* origFirst = _first;
  _first = this;
  _next = origFirst;
}

RazzleMode* RazzleMode::named(const char* find) {
  RazzleMode* r = first();
  while (r) {
    if (strcasecmp(r->name(), find) == 0) {
      break;
    }
    r = r->next();
  }
  return r;
}

RazzleMode* RazzleMode::_first = nullptr;
