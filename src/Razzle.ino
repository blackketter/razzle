#include <Arduino.h>
#include <Switch.h>
#include <Encoder.h>
#include <TimeLib.h>

#include <myWifi.h>

#include "RazzleLeds.h"

#define BUTTON_PIN (D6)
Switch button = Switch(BUTTON_PIN);  // Switch between a digital pin and GND

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
  { "5C:CF:7F:18:EA:42", "RazzleButton",  1, RGB },
  { "5C:CF:7F:10:4C:43", "RazzleString", 50, RGB },
  { "5C:CF:7F:16:E6:EC", "RazzleBox",    64, GRB },
  { nullptr,             "RazzleUndef",   1, RGB }
};

devInfo getDevice() {
  int i = 0;
  do {
    if (strcasecmp(devices[i].mac, WiFi.macAddress().c_str()) == 0) {
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
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  digitalWrite(BUILTIN_LED, true);  // true = LED off

  Serial.begin(115200);
  while (! Serial); // Wait untilSerial is ready

  Serial.println("\nHello");
  delay(1000);
  Serial.println("World!");
  setupLeds(getDevice().colorOrder, getDevice().numLeds);

  setupWifi(getDevice().hostname, -7*60*60);
}

uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
const uint32_t holdTime = 1000;

void loop()
{
  static time_t lastTime = now();
  if (lastTime != now()) {
    Serial.println(now());
    lastTime = now();
  }

  button.poll();
  loopWifi();

  if (firstRun && button.on()) {
    recoverMode = true;
    Serial.println("RECOVER MODE");
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
      Serial.printf("Autoswitch to mode %d\n", getLedMode());
    } else {

      if (button.longPress()) {
        if (getLedMode() >= END) {
           setLedMode(FIRSTMODE);
        } else {
           setLedMode(OFF);
        }
        Serial.printf("Longpress, now mode: %d\n", getLedMode());
      }
      if (button.pushed()) {
        Serial.println("Button pushed");
        switch (getLedMode()) {
          case ON:
            setLedMode(OFF);
            Serial.println("Mode: Off");
            break;
          case OFF:
            setLedMode(ON);
            Serial.println("Mode: On");
            break;
          default:
            setLedMode(getLedMode()+1);
            if (getLedMode() >= END) {
              setLedMode(FIRSTMODE);
            }
            Serial.printf("Mode: %d\n", getLedMode());
        }
      } else if (button.released()) {
        Serial.println("Button released");
        switch (getLedMode()) {
          case OFF:
            Serial.println("Mode: Off");
            if (isRemote()) {
              httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=off&ph=0&");
            }
            break;
          case ON:
            setLedMode(ON);
            Serial.println("Mode: On");
            if (isRemote()) {
              httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=on&ph=0&");
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
