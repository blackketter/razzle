#include "RazzleLeds.h"
#include <colorutils.h>
#include <Clock.h>
#include "Razzle.h"
#include "RazzleDevice.h"
//#include "GFX_Buffer.h"

#include "Commands/FPSCommand.h"
FPSCommand theFPSCommand;

#include "RazzleMode.h"
#include "RazzleModes.h"

#include "FastLED_NeoMatrix.h"

led_t num_leds;

CRGB* leds;

CRGB* frames[2];
//GFX_Buffer* frame2x;

RazzleMode* currMode = nullptr;

millis_t lastFrameMillis = 0;
uint8_t nextFrame = 1;

millis_t nextFrameMillis = 1;
uint8_t lastFrame = 0;
const millis_t defaultFrameInterval = 0;  // as fast as possible
millis_t frameIntervalMillis = defaultFrameInterval;

int modeIndex = 0;
int modeSetIndex = 0;

FastLED_NeoMatrix *matrix;

CRGB white(uint8_t y) {
  uint32_t y32 = y;
  return y32 + (y32 << 8) + (y32 << 16);
}


inline void fps(framerate_t f)  { frameIntervalMillis = f ? 1000/f : 0; };

millis_t nowMillis = 0;
millis_t lastModeSwitchTime = 0;
millis_t lastModeSwitch() { return lastModeSwitchTime; }
void resetLastModeSwitch() { lastModeSwitchTime = Uptime::millis(); }

uint8_t dayBrightness = 128;
uint8_t nightBrightness = 10;
uint8_t getBrightness() { return isDay() ? dayBrightness : nightBrightness; }
uint8_t getNightBrightness() { return nightBrightness; }
uint8_t getDayBrightness() { return dayBrightness; }
void setBrightness(uint8_t day, uint8_t night) { dayBrightness = day; nightBrightness = night; }

