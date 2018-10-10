#ifndef _RazzleDevice_
#define _RazzleDevice_
#include "Razzle.h"

struct devInfo {
  const char* mac;
  const char* hostname;
  int numLeds;
  EOrder colorOrder;
  uint32_t powerSupplyMilliAmps;
  uint8_t defaultDayBrightness;
  uint8_t defaultNightBrightness;
};

devInfo devices[] = {

//{ "5C:CF:7F:C3:AD:F8", "RazzleButton",   1, RGB,  500, 128,  10 },
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",   60, GRB, 2000, 128,  10 },
  { "B4:E6:2D:89:0D:C9", "MiniLoo",    12*16, GRB, 2400,  32,   4 },
  { "30:AE:A4:39:12:AC", "PeggyLoo",   22*30, GRB, 9000,  32,   4 },
  { "80:7D:3A:47:68:4D", "RazzleString",  50, RGB,  500, 255,  10 },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",     64, GRB, 1000, 128,  10 },
  { nullptr,             "RazzleUndef",    1, RGB,    0, 127,  10 }
};

// other devices
// 5C:CF:7F:10:4C:43 - Wemos D1 (full size)

devInfo getDevice() {
  int i = 0;
  do {
    if (strcasecmp(devices[i].mac, thing.getMacAddress().c_str()) == 0) {
      return devices[i];
    }
    i++;
  } while ( devices[i].mac != nullptr );
  return devices[i];  // the unmatched default is returned
}

bool isRemote() {
  return getDevice().numLeds == 1;
}

#endif