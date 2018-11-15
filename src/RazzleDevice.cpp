#include "RazzleDevice.h"

const devInfo devices[] = {

  { "5C:CF:7F:C3:AD:DC", "RazzleButton", 1,  1, RGB,  500, 128,  10, {1} },
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",  1, 60, GRB, 2000, 128,  10, {60} },
  { "B4:E6:2D:89:0D:C9", "MiniLoo",     12, 16, GRB, 2400,  32,   4, {32,32,32,32,32,32} },
  { "30:AE:A4:39:12:AC", "PeggyLoo",    22, 30, GRB, 9000,  32,   4, {120,120,120,120,120,60} },
  { "80:7D:3A:47:68:4D", "RazzleString", 2, 25, RGB,  500, 255,  10, {50} },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",    8,  8, GRB, 1000, 128,  10, {64} },
  { "5C:CF:7F:C3:B0:BD", "RazzleSensor", 1,  1, RGB,  500, 128,  10, {1} },
  { nullptr,             "RazzleUndef",  1,  1, RGB,  500, 127,  10, {1} }
};

const devInfo* getDevice() {
  static devInfo const* cacheDev = nullptr;

  if (cacheDev) { return cacheDev; }

  int i = 0;
  do {
    if (strcasecmp(devices[i].mac, thing.getMacAddress().c_str()) == 0) {
      break;
    }
    i++;
  } while ( devices[i].mac != nullptr );
  cacheDev = &devices[i];
  return cacheDev;  // the unmatched default is returned
}

led_t numPixels() {
  // memoize the number of pixels
  static int32_t cachePixels = -1;
  if (cachePixels < 0) {
    cachePixels = getDevice()->width * getDevice()->height;
  }
  return cachePixels;
}

bool isRemote() {
  return numPixels() == 1;
}

