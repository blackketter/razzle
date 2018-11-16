#include "RazzleMode.h"

class Fire2Mode : public RazzleMode {
  public:
    virtual const char* name() { return "Fire2"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 30; }
    virtual bool canRun() { return numPixels() > 1; }
    virtual void begin() {
      noise = new uint8_t[numPixels()];
      heat = new uint8_t[numPixels()];
    }
    virtual void end() {  delete noise; delete heat;}
  private:
    uint8_t* noise;
    uint8_t* heat;

};

Fire2Mode theFire2Mode;

/*
  FastLED Fire 2018 by Stefan Petrick
  The visual effect highly depends on the framerate.
  In the Youtube video it runs at arround 70 fps.
  https://www.youtube.com/watch?v=SWMu-a9pbyk

  The heatmap movement is independend from the framerate.
  The actual scaling operation is not.
  Check out the code for further comments about the interesting parts

  Modified to work on Razzle

*/

// here we go
void Fire2Mode::draw(FastLED_NeoMatrix* m) {
    CRGB* leds = m->getLeds();
    uint8_t Width  = m->width();
    uint8_t Height = m->height();
    uint8_t CentreX =  (Width / 2) - 1;
    uint8_t CentreY = (Height / 2) - 1;

  // get one noise value out of a moving noise space
  uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
  // get another one
  uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
  // average of both to get a more unpredictable curve
  uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);

  // this factor defines the general speed of the heatmap movement
  // high value = high speed
  uint8_t speed = 27;

  // control parameters for the noise array

  uint32_t x;
  uint32_t y;
  uint32_t z;

  // here we define the impact of the wind
  // high factor = a lot of movement to the sides
  x = 3 * ctrl * speed;

  // this is the speed of the upstream itself
  // high factor = fast movement
  y = 15 * millis() * speed;

  // just for ever changing patterns we move through z as well
  z = 3 * millis() * speed ;

  // ...and dynamically scale the complete heatmap for some changes in the
  // size of the heatspots.
  // The speed of change is influenced by the factors in the calculation of ctrl1 & 2 above.
  // The divisor sets the impact of the size-scaling.
  uint32_t scale_x = ctrl1 / 2;
  uint32_t scale_y = ctrl2 / 2;

  // Calculate the noise array based on the control parameters.
  for (uint8_t i = 0; i < Width; i++) {
    uint32_t ioffset = scale_x * (i - CentreX);
    for (uint8_t j = 0; j < Height; j++) {
      uint32_t joffset = scale_y * (j - CentreY);
      uint16_t data = ((inoise16(x + ioffset, y + joffset, z)) + 1);
      noise[i*Width+j] = data >> 8;
    }
  }


  // Draw the first (lowest) line - seed the fire.
  // It could be random pixels or anything else as well.
  for (uint8_t x = 0; x < Width; x++) {
    // draw
    leds[m->XY(x, Height-1)] = ColorFromPalette( HeatColors_p, noise[x*Width+0]);
    // and fill the lowest line of the heatmap, too
    heat[m->XY(x, Height-1)] = noise[x*Width+0];
  }

  // Copy the heatmap one line up for the scrolling.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      heat[m->XY(x, y)] = heat[m->XY(x, y + 1)];
    }
  }

  // Scale the heatmap values down based on the independent scrolling noise array.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {

      // get data from the calculated noise field
      uint8_t dim = noise[x*Width+y];

      // This number is critical
      // If it´s to low (like 1.1) the fire dosn´t go up far enough.
      // If it´s to high (like 3) the fire goes up too high.
      // It depends on the framerate which number is best.
      // If the number is not right you loose the uplifting fire clouds
      // which seperate themself while rising up.
      dim = dim / 1.6;

      dim = 255 - dim;

      // here happens the scaling of the heatmap
      heat[m->XY(x, y)] = scale8(heat[m->XY(x, y)] , dim);
    }
  }

  // Now just map the colors based on the heatmap.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      leds[m->XY(x, y)] = ColorFromPalette( HeatColors_p, heat[m->XY(x, y)]);
    }
  }
}
