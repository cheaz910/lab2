#include <LedControl.h>

const int sensorPins[] = {A0, A1, A2};
const float voltsPerMeasurement = 5.0/1024.0;
const int minDist = 10;

const byte ledPins[] = {2, 3, 4};

const int playTimeMs = 20000;

const int displaysCount = 1;
const int dataPin = 12;
const int clkPin = 10;
const int csPin = 11;
LedControl ledDisplay = LedControl(dataPin, clkPin, csPin, displaysCount);

void setup()
{
  Serial.begin(115200);

  ledDisplay.shutdown(0, false);
  ledDisplay.setIntensity(0, 16);
  ledDisplay.clearDisplay(0);
}

void loop() {
  int score = 0;
  while (millis() < playTimeMs) {
    int nextSensorPin = random(3);
    score += playPart(nextSensorPin);
  }
  Serial.println(score);
  showScoreOnDisplay(score);
  delay(10000);
  ledDisplay.clearDisplay(0);
}

void showScoreOnDisplay(int score) {
  for (int row = 0; row < 8; row++) {
    for (int column = 0; column < 8; column++) {
      ledDisplay.setLed(0, row, column, true);
    }
  }
}

int playPart(int pin) {
  switchLed(pin, 255);
  while (readDist(pin) > minDist && millis() < playTimeMs) {
    delay(20);
  }
  switchLed(pin, 0);
  if (millis() > playTimeMs) {
    return 0;
  }
  return 1;
}

void switchLed(int pin, int value) {
  analogWrite(ledPins[pin], 255 - value);
}

float readDist(int sensorPin){
  float volts = readAnalog(sensorPin) * voltsPerMeasurement;
  // return 65 * pow(volts, -1.10); // for big IR sensor (SHARP 2Y0A02)
  return pow(14.7737/volts, 1.2134); // for small IR sensor (SHARP 2Y0A21)
}

float readAnalog(int sensorPin)
{
  int sum = 0;
  int maxV = -5000;
  int minV = 5000;
  int n = 15;
  for (int i = 0; i < n; i++)
  {
    int cur = analogRead(sensorPin);
    if (cur > maxV)
    {
      maxV = cur;
    }
    if (cur < minV)
    {
      minV = cur;
    }
    sum += cur;
    delay(6);
  }
  return (sum - maxV - minV) / (float)(n - 2);
}
