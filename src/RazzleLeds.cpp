#include "RazzleLeds.h"
#include <colorutils.h>
#include <WiFiConsole.h>
extern WiFiConsole console;

#define LED_DATA_PIN (D2)
#define CHIPSET     WS2811
#define MAX_NUM_LEDS    64

#define LIGHT_SENSOR (A0)

int num_leds;

CRGB leds[MAX_NUM_LEDS];

CRGB frames[2][MAX_NUM_LEDS];
uint32_t lastFrameMillis = 0;
uint8_t nextFrame = 1;

uint32_t nextFrameMillis = 1;
uint8_t lastFrame = 0;
const uint32_t defaultFrameInterval = 1;  // as fast as possible
uint32_t frameIntervalMillis = defaultFrameInterval;

inline void fps(uint32_t f)  { frameIntervalMillis = 1000/f; };

uint32_t nowMillis = 0;
uint32_t lastModeSwitchTime = 0;

void  setupLeds(EOrder order, int led_count) {

  num_leds = led_count >  MAX_NUM_LEDS ?  MAX_NUM_LEDS : led_count;

  switch (order) {
    case RGB:
      FastLED.addLeds<CHIPSET, LED_DATA_PIN, RGB>(leds, num_leds).setCorrection( TypicalLEDStrip );
      break;
    case GRB:
      FastLED.addLeds<CHIPSET, LED_DATA_PIN, GRB>(leds, num_leds).setCorrection( TypicalLEDStrip );
      break;
    default:
      console.debugln("ERROR: Bad color order");
  }

  fill_solid(leds, num_leds, CRGB::Black);
  FastLED.show();
}


void interpolateFrame() {

  uint8_t fract8 = ((nowMillis - lastFrameMillis) * 256) / (nextFrameMillis - lastFrameMillis);
  if (fract8 == 0) {
    memmove(leds, frames[lastFrame], num_leds * sizeof(CRGB));
  } else {
    for (uint8_t i = 0; i < num_leds; i++) {
      leds[i].r = lerp8by8( frames[lastFrame][i].r, frames[nextFrame][i].r, fract8 );
      leds[i].g = lerp8by8( frames[lastFrame][i].g, frames[nextFrame][i].g, fract8 );
      leds[i].b = lerp8by8( frames[lastFrame][i].b, frames[nextFrame][i].b, fract8 );
    }
  }
}

