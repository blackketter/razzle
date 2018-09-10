#include <Arduino.h>
#include <Switch.h>
#include <WiFiThing.h>
#include <Clock.h>

#include "Credentials.h"
#include "RazzleLeds.h"
#include "Razzle.h"

// needed by platformio
#ifdef ESP8266
#include "ESP8266WiFi.h"
#endif
#ifdef ESP32
#include "WiFi.h"
#endif

#include "NTPClient.h"

#include "RazzleCommands.h"
#ifdef ESP8266
#define BUTTON_PIN (D6)
#endif

#ifdef ESP32
#define BUTTON_PIN (19)
#endif

Switch button = Switch(BUTTON_PIN);  // Switch between a digital pin and GND

WiFiThing thing;

bool firstRun = true;
bool recoverMode = false;


struct devInfo {
  const char* mac;
  const char* hostname;
  int numLeds;
  EOrder colorOrder;
  uint32_t powerSupplyMilliAmps;
};

devInfo devices[] = {
//  { "5C:CF:7F:C3:AD:F8", "RazzleButton",  1, RGB, 500 },
  { "5C:CF:7F:C3:AD:F8", "RazzleStrip",  600, GRB, 20000 },
  { "30:AE:A4:39:12:AC", "Razzle32",     600, GRB, 5000 },
  { "5C:CF:7F:10:4C:43", "RazzleString",  50, RGB, 2500 },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",     64, GRB, 1000 },
  { nullptr,             "RazzleUndef",    1, RGB, 0 }
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

bool isRemote() {
  return getDevice().numLeds == 1;
}

void recover() {
  console.debugln("Runtime Error, entering recovery mode");
  recoverMode = true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  digitalWrite(LED_BUILTIN, true);  // true = LED off

  delay(1000);

  thing.setHostname(getDevice().hostname);
  thing.begin(ssid, passphrase);
  console.debugf("Welcome to %s\n", getDevice().hostname);

  setupLeds(getDevice().colorOrder, getDevice().numLeds, getDevice().powerSupplyMilliAmps);
}

uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
const uint32_t holdTime = 1000;

void loop()
{
  static bool frameled = true;
  button.poll();
  thing.idle();
  theFPSCommand.idled();

  if (firstRun && button.on()) {
    recoverMode = true;
    console.debugln("BUTTON DOWN: RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button
  } else {
    // update display
    uint32_t nowMillis = millis();

    if ((nowMillis - lastModeSwitch()) > autoSwitchInterval && getLedMode() < ENDCOLORS) {
      setLedMode(getLedMode()+1);
      if (getLedMode() >= ENDCOLORS) {
        setLedMode(COLORS);
      }
      console.debugf("Autoswitch to mode %d\n", getLedMode());
    } else {

      if (button.longPress()) {
        if (getLedMode() >= ENDCOLORS) {
           setLedMode(COLORS);
        } else {
           setLedMode(WHITES);
        }
        console.debugf("Longpress, now mode: %d\n", getLedMode());
      }
      if (button.pushed()) {
        console.debugln("Button pushed");
        ledmode_t m = getLedMode();
        m = m+1;
        if (m == ENDCOLORS) { m = COLORS; };
        if (m == ENDWHITES) { m = WHITES; }
        setLedMode(m);
        console.printf("Mode: %d\n", getLedMode());
      } else if (button.released()) {
        console.debugln("Button released");
        switch (getLedMode()) {
          case OFF:
            console.debugln("Mode: Off");
            if (isRemote()) {
              thing.httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=off&ph=0&");
            }
            break;
          case ON:
            setLedMode(ON);
            console.debugln("Mode: On");
            if (isRemote()) {
              thing.httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=on&ph=0&");
            }
            break;
          default:
            break;
        }
      }
    }

    loopLeds();

    // toggle the built-in led
    frameled = !frameled;
    digitalWrite(LED_BUILTIN, frameled);
  }
  firstRun = false;
}


