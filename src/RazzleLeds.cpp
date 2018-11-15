#include "RazzleLeds.h"
#include <colorutils.h>
#include <Clock.h>
#include "RazzleCommands.h"
#include "Razzle.h"

#include "Commands/FPSCommand.h"
FPSCommand theFPSCommand;

#include "RazzleModes.h"

led_t num_leds;

CRGB* leds;

CRGB* frames[2];

RazzleMode* currMode = nullptr;

uint32_t lastFrameMillis = 0;
uint8_t nextFrame = 1;

uint32_t nextFrameMillis = 1;
uint8_t lastFrame = 0;
const uint32_t defaultFrameInterval = 1;  // as fast as possible
uint32_t frameIntervalMillis = defaultFrameInterval;

int modeIndex = 0;
int modeSetIndex = 0;

FastLED_NeoMatrix *matrix;

uint32_t white(uint8_t y) {
  uint32_t y32 = y;
  return y32 + (y32 << 8) + (y32 << 16);
}


inline void fps(uint32_t f)  { frameIntervalMillis = 1000/f; };

uint32_t nowMillis = 0;
uint32_t lastModeSwitchTime = 0;
uint32_t lastModeSwitch() { return lastModeSwitchTime; }

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
  led_t maxSegmentLen = 0;
  for (int i = 0; i < MAX_SEGMENTS; i++) {
  	led_t len = getDevice()->segment[i];
  	if (len > maxSegmentLen) {
  		maxSegmentLen = len;
  	}
  }
  // 120 led long string is about 100fps, dithering at about 50fps
  if (maxSegmentLen > 120) {
    FastLED.setDither( 0 );
  }
  FastLED.setCorrection(UncorrectedColor);
  FastLED.setTemperature(UncorrectedTemperature);
  FastLED.show(getBrightness());

  matrix = new FastLED_NeoMatrix(leds, getDevice()->width, getDevice()->height,
  	NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
}


void interpolateFrame() {
  uint8_t fract8 = ((nowMillis - lastFrameMillis) * 256) / (nextFrameMillis - lastFrameMillis);
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

void render(CRGB* frame, uint32_t time) {
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
  if (newMode == nullptr) return false;
  RazzleMode* namedMode = RazzleMode::named(newMode);
  if (namedMode == nullptr) return false;
  if (!namedMode->canRun()) return false;
  if (currMode == namedMode) return true;

  if (currMode) { currMode->end(); }
  currMode = namedMode;
	currMode->begin();


  lastFrame = 0;
  lastFrameMillis = nowMillis;
  fill_solid( frames[lastFrame], num_leds, CRGB::Black);
  render(frames[lastFrame], lastFrameMillis);

  nextFrame = 1;
  nextFrameMillis = nowMillis + frameIntervalMillis;
  fill_solid( frames[nextFrame], num_leds, CRGB::Black);
  render(frames[nextFrame], nextFrameMillis);
  lastModeSwitchTime = millis();
  return true;
}

void setNextLEDMode() {
	const char* nextModeName;
	RazzleMode* nextMode;

	do {
		modeIndex++;
		if (modeSets[modeSetIndex][modeIndex] == nullptr) {
			modeIndex = 0;
		}

		nextModeName = modeSets[modeSetIndex][modeIndex];
		nextMode = RazzleMode::named(nextModeName);

	} while (nextMode == nullptr || !nextMode->canRun());

		setLEDMode(nextModeName);
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
  nowMillis = millis();
  if (nowMillis >= nextFrameMillis) {
    uint8_t temp = lastFrame;
    lastFrame = nextFrame;
    nextFrame = temp;
    lastFrameMillis = nextFrameMillis;
    nextFrameMillis = nowMillis + frameIntervalMillis;
    memmove(frames[nextFrame], frames[lastFrame], num_leds * sizeof(CRGB));

    render(frames[nextFrame], nextFrameMillis);
  }

  interpolateFrame();
  FastLED.show(getBrightness());
  theFPSCommand.newFrame();
}