void  setupLeds() {

	uint32_t milliAmpsMax =  getDevice()->powerSupplyMilliAmps;
	EOrder order = getDevice()->colorOrder;
 	num_leds = numPixels();

  leds = new CRGB[num_leds];
  frames[0] = new CRGB[num_leds];
  frames[1] = new CRGB[num_leds];
//	frame2x = new GFX_Buffer(getDevice()->width*2, getDevice()->height*2);

  fill_solid(leds, num_leds, CRGB::Black);
  fill_solid(frames[0], num_leds, CRGB::Black);
  fill_solid(frames[1], num_leds, CRGB::Black);

  if (!leds || !frames[0] || !frames[1]) {
    recover();
    return;
  }
	led_t offset = 0;
	led_t c;
	int i = 0;

  switch (order) {
    case RGB:
    	// fucking templates mean that this needs to be hardcoded
    	c = getDevice()->segment[i++];
    	if (c) {
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
				offset += c;
				c = getDevice()->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
					offset += c;
					c = getDevice()->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
						offset += c;
						c = getDevice()->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
							offset += c;
							c = getDevice()->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
								offset += c;
								c = getDevice()->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
									offset += c;
									c = getDevice()->segment[i++];
									if (c) {
										c = getDevice()->segment[i++];
										FastLED.addLeds<CHIPSET, LED_DATA_PIN6, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
										offset += c;
										if (c) {
											FastLED.addLeds<CHIPSET, LED_DATA_PIN7, RGB>(leds, offset, c).setCorrection( TypicalLEDStrip );
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
    	c = getDevice()->segment[i++];
    	if (c) {
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
				offset += c;
				c = getDevice()->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
					offset += c;
					c = getDevice()->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
						offset += c;
						c = getDevice()->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
							offset += c;
							c = getDevice()->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
								offset += c;
								c = getDevice()->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
									offset += c;
									c = getDevice()->segment[i++];
									if (c) {
										FastLED.addLeds<CHIPSET, LED_DATA_PIN6, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
										offset += c;
										c = getDevice()->segment[i++];
										if (c) {
											FastLED.addLeds<CHIPSET, LED_DATA_PIN7, GRB>(leds, offset, c).setCorrection( TypicalLEDStrip );
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

  FastLED.setCorrection(UncorrectedColor);
  FastLED.setTemperature(UncorrectedTemperature);
  FastLED.show(getBrightness());

  matrix = new FastLED_NeoMatrix(leds, getDevice()->width, getDevice()->height,
  	NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
}


void interpolateFrame() {
	uint8_t fract8;
	if (nextFrameMillis == lastFrameMillis) {
		fract8 = 0;
	} else {
  	fract8 = ((nowMillis - lastFrameMillis) * 256) / (nextFrameMillis - lastFrameMillis);
  }

  if (fract8 == 0) {
    memmove(leds, frames[lastFrame], num_leds * sizeof(CRGB));
  } else {
    for (led_t i = 0; i < num_leds; i++) {
      leds[i].r = lerp8by8( frames[lastFrame][i].r, frames[nextFrame][i].r, fract8 );
      leds[i].g = lerp8by8( frames[lastFrame][i].g, frames[nextFrame][i].g, fract8 );
      leds[i].b = lerp8by8( frames[lastFrame][i].b, frames[nextFrame][i].b, fract8 );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void render(CRGB* frame) {
	if (currMode) {
		fps(currMode->fps());
		matrix->setLeds(frame);
		currMode->draw(matrix);
	} else {
		console.debugln("No currMode!");
	}
}

const char* getLEDMode() {
	return currMode->name();
}

bool setLEDMode(const char* newMode) {
//	console.debugln("setledmode");
  if (newMode == nullptr) return false;
  RazzleMode* namedMode = RazzleMode::named(newMode);
  if (namedMode == nullptr) return false;
  if (!namedMode->canRun()) return false;
  if (currMode == namedMode) return true;

//	console.debugln("resetLastModeSwitch");
	resetLastModeSwitch();

//	console.debugln("end");
  if (currMode) { currMode->end(); }
  currMode = namedMode;  // 120 led long string is about 100fps, dithering at about 50fps

//	console.debugln("setDither");
  if (maxSegmentLen() > 120) {
    FastLED.setDither( 0 );
  } else {
  	FastLED.setDither(currMode->dither());
  }
//	console.debugln("begin");
	currMode->begin();
  lastFrame = 0;
  lastFrameMillis = nowMillis;
//	console.debugln("fill_solid");
  fill_solid( frames[lastFrame], num_leds, CRGB::Black);
  render(frames[lastFrame]);

//	console.debugln("nextFrameMillis");
  nextFrame = 1;
  nextFrameMillis = nowMillis + frameIntervalMillis;
//	console.debugln("fill_solid");
  fill_solid( frames[nextFrame], num_leds, CRGB::Black);
//	console.debugln("render");
  render(frames[nextFrame]);

  return true;
}

bool isLEDMode(const char* isMode) {
	return strcasecmp(getLEDMode(), isMode) == 0;
}

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

	setLEDMode(nextMode->name());
}

void setNextLEDModeSet() {
	modeSetIndex++;
	if (modeSets[modeSetIndex] == nullptr) {
		modeSetIndex = 0;
	}
	modeIndex = 0;
	if (!setLEDMode(modeSets[modeSetIndex][modeIndex])) {
		setNextLEDMode();
	}
}

bool shouldAutoSwitch() {
	return modeSetIndex == 0;
}

void loopLeds() {
  nowMillis = Uptime::millis();
  if (nowMillis >= nextFrameMillis) {
    uint8_t temp = lastFrame;
    lastFrame = nextFrame;
    nextFrame = temp;
    lastFrameMillis = nextFrameMillis;
    nextFrameMillis = nowMillis + frameIntervalMillis;
    memmove(frames[nextFrame], frames[lastFrame], num_leds * sizeof(CRGB));

    render(frames[nextFrame]);
  }

	interpolateFrame();
  FastLED.show(getBrightness());
  theFPSCommand.newFrame();
}
