#include "ClockMode.h"

class WordClockMode : public ClockMode {
  public:
    virtual const char* name() { return "Word"; }
    virtual void draw(FastLED_NeoMatrix* m);
    virtual framerate_t fps() { return 2*getDevice()->width; }
    virtual bool canRun() { return getDevice()->width >= 5 && getDevice()->height >= 7 && theClock.hasBeenSet(); }
    virtual bool dither() { return false; }
    virtual void begin(uint16_t w, uint16_t h) { scrollPos = w; }
  private:
    pixel_t scrollPos = 0;
};


// here we go
void WordClockMode::draw(FastLED_NeoMatrix* m) {

  m->fillScreen(LED_BLACK);
  m->setTextColor(LED_RED_HIGH, LED_BLACK),
  m->setCursor(scrollPos, 0);
  uint8_t size = m->height()/7;
  m->setTextSize(size);
  m->setTextWrap(false);

  char time[100];

  sprintf(time, "  %d:%02d  ", theClock.hourFormat12(), theClock.minute());

  m->print(time);

  // calculate string width
  // FIXME getTextBounds clips to screen :(  )
  int16_t x1,y1;
  uint16_t w,h;
  scrollPos--;
  uint16_t totalw = 0;
  totalw = strlen(time);
  int i = 0;
  // assumes we can fit a whole character on the screen width
  while (time[i] != 0) {
    char c[2];
    c[0] = time[i];
    c[1] = 0;
    m->getTextBounds(c, 0, 0, &x1, &y1, &w, &h);
    totalw += w;
    i++;
  }

  // loop around
  if (-scrollPos > totalw) {
    scrollPos = 0;
  }

}

WordClockMode theWordClockMode;
/*
MOTDMode theMOTDMode;
SayMode theSayMode;

class SayCommand : public Command {
  public:
    const char* getName() { return "say"; }
    const char* getHelp() { return "Print text"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      c->println("Goodbye!");
      console.stop();
    }
};
ExitCommand theExitCommand;
*/
