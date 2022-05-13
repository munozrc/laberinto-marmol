#include <FastLED.h>

#define NUM_ROWS 10
#define NUM_COLUMNS 10
#define DATA_PIN 6
#define JOYSTICK_X_PIN A0
#define JOYSTICK_Y_PIN A1

CRGB LEDs[NUM_ROWS * DATA_PIN];

int playerPositionX = 0;
int playerPositionY = 0;

void setup()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_ROWS * NUM_COLUMNS);
  FastLED.setBrightness(10);
  FastLED.clear();
}

unsigned int speed = 100;
unsigned long timestamp = 0;

void loop()
{

  int directionX = getAxisValue(JOYSTICK_X_PIN, 500, 160);
  int directionY = getAxisValue(JOYSTICK_Y_PIN, 500, 160);

  if (millis() > timestamp + speed)
  {
    timestamp = millis();

    int posX = playerPositionX + directionX;
    int posY = playerPositionY + directionY;

    if (posX != playerPositionX || posY != playerPositionY)
    {
      LEDs[getLedPosition(playerPositionX, playerPositionY)] = CRGB::Black;
    }

    if (posX <= 9 && posX >= 0)
    {
      playerPositionX += directionX;
    }

    if (posY <= 9 && posY >= 0)
    {
      playerPositionY += directionY;
    }
  }

  LEDs[getLedPosition(playerPositionX, playerPositionY)] = CRGB::Blue;
  FastLED.show();
}

int getAxisValue(uint8_t pin, int originAxis, int axisThreshold)
{
  int value = 0;
  int raw = analogRead(pin);

  raw < originAxis - axisThreshold ? value = 1 : 0;
  raw > originAxis + axisThreshold ? value = -1 : 0;

  return value;
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