void breathing(CRGB* frame) {

  uint32_t brightness = sin16((millis()*5)%65536); // approx 13s cycle time
  uint8_t scaled = (brightness * brightness)/(65536L*256);

  CRGB c;
  c.setRGB(scaled, scaled, scaled);
  fill_solid( frame, num_leds, c);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  60

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

// Max and Min spark values (default 255 and 160)
#define SPARKHEATMAX 100
#define SPARKHEATMIN 0

#define NUM_SEGMENTS (2)
#define LEDS_PER_SEGMENT (num_leds/NUM_SEGMENTS)
#define LEDS_MIDPOINT (num_leds/2)

#define LEDS_RIGHT (0)
#define LEDS_TOP (LEDS_MIDPOINT-6)
#define LEDS_LEFT (LEDS_MIDPOINT+6)

void life(CRGB* frame) {
  int count = 0;
  static uint32_t lastDraw = 0;
  uint32_t now = millis()/100;
  if (now == lastDraw) {
    return;
  }

  lastDraw = now;

  static int iterations = 0;
  for (int i = 0; i < num_leds; i++) {
    if (frame[i]) { count++; }
  };

  int seed_width = 3;
  if (count == 0 || iterations > 100) {
    fill_solid( frame, num_leds, CRGB::Black);
    for (int i = (LEDS_MIDPOINT - seed_width/2); i < (LEDS_MIDPOINT+seed_width/2); i++) {
      if (random(2)) { frame[i] = CRGB::White; }
    }
    iterations = 0;
  } else {
    uint8_t rule = 110;
    CRGB temp[num_leds];
    for (int i = 1; i < num_leds-1; i++) {
      uint8_t cur_pattern = (frame[i-1]!=(CRGB)CRGB::Black)*4 + (frame[i]!=(CRGB)CRGB::Black)*2 + (frame[i+1]!=(CRGB)CRGB::Black);
      temp[i] = ((rule >> cur_pattern)&0x01) ? CRGB::White : CRGB::Black;
    }
    memmove( frame, temp, num_leds * sizeof( CRGB) );
    iterations++;
  }
}

void Fire2012(CRGB* frame)
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_SEGMENTS][MAX_NUM_LEDS/NUM_SEGMENTS];

  // Step 1.  Cool down every cell a little
  for (int j = 0; j < NUM_SEGMENTS; j++) {
    for ( int i = 0; i < LEDS_PER_SEGMENT; i++) {
      heat[j][i] = qsub8( heat[j][i],  random8(0, ((COOLING * 10) / num_leds) + 2));
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int j = 0; j < NUM_SEGMENTS; j++) {
    for ( int k = LEDS_PER_SEGMENT - 1; k >= 2; k--) {
      heat[j][k] = (heat[j][k - 1] + heat[j][k - 2] + heat[j][k - 2] ) / 3;
    }
  }
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  for (int j = 0; j < NUM_SEGMENTS; j++) {
    if ( random8() < SPARKING ) {
      int y = random8(7);
      heat[j][y] = qadd8( heat[j][y], random8(SPARKHEATMIN, SPARKHEATMAX) );
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for (int s = 0; s < NUM_SEGMENTS; s++) {
    for ( int j = 0; j < LEDS_PER_SEGMENT; j++) {
      CRGB color = HeatColor( heat[s][j]);
      int pixelnumber;
      if ( s % 2 ) {
        pixelnumber = (num_leds - 1) - j;
      } else {
        pixelnumber = j;
      }
      frame[pixelnumber] = color;
    }
  }
}



int mode = FIRSTMODE;

int   getLedMode() { return mode;};

uint32_t white(uint8_t y) {
  uint32_t y32 = y;
  return y32 + (y32 << 8) + (y32 << 16);
}

void render(CRGB* frame, uint32_t time) {
  //  static uint32_t inc = 0;
  //  uint32_t hand = (time / 1000) % num_leds;
  //  fill_solid( frame, num_leds, CRGB::Black);
  //  frame[random(num_leds)] = CRGB::White;
  //  frame[hand] = CRGB::Red;
  //  frame[inc++] = CRGB::Green;
  //  inc = inc % num_leds;

  fps(1000);  // as fast as possible

  uint32_t ambient;
  switch (mode) {

    case AMBIENT:
      fps(3);
      ambient = analogRead(LIGHT_SENSOR);
//      ambient = ambient*ambient/1024;
      console.debugf("ambient: %d\n", ambient);
      fill_solid( frame, num_leds, CRGB::Black);
      fill_solid( frame, num_leds * ambient / 1024, CRGB::Green);
      break;

    case LIFE:
      fps(1);
      life(frame);
      break;

    case WAVE:
      static uint16_t waveoff = 0;
      for (int i = 0; i < num_leds; i++) {
        uint8_t y = (sin16(((int32_t)i * 65536) / num_leds + waveoff) + 32767) >> 8;
        frame[i] = white(y);
      }
      waveoff += 100;
      break;

    case FIRE:
      fps(15);
      Fire2012(frame); // run simulation frame
      break;

    case BREATHING:
      breathing(frame);
      break;

    case FLASHES:
      fill_solid( frame, num_leds, CRGB::Black);
      frame[random(num_leds)] = CRGB::White;
      break;

    case ZIP:
      static int zipper = 0;
      zipper = zipper % num_leds;
      fill_solid( frame, num_leds, CRGB::Black);
      frame[zipper++] = CRGB::White;
      break;

//    case RAINBOW:
//      fill_rainbow(frame, num_leds, 0);
//      break;

    case RAINBOWROTATE:
      static uint8_t cycle;
      fill_rainbow(frame, num_leds, cycle++);
      break;

    case WHITENOISE:
      for (int i = 0; i < num_leds; i++) {
        uint8_t y = random(256);
        frame[i] = white(y);
      }
      break;

    case NOISE:

      frame[random(num_leds)] = random(0x00ffffff);
      break;

    case WHITE:
    case ON:
      fill_solid(frame, num_leds, CRGB::White);
      break;

    case OFF:
      fill_solid(frame, num_leds, CRGB::Black);
      break;
  }

}

void setLedMode(int newMode) {
  mode = newMode;
  lastFrame = 0;
  lastFrameMillis = nowMillis;
  fill_solid( frames[lastFrame], num_leds, CRGB::Black);
  render(frames[lastFrame], lastFrameMillis);

  nextFrame = 1;
  nextFrameMillis = nowMillis + frameIntervalMillis;
  fill_solid( frames[nextFrame], num_leds, CRGB::Black);
  render(frames[nextFrame], nextFrameMillis);
  lastModeSwitchTime = millis();
}

uint32_t lastModeSwitch() { return lastModeSwitchTime; }

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
  FastLED.show();
}
