#include "RazzleDevice.h"

devInfo devices[] = {

  { "5C:CF:7F:C3:B0:BD", "RazzleButton", 1,  1, RGB,  500, 128,  10 },
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",  1, 60, GRB, 2000, 128,  10 },
  { "B4:E6:2D:89:0D:C9", "MiniLoo",     12, 16, GRB, 2400,  32,   4 },
  { "30:AE:A4:39:12:AC", "PeggyLoo",    22, 30, GRB, 9000,  32,   4 },
  { "80:7D:3A:47:68:4D", "RazzleString", 2, 25, RGB,  500, 255,  10 },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",    8,  8, GRB, 1000, 128,  10 },
  { "5C:CF:7F:C3:B0:BD", "RazzleSensor", 1,  1, RGB,  500, 128,  10 },
  { nullptr,             "RazzleUndef",  1,  1, RGB,  500, 127,  10 }
};

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

led_t numPixels() {
  return getDevice().width * getDevice().height;
}

bool isRemote() {
  return numPixels() == 1;
}

