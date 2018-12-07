#include "Clock.h"
#include "Console.h"
#include "RazzleLeds.h"
#include "RazzleMode.h"

class LEDModesCommand : public Command {
  public:
    const char* getName() { return "modes"; }
    const char* getHelp() { return ("list LED modes"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      RazzleMode* m = RazzleMode::first();
      c->printf("Valid LED modes:\n");
      while (m) {
        c->printf("  %s\n", m->name());
        m = m->next();
      }
    }
};
LEDModesCommand theLEDModesCommand;

class LEDModeCommand : public Command {
  public:
    const char* getName() { return "mode"; }
    const char* getHelp() { return ("set LED mode"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        if (RazzleMode::named(params[1])) {
          setLEDMode(params[1]);
        } else {
          theLEDModesCommand.execute(c,paramCount,params);
        }
      }
      c->printf("LED Mode: %s\n", getLEDMode());
    }
};
LEDModeCommand theLEDModeCommand;


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
BrightnessCommand theBrightnessCommand;

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
RecoverCommand theRecoverCommand;


extern uint32_t autoSwitchInterval;

class SwitchCommand : public Command {
  public:
    const char* getName() { return "switch"; }
    const char* getHelp() { return ("set autoswitch interval in seconds (0 to disable)"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      uint32_t b = 0;
      if (paramCount == 1) {
        b = atoi(params[1]);
        autoSwitchInterval = b*1000;
      }
      c->printf("autoswitch interval: %d seconds\n", autoSwitchInterval/1000);
    }
};
SwitchCommand theSwitchCommand;

class NextCommand : public Command {
  public:
    const char* getName() { return "next"; }
    const char* getHelp() { return ("switch to next mode"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      setNextLEDMode(true);
      c->printf("LED Mode: %s\n", getLEDMode());
    }
};
NextCommand theNextCommand;
