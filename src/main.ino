#include <FastLED.h>

#define NUM_ROWS 10
#define NUM_COLUMNS 10
#define DATA_PIN 8

CRGB LEDs[NUM_ROWS * DATA_PIN];

void setup()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_ROWS * NUM_COLUMNS);
  FastLED.setBrightness(30);
}

void loop()
{

  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLUMNS; j++)
    {
      int ledPosition = getLedPosition(i, j);

      LEDs[ledPosition] = CRGB::Red;
      FastLED.show();
      delay(10);

      LEDs[ledPosition] = CRGB::Black;
      delay(10);
    }
  }

  delay(500);
}

int getLedPosition(int x, int y)
{
  if (x & 1)
  {
    return x * NUM_COLUMNS + (NUM_COLUMNS - y - 1);
  }
  else
  {
    return x * NUM_COLUMNS + y;
  }
}