#ifndef _RazzleModes_
#define _RazzleModes_

const char* colorModes[] =
{
  "Cops",
  "Lines",
  "Fire",
  "Life",
  "Breathing",
  "Wave",
  "Flashes",
  "Zipper",
  "RainbowRotate",
  "Noise",
  "WhiteNoise",
  nullptr
};

const char* solidModes[] =
{
  "Off",
  "On",
  "Grey80",
  "Grey40",
  "Grey20",
  "Grey10",
  "Grey08",
  "Grey04",
  "Grey02",
  "Grey01",
  nullptr
};

const char** modeSets[] =
{
  colorModes,
  solidModes,
  nullptr
};

#endif
