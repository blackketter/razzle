#ifndef _Devices_
#define _Devices_
#include <RazzleMatrix.h>

#ifdef ESP8266
#define BUTTON_PIN (D6)
#define BUTTON_POLARITY (LOW)
#define BUTTON_INPUT (INPUT_PULLUP)
#endif

#if defined(ESP32)
#define BUTTON_PIN (22)
#define BUTTON_POLARITY (HIGH)
#define BUTTON_INPUT (INPUT_PULLDOWN)

// esp32 doesn't use Dn for digital pin constants
#define D1 1
#define D4 4
#define D5 5

#endif


struct razzleDevice {
  const char* mac;
  const char* hostname;
  uint8_t relayPin;
  uint8_t pirPin;
  uint8_t defaultDayBrightness;
  uint8_t defaultNightBrightness;
  RazzleMatrixConfig leds;
};

#define PEGGYMATRIXTYPE (NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)

const razzleDevice devices[] = {
//                                        relay  pir brightness    dims  color max   matrix           leds per
//  MAC address           Name              pin  pin  day night   w   h  order mA    type             pin
  { "5C:CF:7F:C3:AD:DC", "RazzleButton",      0,  0,  128,  10, { 1,  1, GRB,  500,                0, {1} }},
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",       0,  0,  128,  10, { 1, 60, GRB, 2000,  PEGGYMATRIXTYPE, {60} }},
  { "B4:E6:2D:89:0D:C9", "MiniLoo",           0,  0,   32,   4, { 12,16, GRB, 2400,  PEGGYMATRIXTYPE, {32,32,32,32,32,32} }},
  { "30:AE:A4:39:12:AC", "PeggyLoo",          0,  0,   32,   4, { 22,30, GRB, 9000,  PEGGYMATRIXTYPE, {120,120,120,120,120,60} }},
  { "80:7D:3A:47:68:4D", "RazzleString",      0,  0,  255,  10, { 2, 25, RGB,  500,  PEGGYMATRIXTYPE, {50} }},
  { "5C:CF:7F:16:E6:EC", "RazzleBox",         0,  0,  128,  10, { 8,  8, GRB, 1000,                0, {64} }},
  { "84:F3:EB:3B:7E:1C", "RazzlePir",        D1, D5,  128,  10, { 1, 12, GRB,  500,                0, {12} }},
  { "84:F3:EB:3B:7A:9E", "RazzleStar",        0,  0,  128,  10, { 1,  7, GRB,  500,                0, {12} }},
  { "80:7D:3A:47:6B:02", "RazzleSquare",      0,  0,  128,  10, { 8,  8, GRB,  500,                0, {64} }},
  { "30:AE:A4:39:18:A0", "RazzleRoof",        0,  0,  128,  10, { 75, 2, RGB, 1000,  PEGGYMATRIXTYPE, {75,75,75,75,75,75,75,75,75,75} }},
  { nullptr,             "RazzleUndef",       0,  0,  127,  10, { 1,  1, RGB,  500,                0, {1} }}
};

// other devices
// 5C:CF:7F:10:4C:43 - Wemos D1 (full size)



#endif
