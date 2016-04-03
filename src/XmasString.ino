#include <FastLED.h>
#include <colorutils.h>

// pin 3 on Uno, 17 on Teensy LC
#define LED_PIN     3

// TeensyLC button 1, Uno button 12
#define BUTTON (12)

#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    50

uint32_t nowMillis;

CRGB leds[NUM_LEDS];

CRGB frames[2][NUM_LEDS];
uint32_t lastFrameMillis = 0;
uint8_t nextFrame = 1;

uint32_t nextFrameMillis = 1;
uint8_t lastFrame = 0;
const uint32_t defaultFrameInterval = 1;  // as fast as possible
uint32_t frameIntervalMillis = defaultFrameInterval;

inline void fps(uint32_t f)  { frameIntervalMillis = 1000/f; }

void interpolateFrame() {

  uint8_t fract8 = ((nowMillis - lastFrameMillis) * 256) / (nextFrameMillis - lastFrameMillis);
  if (fract8 == 0) {
    memmove(leds, frames[lastFrame], NUM_LEDS * sizeof(CRGB));
  } else {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
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
  fill_solid( frame, NUM_LEDS, c);
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
#define LEDS_PER_SEGMENT (NUM_LEDS/NUM_SEGMENTS)
#define LEDS_MIDPOINT (NUM_LEDS/2)

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
  for (int i = 0; i < NUM_LEDS; i++) {
    if (frame[i]) { count++; }
  };

  int seed_width = 3;
  if (count == 0 || iterations > 100) {
    fill_solid( frame, NUM_LEDS, CRGB::Black);
    for (int i = (LEDS_MIDPOINT - seed_width/2); i < (LEDS_MIDPOINT+seed_width/2); i++) {
      if (random(2)) { frame[i] = CRGB::White; }
    }
    iterations = 0;
  } else {
    uint8_t rule = 110;
    CRGB temp[NUM_LEDS];
    for (int i = 1; i < NUM_LEDS-1; i++) {
      uint8_t cur_pattern = (frame[i-1]!=(CRGB)CRGB::Black)*4 + (frame[i]!=(CRGB)CRGB::Black)*2 + (frame[i+1]!=(CRGB)CRGB::Black);
      temp[i] = ((rule >> cur_pattern)&0x01) ? CRGB::White : CRGB::Black;
    }
    memmove( frame, temp, NUM_LEDS * sizeof( CRGB) );
    iterations++;
  }
}

void Fire2012(CRGB* frame)
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_SEGMENTS][LEDS_PER_SEGMENT];

  // Step 1.  Cool down every cell a little
  for (int j = 0; j < NUM_SEGMENTS; j++) {
    for ( int i = 0; i < LEDS_PER_SEGMENT; i++) {
      heat[j][i] = qsub8( heat[j][i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
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
        pixelnumber = (NUM_LEDS - 1) - j;
      } else {
        pixelnumber = j;
      }
      frame[pixelnumber] = color;
    }
  }
}

void setup() {
//  Serial.begin(115200);
//  while (! Serial); // Wait untilSerial is ready
//  Serial.println("Hello");

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

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
  LIFE = FIRSTMODE,
  FIRE,
  BREATHING,
  WAVE,
  FLASHES,
  ZIP,
//  RAINBOW,
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
uint32_t autoSwitchInterval = 1000L * 60 * 5;
const uint32_t holdTime = 1000;

void checkButton() {
  pressed = false;
  released = false;
  if (button()) {
    if (lastUp > lastDown) {
      lastDown = nowMillis;
      pressed = true;
    }
  } else {
    if (lastDown > lastUp) {
      lastUp = nowMillis;
      released = true;
    }
  }
}

uint32_t white(uint8_t y) {
  uint32_t y32 = y;
  return y32 + (y32 << 8) + (y32 << 16);
}

void render(CRGB* frame, uint32_t time) {
  //  static uint32_t inc = 0;
  //  uint32_t hand = (time / 1000) % NUM_LEDS;
  //  fill_solid( frame, NUM_LEDS, CRGB::Black);
  //  frame[random(NUM_LEDS)] = CRGB::White;
  //  frame[hand] = CRGB::Red;
  //  frame[inc++] = CRGB::Green;
  //  inc = inc % NUM_LEDS;

  fps(1000);  // as fast as possible

  switch (mode) {
    case LIFE:
      fps(1);
      life(frame);
      break;

    case WAVE:
      static uint16_t waveoff = 0;
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t y = (sin16(((long)i * 65536) / NUM_LEDS + waveoff) + 32767) >> 8;
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
      fill_solid( frame, NUM_LEDS, CRGB::Black);
      frame[random(NUM_LEDS)] = CRGB::White;
      break;

    case ZIP:
      static int zipper = 0;
      zipper = zipper % NUM_LEDS;
      fill_solid( frame, NUM_LEDS, CRGB::Black);
      frame[zipper++] = CRGB::White;
      break;

//    case RAINBOW:
//      fill_rainbow(frame, NUM_LEDS, 0);
//      break;

    case RAINBOWROTATE:
      static uint8_t cycle;
      fill_rainbow(frame, NUM_LEDS, cycle++);
      break;

    case WHITENOISE:
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t y = random(256);
        frame[i] = white(y);
      }
      break;

    case NOISE:

      frame[random(NUM_LEDS)] = random(0x00ffffff);
      break;

    case WHITE:
    case ON:
      fill_solid(frame, NUM_LEDS, CRGB::White);
      break;

    case OFF:
      fill_solid(frame, NUM_LEDS, CRGB::Black);
      break;
  }

}

void setMode(int newMode) {
  mode = newMode;
  lastFrame = 0;
  lastFrameMillis = nowMillis;
  fill_solid( frames[lastFrame], NUM_LEDS, CRGB::Black);
  render(frames[lastFrame], lastFrameMillis);

  nextFrame = 1;
  nextFrameMillis = nowMillis + frameIntervalMillis;
  fill_solid( frames[nextFrame], NUM_LEDS, CRGB::Black);
  render(frames[nextFrame], nextFrameMillis);
}

void loop()
{

  nowMillis = millis();

  checkButton();

  if (pressed) {
    mode++;
    lastModeSwitch = nowMillis;
    if (mode >= END) {
      mode = FIRSTMODE;
    }
  }

  if ((nowMillis - lastModeSwitch) > autoSwitchInterval && mode < END) {
    mode++;
    lastModeSwitch = nowMillis;
    if (mode >= END) {
      mode = FIRSTMODE;
    }
  }

  if (button()) {
    if ((nowMillis - lastDown) > 2 * holdTime) {
      mode = ON;
    } else if ((nowMillis - lastDown) > holdTime) {
      mode = OFF;
    }
  }

  if (nowMillis >= nextFrameMillis) {
    uint8_t temp = lastFrame;
    lastFrame = nextFrame;
    nextFrame = temp;
    lastFrameMillis = nextFrameMillis;
    nextFrameMillis = nowMillis + frameIntervalMillis;
    memmove(frames[nextFrame], frames[lastFrame], NUM_LEDS * sizeof(CRGB));

    render(frames[nextFrame], nextFrameMillis);
  }

  interpolateFrame();
  FastLED.show();
}

