#include "main.hpp"

const uint8_t outputCount = 8;
uint8_t outputs[outputCount] = {0, 1, 2, 3, 5, 6, 7, 8};

void setup()
{
  Wire.begin(I2C_ADRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(readEvent);

  SoftPWMBegin();

  resetPWM();
}

void loop()
{
  delay(10000);
}

void receiveEvent(int input)
{
  while (Wire.available() >= 2)
  {
    byte output = Wire.read();
    byte value = Wire.read();
    if (value == 0xf0)
    {
      resetPWM();
    }
    else if (output < outputCount)
    {
      SoftPWMSet(outputs[output], value);
    }
  }
}

void readEvent()
{
  Wire.write(I2C_ADRESS);
}

void resetPWM()
{
  for (int i = 0; i < outputCount; i++)
    SoftPWMSet(outputs[i], 0);
}