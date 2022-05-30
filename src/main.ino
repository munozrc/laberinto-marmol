#include <FastLED.h>
#include <Wire.h>

// Direccion I2C de la IMU
#define MPU 0x68

// Ratios de conversion
#define A_R 16384.0 // 32768/2
#define G_R 131.0   // 32768/250

// Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

#define NUM_ROWS 10
#define NUM_COLUMNS 10

#define DATA_PIN 6

#define GOAL 2
#define WALL 5
#define OOOO 0 // ROAD

CRGB LEDs[NUM_ROWS * NUM_COLUMNS];
CRGB wallsColor = CRGB::Gray;
CRGB playerColor = CRGB::OrangeRed;

int playerPositionX = 0;
int playerPositionY = 0;
int levelGame = 0;

// MPU-6050 da los valores en enteros de 16 bits
// Valores RAW
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

// Angulos
float Acc[2];
float Gy[3];
float Angle[3];

float dt;

int board[2][NUM_ROWS][NUM_COLUMNS] = {{{OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, GOAL}},
                                       {{OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, WALL, OOOO, OOOO, OOOO},
                                        {WALL, WALL, WALL, WALL, OOOO, WALL, WALL, OOOO, WALL, OOOO},
                                        {OOOO, OOOO, OOOO, OOOO, OOOO, OOOO, WALL, OOOO, WALL, OOOO},
                                        {WALL, WALL, WALL, OOOO, WALL, WALL, WALL, OOOO, WALL, OOOO},
                                        {WALL, OOOO, OOOO, OOOO, WALL, OOOO, OOOO, OOOO, WALL, OOOO},
                                        {OOOO, OOOO, WALL, WALL, WALL, OOOO, WALL, WALL, WALL, OOOO},
                                        {OOOO, WALL, OOOO, OOOO, OOOO, OOOO, WALL, OOOO, OOOO, OOOO},
                                        {OOOO, WALL, WALL, OOOO, WALL, WALL, WALL, OOOO, OOOO, WALL},
                                        {OOOO, OOOO, WALL, OOOO, WALL, OOOO, OOOO, OOOO, WALL, GOAL},
                                        {WALL, OOOO, OOOO, OOOO, WALL, OOOO, WALL, OOOO, OOOO, OOOO}}};

void setup()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_ROWS * NUM_COLUMNS);
  FastLED.setBrightness(200);
  FastLED.clear();

  Wire.begin(); // A4=SDA / A5=SCL
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.begin(9600);
}

unsigned int speed = 100;
unsigned long timestamp = 0;
unsigned long timestampWire = 0;

void loop()
{

  // Leer los valores del Acelerometro de la IMU
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Pedir el registro 0x3B - corresponde al AcX
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);       // A partir del 0x3B, se piden 6 registros
  AcX = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // A partir de los valores del acelerometro, se calculan los angulos Y, X
  // respectivamente, con la formula de la tangente.
  Acc[1] = atan(-1 * (AcX / A_R) / sqrt(pow((AcY / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;
  Acc[0] = atan((AcY / A_R) / sqrt(pow((AcX / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;

  // Leer los valores del Giroscopio
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);       // A partir del 0x43, se piden 6 registros
  GyX = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Calculo del angulo del Giroscopio
  Gy[0] = GyX / G_R;
  Gy[1] = GyY / G_R;
  Gy[2] = GyZ / G_R;

  dt = (millis() - timestampWire) / 1000.0;
  timestampWire = millis();

  // Aplicar el Filtro Complementario
  Angle[0] = 0.98 * (Angle[0] + Gy[0] * dt) + 0.02 * Acc[0];
  Angle[1] = 0.98 * (Angle[1] + Gy[1] * dt) + 0.02 * Acc[1];

  // Integración respecto del tiempo paras calcular el YAW
  Angle[2] = Angle[2] + Gy[2] * dt;

  float xValue = Angle[0];
  float yValue = Angle[1];
  float zValue = Angle[2];

  int directionX = getAxisValue(xValue, -31, 10);
  int directionY = getAxisValue(yValue, 2, 5);

  Serial.println("x= " + String(xValue, DEC) + " y=" + String(yValue, DEC));

  if (millis() > timestamp + speed)
  {
    timestamp = millis();

    int posX = playerPositionX + directionX;
    int posY = playerPositionY + directionY;
    int cell = board[levelGame][posX][posY];

    if (cell == GOAL)
    {
      levelGame < 2 ? levelGame++ : 0;
      playerPositionX = 0;
      playerPositionY = 0;
    }
    else if (cell == OOOO)
    {
      if (posX <= 9 && posX >= 0)
        playerPositionX = posX;
      if (posY <= 9 && posY >= 0)
        playerPositionY = posY;
    }
  }

  for (int row = 0; row < NUM_ROWS; row++)
  {
    for (int column = 0; column < NUM_COLUMNS; column++)
    {

      if (board[levelGame][row][column] == GOAL)
      {
        LEDs[getLedPosition(row, column)] = CRGB::Blue;
      }

      if (board[levelGame][row][column] == WALL)
      {
        LEDs[getLedPosition(row, column)] = wallsColor;
      }

      if (board[levelGame][row][column] == OOOO)
      {
        LEDs[getLedPosition(row, column)] = CRGB::Black;
      }
    }
  }

  LEDs[getLedPosition(playerPositionX, playerPositionY)] = playerColor;

  FastLED.show();
}

int getAxisValue(float raw, int originAxis, int axisThreshold)
{
  int value = 0;

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