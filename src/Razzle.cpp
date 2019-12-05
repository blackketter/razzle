#include <Arduino.h>
#include <Switch.h>
#include <Clock.h>

#include "Console.h"
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

#include "RazzleDevice.h"

Switch* pir;
Switch* button;

WiFiThing thing;

Clock theClock(&usPT);

bool isDay() {
  uint8_t hour = theClock.hour();
  return theClock.hasBeenSet() && (hour >= 8) && (hour < 17);  // daytime is 8 to 5.  if the clock hasn't been set, it's night for safety
}


bool firstRun = true;
bool recoverMode = false;
uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
//bool lastRadar = false;

void recover() {
  console.debugln("Runtime Error, entering recovery mode");
  recoverMode = true;
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  digitalWrite(LED_BUILTIN, true);  // true = LED off

  if (getDevice()->relayPin) {
    pinMode(getDevice()->relayPin, OUTPUT);
  }

  delay(1000);

  thing.setTimezone(&usPT);
  thing.setHostname(getDevice()->hostname);

  thing.begin(ssid, passphrase);
  console.debugf("Welcome to %s\n", getDevice()->hostname);

  console.debugf("LED is on pin %d\n",LED_BUILTIN);

  setBrightness(getDevice()->defaultDayBrightness, getDevice()->defaultNightBrightness);
  setupLeds();
  setNextLEDMode(true);
  if (getDevice()->pirPin) {
    pir = new Switch(getDevice()->pirPin, INPUT_PULLUP, LOW, 50, 1000);
    console.debugf("PIR is on pin %d\n",getDevice()->pirPin);
  }
  button = new Switch(BUTTON_PIN, BUTTON_INPUT, BUTTON_POLARITY, 50, 1000);  // Switch between a digital pin and GND
  console.debugf("button is on pin %d\n",BUTTON_PIN);
}

void tick() {
  static time_t last = 0;
  static uint32_t loops = 0;
  time_t curSec = Uptime::seconds();
  loops++;
  if (last != curSec) {
    console.debugf("tick - loops %d\n", loops);
    last = curSec;
    loops = 0;
  }
}

void loop()
{
  static bool frameled = true;

  button->poll();

  thing.idle();

  if (firstRun && button->on()) {
    recoverMode = true;
    console.debugln("BUTTON DOWN: RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button
  } else {

    // update display
    millis_t nowMillis = Uptime::millis();

    if (shouldAutoSwitch() && autoSwitchInterval && (nowMillis - lastModeSwitch()) > autoSwitchInterval) {
      setNextLEDMode(true);

      //console.executeCommandLine("info");
      //resetLastModeSwitch();

      console.debugf("Autoswitched to mode %s\n", getLEDMode());
    } else {
      if (button->longPress()) {
        setNextLEDModeSet();
        console.debugf("Long press, now mode: %s\n", getLEDMode());
      }
      if (button->pushedDuration() >  5000) {
        setLEDMode("Reboot");
      }
      if (button->pushed()) {
        setNextLEDMode();
        console.debugf("Short press, now mode: %s\n", getLEDMode());
      } else if (button->released()) {
        if (isLEDMode("Reboot")) {
          thing.reboot();
        }
/*
        switch (getLEDMode()) {
          case OFF:
            console.debugln("Mode: Off");
            if (isRemote()) {
              thing.httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=off&ph=0&");
            }
            break;
          case ON:
            setLEDMode(ON);
            console.debugln("Mode: On");
            if (isRemote()) {
              thing.httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=on&ph=0&");
            }
            break;
          default:
            break;
          }
*/
      }
    }

    if (pir) {
      pir->poll();

      if (pir->pushed()) {
        console.debugln("PIR low");
        if (getDevice()->relayPin) {
          digitalWrite(getDevice()->relayPin, true );
          console.debugln("relay on");
        }
        setLEDMode("OFF");
      }

      if (pir->released()) {
        console.debugln("PIR high");
        if (getDevice()->relayPin) {
          digitalWrite(getDevice()->relayPin, false );
          console.debugln("relay off");
        }
        setLEDMode("ON");
      }
    }

    loopLeds();

    // toggle the built-in led
    frameled = !frameled;
    digitalWrite(LED_BUILTIN, frameled);
/*
    bool radar = digitalRead(RADAR_PIN);
    if (lastRadar != radar) {
      lastRadar = radar;
      console.debugf("Radar: %s\n", radar ? "ON" : "OFF");
    }
*/
  }

  firstRun = false;
}
