#include <Arduino.h>
#include <Switch.h>
#include <Clock.h>

#include "Credentials.h"
#include "Devices.h"
#include "WiFiThing.h"

#include <Console.h>
#include "Commands/DateCommand.h"

#include <RazzleMatrix.h>
#include <DefaultRazzleModes.h>

// needed by platformio
#ifdef ESP8266
#include "ESP8266WiFi.h"
#endif
#ifdef ESP32
#include "WiFi.h"
#endif

#include "NTPClient.h"

#include "DefaultRazzleModeSets.h"

Switch* pir;
Switch* button;

WiFiThing thing;
int modeIndex = 0;
int modeSetIndex = 0;
void setNextLEDMode(bool allowWants = false);
void setNextLEDModeSet();


bool firstRun = true;

bool recoverMode = false;
class RecoverCommand : public Command {
  public:
    const char* getName() override { return "recover"; }
    const char* getHelp() override { return "(1|0|) set or toggle recover mode"; }
    void execute(Console* c, uint8_t paramCount, char** params) override {
      if (paramCount == 1) {
        recoverMode = atoi(params[1]);
      } else {
        recoverMode = !recoverMode;
      }
      c->printf("Recover Mode: %s\n", recoverMode ? "on" : "off");
    }
  private:
};
RecoverCommand theRecoverCommand;

uint32_t autoSwitchInterval = 1000L * 5 * 60;

const razzleDevice* getDevice() {
  const razzleDevice* cacheDev = nullptr;

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

RazzleMatrix* matrix;
void setup() {

  delay(1000);

  if (getDevice()->relayPin) {
    pinMode(getDevice()->relayPin, OUTPUT);
  }

  thing.setTimezone(&usPT);
  thing.setHostname(getDevice()->hostname);
  thing.begin(ssid, passphrase);
  console.debugf("Welcome to %s\n", getDevice()->hostname);

  matrix = setupLeds(&(getDevice()->leds));

  if (matrix == nullptr) {
    recoverMode = true;
    console.debugln("setupLeds failed: recover mode on");
  } else {
    matrix->setBrightness(getDevice()->defaultDayBrightness, getDevice()->defaultNightBrightness);
    console.debugln("LEDs set up");

    button = new Switch(BUTTON_PIN, BUTTON_INPUT, BUTTON_POLARITY, 50, 1000);  // Switch between a digital pin and GND
    console.debugf("button is on pin %d\n",BUTTON_PIN);

    if (getDevice()->pirPin) {
      pir = new Switch(getDevice()->pirPin, INPUT_PULLUP, LOW, 50, 1000);
      console.debugf("PIR is on pin %d\n",getDevice()->pirPin);
    }

    setNextLEDMode(true);
    console.debugln("setup done");
  }

}

void loop() {

  button->poll();

  thing.idle();

  if (firstRun && button->on()) {
    recoverMode = true;
    console.debugln("BUTTON DOWN: RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button
  } else {

      if (button->longPress()) {
        setNextLEDModeSet();
        console.debugf("Long press, now mode: %s\n", matrix->getLEDMode());
      }
      if (button->pushedDuration() >  5000) {
        matrix->setLEDMode("Reboot");
      }
      if (button->pushed()) {
        setNextLEDMode();
        console.debugf("Short press, now mode: %s\n", matrix->getLEDMode());
      } else if (button->released()) {
        if (matrix->isLEDMode("Reboot")) {
          thing.reboot();
        }
/*
        switch (matrix->getLEDMode()) {
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

    if (pir) {
      pir->poll();

      if (pir->pushed()) {
        console.debugln("PIR low");
        if (getDevice()->relayPin) {
          digitalWrite(getDevice()->relayPin, true );
          console.debugln("relay on");
        }
        matrix->setLEDMode("OFF");
      }

      if (pir->released()) {
        console.debugln("PIR high");
        if (getDevice()->relayPin) {
          digitalWrite(getDevice()->relayPin, false );
          console.debugln("relay off");
        }
        matrix->setLEDMode("ON");
      }
    }

    matrix->idle();

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

///////////////////////////////////////////////////////////////////////////////
// Mode switching
///////////////////////////////////////////////////////////////////////////////
const char* colorModes[] =
{
  "Vortex",
  "Fire2",
  "Waterfall",
  "Analog",
  "Digital",
  "Word",
  "Lines",
  "Fire",
  "Life",
  "Breathing",
  "Wave",
  "Flashes",
  "Zipper",
  "RainbowRotate",
  "Noise",
  "WhiteNoise",
  "Cops",
  nullptr
};

const char* solidModes[] =
{
  "Off",
  "On",
  "Grey80",
  "Grey40",
  "Grey20",
  "Grey10",
  "Grey08",
  "Grey04",
  "Grey02",
  "Grey01",
  nullptr
};

const char** modeSets[] =
{
  colorModes,
  solidModes,
  nullptr
};

void setNextLEDMode(bool allowWants) {
	const char* nextModeName;
	RazzleMode* nextMode = nullptr;

	if (allowWants) {
		nextMode = RazzleMode::first();
		int wantsCount = 0;
		while (nextMode) {
			if (nextMode->canRun() && nextMode->wantsToRun())
				wantsCount++;;
			nextMode = nextMode->next();
		}
		if (wantsCount) {
			wantsCount = random(wantsCount);
			nextMode = RazzleMode::first();
			while (nextMode) {
				if (nextMode->canRun() && nextMode->wantsToRun()) {
					if (wantsCount == 0) {
						break;
					}
					wantsCount--;
				}
				nextMode = nextMode->next();
			}
		}
	}

	if (nextMode == nullptr) {
		do {
			modeIndex++;
			if (modeSets[modeSetIndex][modeIndex] == nullptr) {
				modeIndex = 0;
			}

			nextModeName = modeSets[modeSetIndex][modeIndex];
			nextMode = RazzleMode::named(nextModeName);

		} while (nextMode == nullptr || !nextMode->canRun());
	}
	matrix->setLEDMode(nextMode->name());
}

void setNextLEDModeSet() {
	modeSetIndex++;
	if (modeSets[modeSetIndex] == nullptr) {
		modeSetIndex = 0;
		}
	// only autoswitch if we are in the first (zeroth) set
	matrix->autoSwitchEnable(modeSetIndex == 0);
	modeIndex = 0;
	if (!matrix->setLEDMode(modeSets[modeSetIndex][modeIndex])) {
		setNextLEDMode();
	}
}


class NextCommand : public Command {
  public:
    const char* getName() { return "next"; }
    const char* getHelp() { return ("switch to next mode"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      setNextLEDMode(true);
      c->printf("Next LED Mode: %s\n", RazzleMode::defaultMatrix()->getLEDMode());
    }
};
NextCommand theNextCommand;

