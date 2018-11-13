#include "RazzleMode.h"

class FireMode : public RazzleMode {
  public:
    virtual const char* name() { return "Fire"; }
    virtual void draw(CRGB* frame);
    virtual framerate_t fps() { return 15; }
    virtual bool canRun() { return numPixels() > 1; }
  private:
};

FireMode theFireMode;




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
// This simulation scales it self a bit depending on number of pixels; it should look
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

void FireMode::draw(CRGB* frame)
{
  led_t pixels = numPixels();
  led_t leds_per_segment = (pixels/NUM_SEGMENTS);

  // Array of temperature readings at each simulation cell
  static byte* heat = 0;

  if (heat == 0) {
    heat = new byte[pixels];
  }

  // Step 1.  Cool down every cell a little
  for (led_t j = 0; j < NUM_SEGMENTS; j++) {
    for ( led_t i = 0; i < leds_per_segment; i++) {
      led_t offset = j*NUM_SEGMENTS + i;
      heat[offset] = qsub8( heat[offset],  random8(0, ((COOLING * 10) / pixels) + 2));
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (led_t j = 0; j < NUM_SEGMENTS; j++) {
    for ( led_t k = leds_per_segment - 1; k >= 2; k--) {
      led_t offset = j*NUM_SEGMENTS + k;
      heat[offset] = (heat[offset - 1] + heat[offset - 2] + heat[offset - 2] ) / 3;
    }
  }
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  for (led_t j = 0; j < NUM_SEGMENTS; j++) {
    if ( random8() < SPARKING ) {
      led_t y = random8(7);
      led_t offset = j*NUM_SEGMENTS + y;
      heat[offset] = qadd8( heat[offset], random8(SPARKHEATMIN, SPARKHEATMAX) );
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for (led_t s = 0; s < NUM_SEGMENTS; s++) {
    for ( led_t j = 0; j < leds_per_segment; j++) {
      led_t offset = s*NUM_SEGMENTS + j;
      CRGB color = HeatColor( heat[offset]);
      led_t pixelnumber;
      if ( s % 2 ) {
        pixelnumber = (pixels - 1) - j;
      } else {
        pixelnumber = j;
      }
      frame[pixelnumber] = color;
    }
  }
}

