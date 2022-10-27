#include <Arduino.h>
// StrandTest from AdaFruit implemented as a state machine
// pattern change by push button
// By Mike Cook Jan 2016

#define PINforControl 21 // pin connected to 1 single NeoPixels strip
#define NUMPIXELS1 1500  // number of LEDs on strip

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
  CHASING,
  CHASING_BLUE,
  FADING_TO_GREEN,
  FADING_TO_RED
};

struct subStrips
{
  uint16_t startIndex;
  uint16_t nb_LED;
  pattern_type activePattern;
  unsigned long patternInterval;
  unsigned long lastUpdate;
  int *led;
  int size_tab;
};

// test olivier, déclaration d'un tableau avec les ID des premieres et dernieres leds de chaque bandeau
int batiment_acc[12] = {230, 242, 393, 401, 782, 800, 619, 627, 566, 570};
int panneau_pv[] = {230, 242, 401, 478, 991, 1051, 1064, 1152};
int batt[] = {246, 251, 483, 486, 1058, 1062};
int borne_charge[] = {988, 991, 1300, 1305};
int vehicule[] = {986, 988, 1209, 1213, 1305, 1312, 1320, 1324};
int maison[] = {1292, 1300, 1313, 1320, 1324, 1332};

// 2 petite leds blue ?? 
int partie_bleu[] = {0, 230, 251, 293, 295, 393, 488, 548, 570, 619, 627, 640, 800, 986, 1152, 1209, 1213, 1290, 642, 695, 697, 782};
int poste_distribution[] = {293, 295, 401, 403, 515, 517, 640, 642, 696, 698, 860, 862, 1290, 1292}; // 370 ?

int chemin_pv[] = {242, 246, 478, 483, 486, 488, 1062, 1064, 1051, 1058};

subStrips stripFade = {40, 10, FADING, 35, 0, panneau_pv, 8};
subStrips stripRainbow = {11, 10, RAINBOW, 20, 0};
subStrips stripBlink2 = {30, 10, BLINK, 215, 0};
subStrips stripBlink = {30, 10, BLINK, 2000, 0, panneau_pv, 30};
subStrips stripChasing = {50, 20, CHASING, 2, 0, chemin_pv, 10};
subStrips stripFadingToGreen = {50, 20, FADING_TO_GREEN, 1000, 0, batt, 6};
subStrips stripChasingBlue = {50, 20, CHASING_BLUE, 2, 0, partie_bleu, 22};

subStrips stripFadingToRed = {50, 20, FADING_TO_RED, 1000, 0, batt, 6};
subStrips stripFadeBattery = {40, 10, FADING, 35, 0, batiment_acc, 10};

int steps_fade = 0;
int steps_chase = 0;
int steps_chase_blue = 0;
int steps_fade_to_green = 1;
int steps_fade_to_red = 1;

void setup()
{
  strip.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  wipe(); // wipes the LED buffers
}

void loop()
{
// PV Version Nuit // 
    if (millis() - stripFadingToRed.lastUpdate > stripFadingToRed.patternInterval)
    {
      updateSubStripPattern(stripFadingToRed);
    }

    if (millis() - stripFadeBattery.lastUpdate > stripFadeBattery.patternInterval)
    {
      updateSubStripPattern(stripFadeBattery);
    }

        if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
    {
      updateSubStripPattern(stripChasingBlue);
    }
  // subStrip Ramp
  /*
  if (millis() - stripBlink.lastUpdate > stripBlink.patternInterval)
  {
    updateSubStripPattern(stripBlink);
  }
  */

  // subStrip Single1
  /*
  if (millis() - stripRainbow.lastUpdate > stripRainbow.patternInterval)
    updateSubStripPattern(stripRainbow);
*/

  // Panneau nuit

  // PANNEAU PV //
  /*
    if (millis() - stripFade.lastUpdate > stripFade.patternInterval)
    {
      updateSubStripPattern(stripFade);
    }

    if (millis() - stripChasing.lastUpdate > stripChasing.patternInterval)
    {
      updateSubStripPattern(stripChasing);
    }

    if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
    {
      updateSubStripPattern(stripChasingBlue);
    }
    if (millis() - stripFadingToRed.lastUpdate > stripFadingToRed.patternInterval)
    {
      updateSubStripPattern(stripFadingToRed);
    }
  */
}

