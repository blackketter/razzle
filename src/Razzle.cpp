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

#include "RazzleCommands.h"
#include "RazzleDevice.h"

#if defined(ESP8266)
#define BUTTON_PIN (D6)
#define BUTTON_POLARITY (LOW)
#define BUTTON_INPUT (INPUT_PULLUP)
//#define RADAR_PIN (D5)
#endif

#if defined(ESP32)
#define BUTTON_PIN (22)
#define BUTTON_POLARITY (HIGH)
#define BUTTON_INPUT (INPUT_PULLDOWN)
//#define RADAR_PIN (25)
#endif

Switch button = Switch(BUTTON_PIN, BUTTON_INPUT, BUTTON_POLARITY, 50, 1000);  // Switch between a digital pin and GND

WiFiThing thing;

Clock dayClock(&usPT);
bool isDay() {

  uint8_t hour = dayClock.hour();
  return dayClock.hasBeenSet() && (hour >= 8) && (hour < 20);  // daytime is 8 to 8.  if the clock hasn't been set, it's night for safety
}


bool firstRun = true;
bool recoverMode = false;
uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
const uint32_t holdTime = 1000;

//bool lastRadar = false;

void recover() {
  console.debugln("Runtime Error, entering recovery mode");
  recoverMode = true;
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  digitalWrite(LED_BUILTIN, true);  // true = LED off

//  pinMode(RADAR_PIN, INPUT);

  delay(1000);

  thing.setTimezone(&usPT);
  thing.setHostname(getDevice()->hostname);

  thing.begin(ssid, passphrase);
  console.debugf("Welcome to %s\n", getDevice()->hostname);

  console.debugf("LED is on pin %d\n",LED_BUILTIN);

  setBrightness(getDevice()->defaultDayBrightness, getDevice()->defaultNightBrightness);
  setupLeds();
  setNextLEDMode();
}

void loop()
{
  static bool frameled = true;
  button.poll();
  thing.idle();

  if (firstRun && button.on()) {
    recoverMode = true;
    console.debugln("BUTTON DOWN: RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button
  } else {
    // update display
    millis_t nowMillis = Uptime::millis();

    if (shouldAutoSwitch() && (nowMillis - lastModeSwitch()) > autoSwitchInterval) {
      setNextLEDMode();
      console.debugf("Autoswitch to mode %s\n", getLEDMode());
    } else {
      if (button.longPress()) {
        setNextLEDModeSet();
        console.debugf("Long press, now mode: %s\n", getLEDMode());
      }
      if (button.pushed()) {
        setNextLEDMode();
        console.debugf("Short press, now mode: %s\n", getLEDMode());
      } else if (button.released()) {
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
