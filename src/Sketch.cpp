#include <Arduino.h>
#include <Switch.h>
#include <Clock.h>

#include "Credentials.h"
#include "Devices.h"
#include "WiFiThing.h"

#include <Console.h>

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

uint32_t autoSwitchInterval = 1000L * 5 * 60;
//bool lastRadar = false;

const LEDColorCorrection defaultCorrection = TypicalSMD5050; // TypicalLEDStrip;
const ColorTemperature defaultTemperature = DirectSunlight;

///////////////////////////////////////////////////////////////////////////////
// setupLeds
///////////////////////////////////////////////////////////////////////////////

extern led_t num_leds;
extern RazzleMatrix* frames[2];
extern const LEDColorCorrection defaultCorrection;
extern CRGB* leds;
extern const ColorTemperature defaultTemperature;
extern FastLED_NeoMatrix *matrix;

bool setupLeds(const RazzleMatrixConfig* info) {
  setLEDMatrix(info);
	uint32_t milliAmpsMax =  info->powerSupplyMilliAmps;
	EOrder order = info->colorOrder;
 	num_leds = info->width*info->height;

  leds = new CRGB[num_leds];
  CRGB* frameled0 = new CRGB[num_leds];
  CRGB* frameled1 = new CRGB[num_leds];
  frames[0] = new RazzleMatrix(frameled0, info->width, info->height, info->matrixType);
  frames[1] = new RazzleMatrix(frameled1, info->width, info->height, info->matrixType);

  if (!leds || !frameled0 || !frameled1 || !frames[0] || !frames[1]) {
    console.debugln("Error allocating LED buffers");
    recoverMode = true;
    return false;
  }
  fill_solid(leds, num_leds, CRGB::Black);
  frames[0]->fillScreen(LED_BLACK);
  frames[1]->fillScreen(LED_BLACK);

	led_t offset = 0;
	led_t c;
	int i = 0;

  switch (order) {
    case RGB:
    	// fucking templates mean that this needs to be hardcoded
    	c = info->segment[i++];
    	if (c) {
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, RGB>(leds, offset, c).setCorrection( defaultCorrection );
				offset += c;
				c = info->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, RGB>(leds, offset, c).setCorrection( defaultCorrection );
					offset += c;
					c = info->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, RGB>(leds, offset, c).setCorrection( defaultCorrection );
						offset += c;
						c = info->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, RGB>(leds, offset, c).setCorrection( defaultCorrection );
							offset += c;
							c = info->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, RGB>(leds, offset, c).setCorrection( defaultCorrection );
								offset += c;
								c = info->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, RGB>(leds, offset, c).setCorrection( defaultCorrection );
									offset += c;
									c = info->segment[i++];
									if (c) {
										c = info->segment[i++];
										FastLED.addLeds<CHIPSET, LED_DATA_PIN6, RGB>(leds, offset, c).setCorrection( defaultCorrection );
										offset += c;
										if (c) {
											FastLED.addLeds<CHIPSET, LED_DATA_PIN7, RGB>(leds, offset, c).setCorrection( defaultCorrection );
    								}
    							}
    						}
    					}
    				}
    			}
    		}
    	}
      break;
    case GRB:
    	// fucking templates mean that this needs to be hardcoded
    	c = info->segment[i++];
    	if (c) {
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, GRB>(leds, offset, c).setCorrection( defaultCorrection );
				offset += c;
				c = info->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, GRB>(leds, offset, c).setCorrection( defaultCorrection );
					offset += c;
					c = info->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, GRB>(leds, offset, c).setCorrection( defaultCorrection );
						offset += c;
						c = info->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, GRB>(leds, offset, c).setCorrection( defaultCorrection );
							offset += c;
							c = info->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, GRB>(leds, offset, c).setCorrection( defaultCorrection );
								offset += c;
								c = info->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, GRB>(leds, offset, c).setCorrection( defaultCorrection );
									offset += c;
									c = info->segment[i++];
									if (c) {
										FastLED.addLeds<CHIPSET, LED_DATA_PIN6, GRB>(leds, offset, c).setCorrection( defaultCorrection );
										offset += c;
										c = info->segment[i++];
										if (c) {
											FastLED.addLeds<CHIPSET, LED_DATA_PIN7, GRB>(leds, offset, c).setCorrection( defaultCorrection );
    								}
    							}
    						}
    					}
    				}
    			}
    		}
    	}
      break;
    default:
      console.debugln("ERROR: Bad color order");
  }

  FastLED.setMaxPowerInVoltsAndMilliamps	( 5, milliAmpsMax);

  FastLED.setCorrection(defaultCorrection);
  FastLED.setTemperature(defaultTemperature);
  FastLED.show(getBrightness());

  matrix = new FastLED_NeoMatrix(leds, info->width, info->height, info->matrixType);
  return true;
}

void setup() {

  if (getDevice()->relayPin) {
    pinMode(getDevice()->relayPin, OUTPUT);
  }

  delay(1000);

  thing.setTimezone(&usPT);
  thing.setHostname(getDevice()->hostname);

  thing.begin(ssid, passphrase);
  console.debugf("Welcome to %s\n", getDevice()->hostname);

  setBrightness(getDevice()->defaultDayBrightness, getDevice()->defaultNightBrightness);
  if (!setupLeds(&(getDevice()->leds))) {
    recoverMode = true;
    console.debugln("setupLeds failed: recover mode on");
  }

  RazzleMode::defaultMatrix(frames[0]);

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