void updateSubStripPattern(subStrips &substripOut)
{ // out parameter
  switch (substripOut.activePattern)
  {
  case RAINBOW:
    rainbow(substripOut);
    break;
  case FADING_TO_GREEN:
    steps_fade_to_green = fade_green(substripOut, steps_fade_to_green);
    break;
  case FADING_TO_RED:
    steps_fade_to_red = fade_to_red(substripOut, steps_fade_to_red);
    break;
  case CHASING:
    steps_chase = chasing(substripOut, strip.Color(100, 100, 0), steps_chase);
    break;
  case CHASING_BLUE:
    steps_chase = chasing_blue(substripOut, strip.Color(0, 0, 30), steps_chase);
    break;
  case FADING:
    steps_fade = fade(substripOut, strip.Color(100, 0, 0), steps_fade);
    break;
  case BLINK:
    blinkEffect(substripOut, 111);
    break;
  }
}

int chasing2(subStrips &substrip, uint32_t color, int steps_chase)
{
  // Serial.println("CHASING");
  steps_chase++;
  for (int i = substrip.startIndex; i < (substrip.startIndex + substrip.nb_LED); i++)
  {
    // sine wave, 3 offset waves make a rainbow!
    // float level = sin(i+Position) * 127 + 128;
    // setPixel(i,level,0,0);
    // float level = sin(i+Position) * 127 + 128;
    int color = sin(i + steps_chase) * 127 + 128;
    if (color < 200)
    {
      color = 0;
    }
    strip.setPixelColor(i, 0, color, 0);
  }

  strip.show();

  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase;
}

int chasing_blue(subStrips &substrip, uint32_t color, int steps_chase_blue)
{
  steps_chase_blue++;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = (sin(dx + steps_chase) * 127 + 128) / 6;
      if (color < 10)
      {
        color = 0;
      }
      strip.setPixelColor(dx, 0, 0, color);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase_blue;
}

int chasing(subStrips &substrip, uint32_t color, int steps_chase)
{
  steps_chase++;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = sin(dx + steps_chase) * 127 + 128;
      if (color < 120)
      {
        color = 0;
      }
      strip.setPixelColor(dx, color, color, 0);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase;
}

// méthode olivier
void blinkEffect(subStrips &substrip, uint32_t color)
{
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      // Serial.println(dx);
      if (strip.getPixelColor(dx) == 0)
      {
        strip.setPixelColor(dx, color);
      }
      else
      {
        strip.setPixelColor(dx, 0);
      }
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display
}

int fade2(subStrips &substrip, uint32_t color, int steps_fade)
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

int fade(subStrips &substrip, uint32_t color, int steps_fade)
{
  if (steps_fade < 50)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(50 - steps_fade, 50 - steps_fade, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade += 3;
  }
  else
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(-50 + steps_fade, -50 + steps_fade, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade += 3;
  }

  if (steps_fade > 100)
  {
    steps_fade = 0;
  }

  return steps_fade;
}

int fade_green(subStrips &substrip, int steps_fade_to_green)
{
  if (steps_fade_to_green < 150)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(150, steps_fade_to_green, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_green += 3;
  }

  if (steps_fade_to_green > 150 && steps_fade_to_green < 300)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(300 - steps_fade_to_green, 150, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_green += 3;
  }
  return steps_fade_to_green;
}

int fade_to_red(subStrips &substrip, int steps_fade_to_red)
{
  if (steps_fade_to_red < 150)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(steps_fade_to_red, 150, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red += 3;
  }

  if (steps_fade_to_red > 150 && steps_fade_to_red < 300)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(150, 300-steps_fade_to_red, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red += 3;
  }
  return steps_fade_to_red;
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