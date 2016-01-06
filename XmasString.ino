#include <FastLED.h>
#include <colorutils.h>

// pin 3 on Uno, 17 on Teensy LC
#define LED_PIN     3

// TeensyLC button 1, Uno button 12
#define BUTTON (12)

#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    50

CRGB leds[NUM_LEDS];

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
#define LEDS_PER_SEGMENT (NUM_LEDS/NUM_SEGMENTS)
#define LEDS_MIDPOINT (NUM_LEDS/2)

#define LEDS_RIGHT (0)
#define LEDS_TOP (LEDS_MIDPOINT-6)
#define LEDS_LEFT (LEDS_MIDPOINT+6)

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_SEGMENTS][LEDS_PER_SEGMENT];

  // Step 1.  Cool down every cell a little
    for (int j = 0; j < NUM_SEGMENTS; j++) {
      for( int i = 0; i < LEDS_PER_SEGMENT; i++) {
        heat[j][i] = qsub8( heat[j][i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
      }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int j = 0; j < NUM_SEGMENTS; j++) {
      for( int k= LEDS_PER_SEGMENT - 1; k >= 2; k--) {
        heat[j][k] = (heat[j][k - 1] + heat[j][k - 2] + heat[j][k - 2] ) / 3;
      }
    }
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    for (int j = 0; j < NUM_SEGMENTS; j++) {
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat[j][y] = qadd8( heat[j][y], random8(SPARKHEATMIN,SPARKHEATMAX) );
      }
    }

    // Step 4.  Map from heat cells to LED colors
    for (int s = 0; s < NUM_SEGMENTS; s++) {
      for( int j = 0; j < LEDS_PER_SEGMENT; j++) {
        CRGB color = HeatColor( heat[s][j]);
        int pixelnumber;
        if( s%2 ) {
          pixelnumber = (NUM_LEDS-1) - j;
        } else {
          pixelnumber = j;
        }
        leds[pixelnumber] = color;
      }
    }
}

void setup() {
  delay(1000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  set_max_power_in_volts_and_milliamps( 5, 2400);

  pinMode(BUTTON, INPUT);      // Push-Button On Bread Board
  digitalWrite(BUTTON, HIGH);  // Turn on internal Pull-Up Resistor

}

bool button() {
  return !digitalRead(BUTTON);
}


uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

enum modes {
  FIRSTMODE,
  FIRE = FIRSTMODE,
  WAVE,
  FLASHES,
  ZIP,
  RAINBOW,
  RAINBOWROTATE,
  NOISE,
  WHITENOISE,
  WHITE,
  END,
  OFF,
  ON,
};

int mode = FIRSTMODE;
uint32_t lastModeSwitch = 0;
uint32_t autoSwitchInterval = 1000L*60;
const uint32_t holdTime = 1000;

void checkButton() {
  pressed = false;
  released = false;
  if (button()) {
    if (lastUp > lastDown) {
      lastDown = millis();
      pressed = true;
    }
  } else {
    if (lastDown > lastUp) {
      lastUp = millis();
      released = true;
    }
  }
}

uint32_t white(uint8_t y) {
  uint32_t y32 = y;
  return y32 + (y32 << 8) + (y32<<16);
}

void loop()
{

  checkButton();

  if (pressed) {
    mode++;
    lastModeSwitch = millis();
    if (mode >= END) {
      mode = FIRSTMODE;
    }
  }

  if ((millis()-lastModeSwitch) > autoSwitchInterval && mode < END) {
    mode++;
    lastModeSwitch = millis();
    if (mode >= END) {
      mode = FIRSTMODE;
    }
  }

  if (button()) {
    if ((millis() - lastDown) > 2*holdTime) {
    mode = ON;
    } else if ((millis() - lastDown) > holdTime) {
      mode = OFF;
    }
  }

  uint32_t fps = 0;  // unlimited

  switch (mode) {

    case WAVE:
      static uint16_t waveoff = 0;
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t y = (sin16(((long)i * 65536)/NUM_LEDS + waveoff) + 32767) >> 8;
        leds[i]=white(y);
      }
      waveoff+=100;
      fps = 100;
      break;

    case FIRE:
      Fire2012(); // run simulation frame
      fps = 50;
      break;

    case FLASHES:
      fill_solid( leds, NUM_LEDS, CRGB::Black);
      leds[random(NUM_LEDS)] = CRGB::White;
      fps = 50;
      break;

    case ZIP:
      static int zipper = 0;
      zipper = zipper % NUM_LEDS;
      fill_solid( leds, NUM_LEDS, CRGB::Black);
      leds[zipper++] = CRGB::White;
      fps = 100;
      break;

    case RAINBOW:
      fill_rainbow(leds, NUM_LEDS, 0);
      break;

    case RAINBOWROTATE:
      static uint8_t cycle;
      fill_rainbow(leds, NUM_LEDS, cycle++);
      break;

    case WHITENOISE:
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t y = random(256);
        leds[i] = white(y);
      }
      break;

    case NOISE:
      leds[random(NUM_LEDS)] = random(0x00ffffff);
      break;

    case WHITE:
    case ON:
      fill_solid(leds, NUM_LEDS, CRGB::White);
      break;

    case OFF:
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      break;
  }


    FastLED.show();
    if (fps) {
      FastLED.delay(1000L/fps);
    }
}

