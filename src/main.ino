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
  BLINK,
  CHASING_YELLOW,
  CHASING_YELLOW_REVERSE,
  CHASING_BLUE,
  FADING_YELLOW,
  FADING_GREEN_MAISON,
  FADING_TO_GREEN_BATT,
  FADING_TO_RED_BATT,
  FADING_TO_GREEN_VOITURE,
  FADING_TO_RED_VOITURE,
  CONSTANT
};

struct subStrips
{
  pattern_type activePattern;
  unsigned long patternInterval;
  unsigned long lastUpdate;
  int *led;
  int size_tab;
};

int partie_bleu[] = {0, 230, 251, 293, 295, 393, 486, 515, 517, 548, 570, 619, 627, 640, 800, 986, 1152, 1209, 1213, 1290, 642, 695, 698, 782}; // 2 petite leds blue ??
int poste_distribution[] = {293, 295, 401, 403, 515, 517, 640, 642, 696, 698, 860, 862, 1290, 1292};                                            // 370 ?
int chemin_pv[] = {1062, 1064};
int chemin_pv_reverse[] = {242, 246, 478, 483, 1051, 1058};
int batiment_acc[12] = {230, 242, 393, 401, 782, 800, 619, 627, 566, 570};
int panneau_pv[] = {230, 242, 403, 478, 991, 1051, 1064, 1152};
int batt[] = {246, 251, 483, 486, 1058, 1062};
int borne_charge[] = {988, 991, 1300, 1305};
int vehicule[] = {986, 988, 1209, 1213, 1305, 1312, 1320, 1324};
int vehicule_to_grid[] = {986, 988, 1209, 1213};
int vehicule_to_home[] = {1305, 1312, 1320, 1324};
int maison[] = {1292, 1300, 1313, 1320, 1324, 1332};

subStrips stripBlink = {BLINK, 2000, 0, panneau_pv, 30};
subStrips stripFadeYellowPV = {FADING_YELLOW, 35, 0, panneau_pv, 8};
subStrips stripFadeYellowBatimentACC = {FADING_YELLOW, 35, 0, batiment_acc, 10};

subStrips stripChasingBlue = {CHASING_BLUE, 2, 0, partie_bleu, 24};
subStrips stripChasingYellow = {CHASING_YELLOW, 2, 0, chemin_pv, 2};
subStrips stripChasingYellowReverse = {CHASING_YELLOW_REVERSE, 2, 0, chemin_pv_reverse, 6};

subStrips stripFadingToGreen = {FADING_TO_GREEN_BATT, 1000, 0, batt, 6};
subStrips stripFadingToRed = {FADING_TO_RED_BATT, 1000, 0, batt, 6};
subStrips stripFadingToGreenVoiture = {FADING_TO_GREEN_VOITURE, 250, 0, vehicule_to_grid, 4};
subStrips stripFadingToRedVoiture = {FADING_TO_RED_VOITURE, 250, 0, vehicule_to_home, 4};

subStrips stripConstant = {CONSTANT, 100000, 100000 - 100, borne_charge, 4};
subStrips stripConstant_poste_distribution = {CONSTANT, 100000, 100000 - 100, poste_distribution, 14};
subStrips stripFadeGreenMaison = {FADING_GREEN_MAISON, 35, 0, maison, 6};

int steps_fade_yellow = 0;
int steps_chase_yellow = 0;
int steps_chase_yellow_reverse = 0;
int steps_chase_blue = 0;
int steps_fade_to_green = 1;
int steps_fade_to_red = 1;
int steps_fade_to_green_voiture = 1;
int steps_fade_to_red_voiture = 1;
int steps_fade_green_maison = 0;

void setup()
{
  strip.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  wipe(); // wipes the LED buffers
}

