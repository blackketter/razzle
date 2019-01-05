#include "RazzleLeds.h"
#include <colorutils.h>
#include <Clock.h>
#include "Razzle.h"
#include "RazzleDevice.h"
//#include "GFX_Buffer.h"

#include "Commands/FPSCommand.h"
FPSCommand theFPSCommand;

const LEDColorCorrection defaultCorrection = TypicalSMD5050; // TypicalLEDStrip;
const ColorTemperature defaultTemperature = DirectSunlight;

#include "RazzleMode.h"
#include "RazzleModes.h"

#include "FastLED_NeoMatrix.h"
// gO is gamma offset
const uint8_t gO = 1;
const uint8_t gamma8[] = {
       0,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,
    0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  0+gO,  1+gO,  1+gO,  1+gO,  1+gO,
    1+gO,  1+gO,  1+gO,  1+gO,  1+gO,  1+gO,  1+gO,  1+gO,  1+gO,  2+gO,  2+gO,  2+gO,  2+gO,  2+gO,  2+gO,  2+gO,
    2+gO,  3+gO,  3+gO,  3+gO,  3+gO,  3+gO,  3+gO,  3+gO,  4+gO,  4+gO,  4+gO,  4+gO,  4+gO,  5+gO,  5+gO,  5+gO,
    5+gO,  6+gO,  6+gO,  6+gO,  6+gO,  7+gO,  7+gO,  7+gO,  7+gO,  8+gO,  8+gO,  8+gO,  9+gO,  9+gO,  9+gO, 10+gO,
   10+gO, 10+gO, 11+gO, 11+gO, 11+gO, 12+gO, 12+gO, 13+gO, 13+gO, 13+gO, 14+gO, 14+gO, 15+gO, 15+gO, 16+gO, 16+gO,
   17+gO, 17+gO, 18+gO, 18+gO, 19+gO, 19+gO, 20+gO, 20+gO, 21+gO, 21+gO, 22+gO, 22+gO, 23+gO, 24+gO, 24+gO, 25+gO,
   25+gO, 26+gO, 27+gO, 27+gO, 28+gO, 29+gO, 29+gO, 30+gO, 31+gO, 32+gO, 32+gO, 33+gO, 34+gO, 35+gO, 35+gO, 36+gO,
   37+gO, 38+gO, 39+gO, 39+gO, 40+gO, 41+gO, 42+gO, 43+gO, 44+gO, 45+gO, 46+gO, 47+gO, 48+gO, 49+gO, 50+gO, 50+gO,
   51+gO, 52+gO, 54+gO, 55+gO, 56+gO, 57+gO, 58+gO, 59+gO, 60+gO, 61+gO, 62+gO, 63+gO, 64+gO, 66+gO, 67+gO, 68+gO,
   69+gO, 70+gO, 72+gO, 73+gO, 74+gO, 75+gO, 77+gO, 78+gO, 79+gO, 81+gO, 82+gO, 83+gO, 85+gO, 86+gO, 87+gO, 89+gO,
   90+gO, 92+gO, 93+gO, 95+gO, 96+gO, 98+gO, 99+gO,101+gO,102+gO,104+gO,105+gO,107+gO,109+gO,110+gO,112+gO,114+gO,
  115+gO,117+gO,119+gO,120+gO,122+gO,124+gO,126+gO,127+gO,129+gO,131+gO,133+gO,135+gO,137+gO,138+gO,140+gO,142+gO,
  144+gO,146+gO,148+gO,150+gO,152+gO,154+gO,156+gO,158+gO,160+gO,162+gO,164+gO,167+gO,169+gO,171+gO,173+gO,175+gO,
  177+gO,180+gO,182+gO,184+gO,186+gO,189+gO,191+gO,193+gO,196+gO,198+gO,200+gO,203+gO,205+gO,208+gO,210+gO,213+gO,
  215+gO,218+gO,220+gO,223+gO,225+gO,228+gO,231+gO,233+gO,236+gO,239+gO,241+gO,244+gO,247+gO,249+gO,252+gO,   255 };


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
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, RGB>(leds, offset, c).setCorrection( defaultCorrection );
				offset += c;
				c = getDevice()->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, RGB>(leds, offset, c).setCorrection( defaultCorrection );
					offset += c;
					c = getDevice()->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, RGB>(leds, offset, c).setCorrection( defaultCorrection );
						offset += c;
						c = getDevice()->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, RGB>(leds, offset, c).setCorrection( defaultCorrection );
							offset += c;
							c = getDevice()->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, RGB>(leds, offset, c).setCorrection( defaultCorrection );
								offset += c;
								c = getDevice()->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, RGB>(leds, offset, c).setCorrection( defaultCorrection );
									offset += c;
									c = getDevice()->segment[i++];
									if (c) {
										c = getDevice()->segment[i++];
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
    	c = getDevice()->segment[i++];
    	if (c) {
    		FastLED.addLeds<CHIPSET, LED_DATA_PIN0, GRB>(leds, offset, c).setCorrection( defaultCorrection );
				offset += c;
				c = getDevice()->segment[i++];
				if (c) {
					FastLED.addLeds<CHIPSET, LED_DATA_PIN1, GRB>(leds, offset, c).setCorrection( defaultCorrection );
					offset += c;
					c = getDevice()->segment[i++];
					if (c) {
						FastLED.addLeds<CHIPSET, LED_DATA_PIN2, GRB>(leds, offset, c).setCorrection( defaultCorrection );
						offset += c;
						c = getDevice()->segment[i++];
						if (c) {
							FastLED.addLeds<CHIPSET, LED_DATA_PIN3, GRB>(leds, offset, c).setCorrection( defaultCorrection );
							offset += c;
							c = getDevice()->segment[i++];
							if (c) {
								FastLED.addLeds<CHIPSET, LED_DATA_PIN4, GRB>(leds, offset, c).setCorrection( defaultCorrection );
								offset += c;
								c = getDevice()->segment[i++];
								if (c) {
									FastLED.addLeds<CHIPSET, LED_DATA_PIN5, GRB>(leds, offset, c).setCorrection( defaultCorrection );
									offset += c;
									c = getDevice()->segment[i++];
									if (c) {
										FastLED.addLeds<CHIPSET, LED_DATA_PIN6, GRB>(leds, offset, c).setCorrection( defaultCorrection );
										offset += c;
										c = getDevice()->segment[i++];
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

  matrix = new FastLED_NeoMatrix(leds, getDevice()->width, getDevice()->height, getDevice()->matrixType);
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
//  if (maxSegmentLen() > 120) {
    FastLED.setDither( 0 );
//  } else {
//  	FastLED.setDither(currMode->dither());
//  }
//	console.debugln("begin");
	currMode->begin(getDevice()->width,getDevice()->height);
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
