#ifndef _RazzleCommands_
#define _RazzleCommands_

#include "Console.h"
#include "RazzleLeds.h"
#include "Clock.h"

class LEDModeCommand : public Command {
  public:
    const char* getName() { return "mode"; }
    const char* getHelp() { return ("set LED mode"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        setLedMode(atoi(params[1]));
      }
      c->printf("LED Mode: %d\n", getLedMode());
    }
};

class BrightnessCommand : public Command {
  public:
    const char* getName() { return "bright"; }
    const char* getHelp() { return ("set global LED brightness"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {

      if (paramCount == 1) {
        uint8_t b = atoi(params[1]);
        setBrightness(b,b);
      } else if (paramCount == 2) {
        setBrightness(atoi(params[1]),atoi(params[2]));
      }

      c->printf("LED Brightness: day: %d, night: %d (It's %s!)\n", getDayBrightness(), getNightBrightness(), isDay() ? "day" : "night");
    }
};

extern bool recoverMode;

class RecoverCommand : public Command {
  public:
    const char* getName() { return "recover"; }
    const char* getHelp() { return "(1|0|) set or toggle recover mode"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        recoverMode = atoi(params[1]);
      } else {
        recoverMode = !recoverMode;
      }
      c->printf("Recover Mode: %s\n", recoverMode ? "on" : "off");
    }
  private:
};


#endif