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


#include "sunMoon.h"
#include "TimeLord.h"

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

sunMoon  sm;
Clock gmtClock;
#define OUR_latitude    -122.4438               // San Francisco cordinates
#define OUR_longtitude    37.7619
#define OUR_timezone    0 // -(8*60)                     // localtime with UTC difference in minutes

bool isSolarDay() {
  bool isDay = false;
  time_t nowUTC = gmtClock.now();
  time_t sRise = sm.sunRise(nowUTC);
  time_t sSet  = sm.sunSet(nowUTC);
  console.debugf("now:%d rise:%d set:%d\n", nowUTC,sRise,sSet);

  if (nowUTC > sRise && nowUTC < sSet) {
    console.debugln("Day!");
    isDay = true;
  } else {
    isDay = false;
    console.debugln("Night!");
  }

  TimeLord tardis;
  tardis.TimeZone(-8 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST

  // as long as the RTC never changes back and forth between DST and non-DST

  tardis.Position(OUR_latitude, OUR_longtitude); // tell TimeLord where in the world we are

  uint8_t todays[] = { 0, 0, 12, 27, 9, 18 }; // store today's date (at noon) in an array for TimeLord to use

if (tardis.SunRise(todays)) {// if the sun will rise today (it might not, in the [ant]arctic)
   {
   console.print("Sunrise: ");
   console.print((int) todays[tl_hour]);
   console.print(":");
   console.println((int) todays[tl_minute]);
   }

   if (tardis.SunSet(todays)) // if the sun will set todays (it might not, in the [ant]arctic)
   {
   console.print("Sunset: ");
   console.print((int) todays[tl_hour]);
   console.print(":");
   console.println((int) todays[tl_minute]);
   }
   console.println();
  }

  return isDay;
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
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);

  gmtClock.setZone(&UTC);

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
  setupLeds(getDevice()->colorOrder, numPixels(), getDevice()->powerSupplyMilliAmps);
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
    uint32_t nowMillis = millis();

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

    // once a second
    static time_t lastSec = 0;
    time_t sec = gmtClock.now();
    if (sec != lastSec) {
      lastSec = sec;
    }

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
