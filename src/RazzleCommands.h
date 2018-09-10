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

extern LEDModeCommand theLEDModeCommand;


class FPSCommand : public Command {
  public:
    const char* getName() { return "fps"; }
    const char* getHelp() { return "toggle displaying frame rate in fps"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      enable = !enable;
      c->printf("FPS display %s\n", enable ? "enabled" : "disabled");
    }
    void newFrame() {
      frames++;
      millis_t now = Uptime::millis();
      millis_t frameDur = now - lastFrame;
      if (frameDur > maxFrame) { maxFrame = frameDur; }
      if (now - lastTime > 1000) {
        if (enable) {
          console.debugf("FPS: %.2f (Max frame: %dms, Max idle: %dms)\n", ((float)(frames*1000))/(now - lastTime), (int)maxFrame, (int)maxIdle);
        }
        lastTime = now;
        frames = 0;
        maxFrame = 0;
        maxIdle = 0;
      }
      lastFrame = now;
    }

    void idled() {
      millis_t now = Uptime::millis();
      millis_t idleDur = now - lastIdle;
      if (idleDur > maxIdle) { maxIdle = idleDur; }
      lastIdle = now;
    }

  private:
    bool enable = false;
    uint32_t frames = 0;
    millis_t lastTime;
    millis_t lastFrame;
    millis_t maxFrame;

    millis_t lastIdle = 0;
    millis_t maxIdle = 0;
};

extern FPSCommand theFPSCommand;

#endif