void loop()
{

  // VEHICULE TO HOME //

  
  if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
  {
    updateSubStripPattern(stripChasingBlue);
  }
  if (millis() - stripFadingToRedVoiture.lastUpdate > stripFadingToRedVoiture.patternInterval)
  {
    updateSubStripPattern(stripFadingToRedVoiture);
  }
  if (millis() - stripFadeGreenMaison.lastUpdate > stripFadeGreenMaison.patternInterval)
  {
    updateSubStripPattern(stripFadeGreenMaison);
  }
  if (millis() - stripConstant.lastUpdate > stripConstant.patternInterval)
  {
    updateSubStripPattern(stripConstant);
  }
  if (millis() - stripConstant_poste_distribution.lastUpdate > stripConstant_poste_distribution.patternInterval)
  {
    updateSubStripPattern(stripConstant_poste_distribution);
  }
  
  // VEHICULE TO GRID //
/*
  if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
  {
    updateSubStripPattern(stripChasingBlue);
  }
  if (millis() - stripFadeYellowPV.lastUpdate > stripFadeYellowPV.patternInterval)
  {
    updateSubStripPattern(stripFadeYellowPV);
  }
  if (millis() - stripFadingToGreenVoiture.lastUpdate > stripFadingToGreenVoiture.patternInterval)
  {
    updateSubStripPattern(stripFadingToGreenVoiture);
  }

  if (millis() - stripConstant.lastUpdate > stripConstant.patternInterval)
  {
    updateSubStripPattern(stripConstant);
  }
  if (millis() - stripConstant_poste_distribution.lastUpdate > stripConstant_poste_distribution.patternInterval)
  {
    updateSubStripPattern(stripConstant_poste_distribution);
  }
*/
  // PANNEAU PV //
  /*
    if (millis() - stripFadeYellowPV.lastUpdate > stripFadeYellowPV.patternInterval)
    {
      updateSubStripPattern(stripFadeYellowPV);
    }

    if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
    {
      updateSubStripPattern(stripChasingBlue);
    }
    if (millis() - stripFadingToGreen.lastUpdate > stripFadingToGreen.patternInterval)
    {
      updateSubStripPattern(stripFadingToGreen);
    }

    if (millis() - stripConstant_poste_distribution.lastUpdate > stripConstant_poste_distribution.patternInterval)
    {
      updateSubStripPattern(stripConstant_poste_distribution);
    }

    if (millis() - stripChasingYellow.lastUpdate > stripChasingYellow.patternInterval)
    {
      updateSubStripPattern(stripChasingYellow);
    }

    if (millis() - stripChasingYellowReverse.lastUpdate > stripChasingYellowReverse.patternInterval)
    {
      updateSubStripPattern(stripChasingYellowReverse);
    }
  */

  // PV Version Nuit //
  /*
  if (millis() - stripFadingToRed.lastUpdate > stripFadingToRed.patternInterval)
  {
    updateSubStripPattern(stripFadingToRed);
  }

  if (millis() - stripFadeYellowBatimentACC.lastUpdate > stripFadeYellowBatimentACC.patternInterval)
  {
    updateSubStripPattern(stripFadeYellowBatimentACC);
  }

  if (millis() - stripChasingBlue.lastUpdate > stripChasingBlue.patternInterval)
  {
    updateSubStripPattern(stripChasingBlue);
  }

  if (millis() - stripConstant_poste_distribution.lastUpdate > stripConstant_poste_distribution.patternInterval)
  {
    updateSubStripPattern(stripConstant_poste_distribution);
  }
  */
}

void updateSubStripPattern(subStrips &substripOut)
{ // out parameter
  switch (substripOut.activePattern)
  {
  case CONSTANT:
    constant(substripOut, strip.Color(0, 0, 250));
    break;
  case FADING_TO_GREEN_BATT:
    steps_fade_to_green = fade_green_batt(substripOut, steps_fade_to_green);
    break;
  case FADING_TO_GREEN_VOITURE:
    steps_fade_to_green_voiture = fade_to_green_voiture(substripOut, steps_fade_to_green_voiture);
    break;
  case FADING_TO_RED_VOITURE:
    steps_fade_to_red_voiture = fade_to_red_voiture(substripOut, steps_fade_to_red_voiture);
    break;
  case FADING_TO_RED_BATT:
    steps_fade_to_red = fade_to_red_batt(substripOut, steps_fade_to_red);
    break;
  case CHASING_YELLOW:
    steps_chase_yellow = chasing_yellow(substripOut, steps_chase_yellow);
    break;
  case CHASING_YELLOW_REVERSE:
    steps_chase_yellow_reverse = chasing_yellow_reverse(substripOut, steps_chase_yellow_reverse);
    break;
  case CHASING_BLUE:
    steps_chase_blue = chasing_blue(substripOut, steps_chase_blue);
    break;
  case FADING_YELLOW:
    steps_fade_yellow = fade_yellow(substripOut, steps_fade_yellow);
    break;
  case FADING_GREEN_MAISON:
    steps_fade_green_maison = fade_green_maison(substripOut, steps_fade_green_maison);
    break;
  case BLINK:
    blinkEffect(substripOut, 111);
    break;
  }
}

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

