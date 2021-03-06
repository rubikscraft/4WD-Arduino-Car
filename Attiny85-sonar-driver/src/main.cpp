#include "main.hpp"

const int16_t maxDistanceDefault = 100;
const int16_t servoPositionDefault = 90;
const int16_t fetchSpeedDefault = -1;
const int16_t averageAmountDefault = 1;
const int16_t soundVelocityDefault = 343;

int16_t maxDistance = maxDistanceDefault;
int16_t servoPosition = servoPositionDefault;
int16_t fetchSpeed = fetchSpeedDefault;
int16_t currentDistance = maxDistanceDefault;
int16_t averageAmount = averageAmountDefault;
int16_t soundVelocity = soundVelocityDefault;

Servo servo;

void setup()
{
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(pingPin, LOW);

  servo.attach(servoPin);

  Wire.begin(I2C_ADRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(readEvent);
}

void loop()
{
  if (fetchSpeed >= 0)
  {
    currentDistance = getSonar();
    delay(fetchSpeed);
  }
  else
  {
    delay(10);
  }
}

void receiveEvent(int howMany)
{
  while (Wire.available() >= 3)
  {
    uint8_t command = Wire.read();
    uint8_t most = Wire.read();
    uint8_t least = Wire.read();

    int16_t value = (most << 8) | least;

    switch (command)
    {
    case 1:
      fetchSpeed = value;
      break;
    case 2:
      maxDistance = value;
    case 3:
      servoPosition = value;
      servo.write(servoPosition);
      break;
    case 4:
      averageAmount = value;
      if (averageAmount < 1)
        averageAmount = 1;
      break;
    case 5:
      soundVelocity = value;
      if (soundVelocity < 1)
        soundVelocity = 1;
      break;
    case 0xf0:
      maxDistance = maxDistanceDefault;
      servoPosition = servoPositionDefault;
      fetchSpeed = fetchSpeedDefault;
      currentDistance = maxDistanceDefault;
      averageAmount = averageAmountDefault;
      soundVelocity = soundVelocityDefault;
      servo.write(servoPosition);
      break;
    case 0xff:
      currentDistance = value;
      break;
    }
  }
}

void readEvent()
{
  Wire.write(I2C_ADRESS);
  sendInt16(currentDistance);
  sendInt16(fetchSpeed);
  sendInt16(averageAmount);
  sendInt16(maxDistance);
  sendInt16(servoPosition);
  sendInt16(soundVelocity);
}

void sendInt16(int16_t value)
{
  // Yes this gives a narrowing conversion, thats the point
  uint8_t send[2] = {
      value >> 8,
      value};
  Wire.write(send, 2);
}

uint16_t getSonar()
{
  int tempDistance = 0;
  for (int16_t i = 0; i < averageAmount; i++)
  {
    unsigned long pingTime;

    digitalWrite(pingPin, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);

    pingTime = pulseIn(echoPin, HIGH, SONIC_TIMEOUT); // Wait HC-SR04 returning to the high level and measure out this waitting time
    if (pingTime != 0)
      tempDistance += pingTime * soundVelocity / 10000; // calculate the distance according to the time
    else
      tempDistance += maxDistance;

    delay(1);
  }

  return tempDistance / averageAmount;
}