#include "RazzleMode.h"

class Fire2Mode : public RazzleMode {
  public:
    virtual const char* name() { return "Fire2"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 30; }
    virtual bool canRun() { return numPixels() > 1; }
    virtual void begin(uint16_t w, uint16_t h) {
      _noise = new uint8_t[w*h];
      _heat = new uint8_t[w*h];

      // some defaults, based on display dimensions
      _height = h / 10.0;
      _speed = w;
    }
    virtual void end() {  delete _noise; delete _heat;}
    float getHeight() { return _height; };
    void setHeight(float height) { _height = height; }
    uint8_t getSpeed() { return _speed; };
    void setSpeed(uint8_t speed) { _speed = speed; }
  private:
    uint8_t* _noise;
    uint8_t* _heat;

  // ...and dynamically scale the complete heatmap for some changes in the
  // size of the heatspots.
  // The speed of change is influenced by the factors in the calculation of ctrl1 & 2.
  // The divisor sets the impact of the size-scaling.
    float _height = 1.6;

  // this factor defines the general speed of the heatmap movement
  // high value = high speed
    uint8_t _speed = 27;
};

Fire2Mode theFire2Mode;


class FireCommand : public Command {
  public:
    const char* getName() { return "fire"; }
    const char* getHelp() { return ("fire height (default 1.6), speed (default 27)"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount >= 1) {
        float height = atof(params[1]);
        if (height) {
          theFire2Mode.setHeight(height);
        }
      }
      if (paramCount == 2) {
        uint8_t speed = atoi(params[2]);
        if (speed) {
          theFire2Mode.setSpeed(speed);
        }
      }
      c->printf("Fire height: %f speed: %d\n", theFire2Mode.getHeight(), theFire2Mode.getSpeed());
    }
};
FireCommand theFireCommand;

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


  // control parameters for the noise array

  uint32_t x;
  uint32_t y;
  uint32_t z;

  // here we define the impact of the wind
  // high factor = a lot of movement to the sides
  x = 3 * ctrl * _speed;

  // this is the speed of the upstream itself
  // high factor = fast movement
  y = 15 * millis() * _speed;

  // just for ever changing patterns we move through z as well
  z = 3 * millis() * _speed ;

  uint32_t scale_x = ctrl1 / 2;
  uint32_t scale_y = ctrl2 / 2;

  // Calculate the noise array based on the control parameters.
  for (uint8_t i = 0; i < Width; i++) {
    uint32_t ioffset = scale_x * (i - CentreX);
    for (uint8_t j = 0; j < Height; j++) {
      uint32_t joffset = scale_y * (j - CentreY);
      uint16_t data = ((inoise16(x + ioffset, y + joffset, z)) + 1);
      _noise[i*Height+j] = data >> 8;
    }
  }


  // Draw the first (lowest) line - seed the fire.
  // It could be random pixels or anything else as well.
  for (uint8_t x = 0; x < Width; x++) {
    // draw
    m->drawPixelCRGB(x, Height-1, ColorFromPalette( HeatColors_p, _noise[x*Height+0]));
    // and fill the lowest line of the heatmap, too
    _heat[x*Height+Height-1] = _noise[x*Height+0];
  }

  // Copy the heatmap one line up for the scrolling.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      _heat[x*Height + y] = _heat[x*Height + y + 1];
    }
  }

  // Scale the heatmap values down based on the independent scrolling noise array.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {

      // get data from the calculated noise field
      uint8_t dim = _noise[x*Height+y];

      // This number is critical
      // If it´s to low (like 1.1) the fire dosn´t go up far enough.
      // If it´s to high (like 3) the fire goes up too high.
      // It depends on the framerate which number is best.
      // If the number is not right you lose the uplifting fire clouds
      // which seperate themself while rising up.
      dim = dim / _height;

      dim = 255 - dim;

      // here happens the scaling of the heatmap
      _heat[x*Height + y] = scale8(_heat[x*Height + y] , dim);
    }
  }

  // Now just map the colors based on the heatmap.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      m->drawPixelCRGB(x, y, ColorFromPalette( HeatColors_p, _heat[x*Height + y]));
    }
  }
}
