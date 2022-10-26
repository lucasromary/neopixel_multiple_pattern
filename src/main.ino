#include <Arduino.h>
// StrandTest from AdaFruit implemented as a state machine
// pattern change by push button
// By Mike Cook Jan 2016

#define PINforControl 21 // pin connected to 1 single NeoPixels strip
#define NUMPIXELS1 80    // number of LEDs on strip

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS1, PINforControl, NEO_GRB + NEO_KHZ800);
// Pattern types supported:
enum pattern_type
{
  NONE,
  BLINK,
  RAINBOW,
  RAINBOW_CYCLE,
  THEATER_CHASE_RAINBOW,
  COLOR_WIPE,
  FADING,
  CHASING
};

struct subStrips
{
  uint16_t startIndex;
  uint16_t nb_LED;
  pattern_type activePattern;
  unsigned long patternInterval;
  unsigned long lastUpdate;
};

int neopixel_blink[21] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
// declare substrips
// the 20 LED are grouped by 7, 1, 1, 1, 1, 1, 7 (see image in the post)

subStrips stripRamp = {0, 7, THEATER_CHASE_RAINBOW, 20, 0};
subStrips stripSingle1 = {7, 1, NONE, 20, 0};
subStrips stripFade = {40, 10, FADING, 2, 0};
subStrips stripRainbow = {11, 10, RAINBOW, 20, 0};
subStrips stripBlink = {30, 10, BLINK, 215, 0};
subStrips stripSingle5 = {11, 1, RAINBOW, 20, 0};
subStrips stripAmbilight = {0, 0, RAINBOW_CYCLE, 20, 0};
subStrips stripChasing = {50, 20, CHASING, 100, 0};

unsigned long patternInterval = 20;            // time between steps in the pattern
unsigned long lastUpdate = 0;                  // for millis() when last update occoured
unsigned long intervals[] = {20, 20, 50, 100}; // speed for each pattern
const byte button = 2;                         // pin to connect button switch to between pin and ground
int steps_fade = 0;
int steps_chase = 0;

void setup()
{
  strip.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  wipe();                        // wipes the LED buffers
  pinMode(button, INPUT_PULLUP); // change pattern
}

void loop()
{
  // subStrip Ramp
  if (millis() - stripBlink.lastUpdate > stripBlink.patternInterval)
  {
    updateSubStripPattern(stripBlink);
  }

  // subStrip Single1
  if (millis() - stripRainbow.lastUpdate > stripRainbow.patternInterval)
    updateSubStripPattern(stripRainbow);

  if (millis() - stripFade.lastUpdate > stripFade.patternInterval)
  {
    updateSubStripPattern(stripFade);
  }
  
    if (millis() - stripChasing.lastUpdate > stripChasing.patternInterval){
      updateSubStripPattern(stripChasing);
    }
  
  //...
}

void updateSubStripPattern(subStrips &substripOut)
{ // out parameter
  switch (substripOut.activePattern)
  {
  case NONE:
    wipeSubStrip(substripOut);
    break;
  case RAINBOW:
    rainbow(substripOut);
    break;
  case RAINBOW_CYCLE:
    rainbowCycle(substripOut);
    break;
  case CHASING:
    steps_chase = chasing(substripOut, strip.Color(255, 0, 0), steps_chase);
    break;
  case FADING:
    steps_fade = fade(substripOut, strip.Color(255, 0, 0), steps_fade);
    break;
  case THEATER_CHASE_RAINBOW:
    theaterChaseRainbow(substripOut);
    break;
  case COLOR_WIPE:
    colorWipe(substripOut, strip.Color(255, 0, 0)); // red
    break;
  case BLINK:
    blinkEffect(substripOut, 111);
    break;
  }
}

int chasing(subStrips &substrip, uint32_t color, int steps_chase)
{
  Serial.println("CHASING");

    steps_chase++;
    for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
    {
      // sine wave, 3 offset waves make a rainbow!
      // float level = sin(i+Position) * 127 + 128;
      // setPixel(i,level,0,0);
      // float level = sin(i+Position) * 127 + 128;
      int color = sin(i + steps_chase) * 127 + 128;
      if (color < 120){
        color = 0;
      }
      strip.setPixelColor(i,0, color, 0);
    }

    strip.show();

  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase;
}
void blinkEffect(subStrips &substrip, uint32_t color)
{
  // Serial.println("blink");
  for (int i = 0; i < sizeof(neopixel_blink); i++)
  {
    if (strip.getPixelColor(neopixel_blink[i]) == 0)
    {
      strip.setPixelColor(neopixel_blink[i], color);
    }
    else
    {
      strip.setPixelColor(neopixel_blink[i], 0);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display
}

int fade(subStrips &substrip, uint32_t color, int steps_fade)
{
  if (steps_fade < 255)
  {
    for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
    {
      strip.setPixelColor(i, strip.Color(255 - steps_fade, 0, 0));
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade++;
  }
  else
  {
    for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
    {
      strip.setPixelColor(i, strip.Color(-255 + steps_fade, 0, 0));
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade++;
  }

  if (steps_fade == 510)
  {
    steps_fade = 0;
  }

  return steps_fade;
}

void rainbow(subStrips &substrip)
{ // modified from Adafruit example to make it a state machine
  static uint16_t j = 0;
  for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
  {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
  j++;
  if (j >= 256)
    j = 0;
  substrip.lastUpdate = millis(); // time for next change to the display
}

void rainbowCycle(subStrips &substrip)
{ // modified from Adafruit example to make it a state machine
  // Serial.println("Rainbow");
  for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
  {
    strip.setPixelColor(i, Wheel(((i * 256 / substrip.nb_LED) + substrip.startIndex) & 255));
  }
  strip.show();
}

void theaterChaseRainbow(subStrips &substrip)
{ // modified from Adafruit example to make it a state machine
  //
}

void colorWipe(subStrips &substrip, uint32_t c)
{ // modified from Adafruit example to make it a state machine
  //
}

void wipeSubStrip(subStrips &substrip)
{ // clear all LEDs
  for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  // substrip.lastUpdate = millis(); // no need to refresh LED off
  substrip.patternInterval = 10000000; // no need to refresh LED off
}

void wipe()
{ // clear all LEDs
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}