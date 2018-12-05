#include "RazzleDevice.h"

const devInfo devices[] = {
//                                  dimensions  color max   brightness   pins     led
//  MAC address           Name           w   h  order mA     day night relay  pir  segments
  { "5C:CF:7F:C3:AD:DC", "RazzleButton", 1,  1, RGB,  500,   128,  10,     0,  0,  {1} },
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",  1, 60, GRB, 2000,   128,  10,     0,  0,  {60} },
  { "B4:E6:2D:89:0D:C9", "MiniLoo",     12, 16, GRB, 2400,    32,   4,     0,  0,  {32,32,32,32,32,32} },
  { "30:AE:A4:39:12:AC", "PeggyLoo",    22, 30, GRB, 9000,    32,   4,     0,  0,  {120,120,120,120,120,60} },
  { "80:7D:3A:47:68:4D", "RazzleString", 2, 25, RGB,  500,   255,  10,     0,  0,  {50} },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",    8,  8, GRB, 1000,   128,  10,     0,  0,  {64} },
  { "84:F3:EB:3B:7E:1C", "RazzlePir",    1, 12, GRB,  500,   128,  10,    D1, D5,  {12} },
  { "84:F3:EB:3B:7A:9E", "RazzleStar",   1,  7, GRB,  500,   128,  10,     0,  0,  {12} },
  { nullptr,             "RazzleUndef",  1,  1, RGB,  500,   127,  10,     0,  0,  {1} }
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

led_t maxSegmentLen() {
  static led_t maxSegmentLenCache = 0;
  if (maxSegmentLenCache == 0) {
    for (int i = 0; i < MAX_SEGMENTS; i++) {
      led_t len = getDevice()->segment[i];
      if (len > maxSegmentLenCache) {
        maxSegmentLenCache = len;
      }
    }
  }

  return maxSegmentLenCache;
}