int chasing_blue_org(subStrips &substrip, int steps_chase_blue)
{
  steps_chase_blue++;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = (sin(dx + steps_chase_blue) * 127 + 128) / 6;
      if (color < 10)
      {
        color = 0;
      }
      strip.setPixelColor(dx, 0, color, color);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase_blue;
}

int chasing_blue(subStrips &substrip, int steps_chase_blue)
{
  steps_chase_blue--;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = (sin(dx + steps_chase_blue) * 127 + 128) / 6;
      if (color < 10)
      {
        color = 0;
      }
      strip.setPixelColor(dx, 0, color, color);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase_blue;
}

int chasing_yellow(subStrips &substrip, int steps_chase_yellow)
{
  steps_chase_yellow++;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = sin(dx + steps_chase_yellow) * 127 + 128;
      if (color < 120)
      {
        color = 0;
      }
      strip.setPixelColor(dx, color, color, 0);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase_yellow;
}

int chasing_yellow_reverse(subStrips &substrip, int steps_chase_yellow_reverse)
{
  steps_chase_yellow_reverse--;
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      int color = sin(dx + steps_chase_yellow_reverse) * 127 + 128;
      if (color < 120)
      {
        color = 0;
      }
      strip.setPixelColor(dx, color, color, 0);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display

  return steps_chase_yellow_reverse;
}

void constant(subStrips &substrip, uint32_t color)
{
  for (int i = 0; i < substrip.size_tab; i += 2)
  {
    for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
    {
      strip.setPixelColor(dx, color);
    }
  }
  strip.show();
  substrip.lastUpdate = millis(); // time for next change to the display
}

int fade_yellow(subStrips &substrip, int steps_fade_yellow)
{
  if (steps_fade_yellow < 50)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(50 - steps_fade_yellow, 50 - steps_fade_yellow, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_yellow += 3;
  }
  else
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(-50 + steps_fade_yellow, -50 + steps_fade_yellow, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_yellow += 3;
  }

  if (steps_fade_yellow > 100)
  {
    steps_fade_yellow = 0;
  }

  return steps_fade_yellow;
}

int fade_green_batt(subStrips &substrip, int steps_fade_to_green)
{
  if (steps_fade_to_green < 250)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(250, steps_fade_to_green, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_green += 6;
  }

  if (steps_fade_to_green > 250 && steps_fade_to_green < 500)
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
    steps_fade_to_green += 6;
  }
  return steps_fade_to_green;
}

int fade_green_maison(subStrips &substrip, int steps_fade_green_maison)
{
  if (steps_fade_green_maison < 250)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(0, 250 - steps_fade_green_maison, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_green_maison += 6;
  }
  else
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(0, -250 + steps_fade_green_maison, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_green_maison += 6;
  }

  if (steps_fade_green_maison > 500)
  {
    steps_fade_green_maison = 0;
  }

  return steps_fade_green_maison;
}

int fade_to_green_voiture(subStrips &substrip, int steps_fade_to_green_voiture)
{
  if (steps_fade_to_green_voiture < 250)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(250, steps_fade_to_green_voiture, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_green_voiture += 6;
  }

  if (steps_fade_to_green_voiture > 250 && steps_fade_to_green_voiture < 500)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(500 - steps_fade_to_green_voiture, 250, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_green_voiture += 6;
  }
  return steps_fade_to_green_voiture;
}

int fade_to_red_voiture(subStrips &substrip, int steps_fade_to_red_voiture)
{
  if (steps_fade_to_red_voiture < 250)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(steps_fade_to_red_voiture,250 , 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red_voiture += 6;
  }

  if (steps_fade_to_red_voiture > 250 && steps_fade_to_red_voiture < 500)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(250, 500 - steps_fade_to_red_voiture, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red_voiture += 6;
  }
  return steps_fade_to_red_voiture;
}

int fade_to_red_batt(subStrips &substrip, int steps_fade_to_red)
{
  if (steps_fade_to_red < 250)
  {
    // Serial.println(steps_fade_to_red);
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(steps_fade_to_red, 250, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red += 6;
  }

  if (steps_fade_to_red > 250 && steps_fade_to_red < 500)
  {
    for (int i = 0; i < substrip.size_tab; i += 2)
    {
      for (int dx = substrip.led[i]; dx < substrip.led[i + 1]; dx++)
      {
        strip.setPixelColor(dx, strip.Color(250, 500 - steps_fade_to_red, 0));
      }
    }
    strip.show();
    substrip.lastUpdate = millis(); // time for next change to the display
    steps_fade_to_red += 6;
  }
  return steps_fade_to_red;
}

/*
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
*/
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