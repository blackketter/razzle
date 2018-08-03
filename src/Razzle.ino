#include <Arduino.h>
#include <Switch.h>
#include <Encoder.h>
#include <WiFiThing.h>
#include <Clock.h>

#include "Credentials.h"
#include "RazzleLeds.h"

// needed by platformio
#include "ESP8266WiFi.h"
#include "NTPClient.h"

#define BUTTON_PIN (D6)
Switch button = Switch(BUTTON_PIN);  // Switch between a digital pin and GND

WiFiThing thing;

#define ENCODER_A_PIN (D7)
#define ENCODER_B_PIN (D8)
Encoder knob(ENCODER_A_PIN,ENCODER_B_PIN);

bool firstRun = true;
bool recoverMode = false;

struct devInfo {
  const char* mac;
  const char* hostname;
  int numLeds;
  EOrder colorOrder;
};

devInfo devices[] = {
  { "5C:CF:7F:C3:AD:F8", "RazzleButton",  1, RGB },
  { "5C:CF:7F:10:4C:43", "RazzleString", 50, RGB },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",    64, GRB },
  { nullptr,             "RazzleUndef",   1, RGB }
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


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  digitalWrite(LED_BUILTIN, true);  // true = LED off

  delay(1000);
  setupLeds(getDevice().colorOrder, getDevice().numLeds);

  thing.begin(ssid, passphrase);
  thing.setHostname(getDevice().hostname);
  Serial.println("setup");
}

uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
const uint32_t holdTime = 1000;

void loop()
{
  button.poll();
  thing.idle();

  if (firstRun && button.on()) {
    recoverMode = true;
    console.debugln("RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button
  } else {
    // update display
    uint32_t nowMillis = millis();

    if ((nowMillis - lastModeSwitch()) > autoSwitchInterval && getLedMode() < END) {
      setLedMode(getLedMode()+1);
      if (getLedMode() >= END) {
        setLedMode(FIRSTMODE);
      }
      console.debugf("Autoswitch to mode %d\n", getLedMode());
    } else {

      if (button.longPress()) {
        if (getLedMode() >= END) {
           setLedMode(FIRSTMODE);
        } else {
           setLedMode(OFF);
        }
        console.debugf("Longpress, now mode: %d\n", getLedMode());
      }
      if (button.pushed()) {
        console.debugln("Button pushed");
        switch (getLedMode()) {
          case ON:
            setLedMode(OFF);
            console.debugln("Mode: Off");
            break;
          case OFF:
            setLedMode(ON);
            console.debugln("Mode: On");
            break;
          default:
            setLedMode(getLedMode()+1);
            if (getLedMode() >= END) {
              setLedMode(FIRSTMODE);
            }
            console.printf("Mode: %d\n", getLedMode());
        }
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
  }
  firstRun = false;
}
