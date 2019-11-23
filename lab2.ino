#include <LedControl.h>

const int sensorPins[] = {A0, A1, A2};
const float voltsPerMeasurement = 5.0/1024.0;
const int minDist = 10;

const byte ledPins[] = {22, 24, 26};

const int playTimeMs = 20000;

const int displaysCount = 1;
const int dataPin = 12;
const int clkPin = 10;
const int csPin = 11;
LedControl ledDisplay = LedControl(dataPin, clkPin, csPin, displaysCount);

const float timerPixelFadeOutTimeMs = playTimeMs / 64; // 312.5
int timerPixelsFadedOut = 0;
int timerCurrentRow = 7;
int timerCurrentCol = 7;

int score = 0;
int prevPin = -1;
int nextPin = -1;
int possibleNextPin = -1;

enum state { 
  WAIT_ACTION_FROM_USER,
  WAIT_REMOVE_HAND_FROM_USER,
  GENERATE_NEXT_PIN,
  END_GAME
};
state currentState = GENERATE_NEXT_PIN;

void setup()
{
  Serial.begin(115200);

  ledDisplay.shutdown(0, false);
  ledDisplay.setIntensity(0, 4);
  ledDisplay.clearDisplay(0);
  
  fillDisplay(true);
  currentState = GENERATE_NEXT_PIN;
}

void loop() {
  switch(currentState) {
    case GENERATE_NEXT_PIN:
      generate_next_pin();
      break;
    case WAIT_REMOVE_HAND_FROM_USER:
      wait_remove_hand_from_user();
      break;
    case WAIT_ACTION_FROM_USER:
      wait_action_from_user();
      break;
    case END_GAME:
      showScoreOnDisplay(score);
      delay(60000);
      break;
  }
  checkTime();
}

void generate_next_pin() {
  possibleNextPin = random(3);
  if (possibleNextPin == prevPin) {
    return;
  }
  nextPin = possibleNextPin;
  currentState = WAIT_ACTION_FROM_USER;
}

void wait_remove_hand_from_user() {
  if (readDist(0) <= minDist || readDist(1) <= minDist || readDist(2) <= minDist) {
    return;
  }
  currentState = GENERATE_NEXT_PIN;
}

void wait_action_from_user() {
  switchLed(nextPin, 255);
  if (readDist(nextPin) > minDist) {
    return;
  }
  switchLed(nextPin, 0);
  score += 1;
  currentState = WAIT_REMOVE_HAND_FROM_USER;
  prevPin = nextPin;
}

void showScoreOnDisplay(int score) {
  for (int row = 0; row < 8; row++) {
    for (int column = 0; column < 8; column++) {
      if (score <= 0)
        break;
      delay(70);
      score -= 1;
      ledDisplay.setLed(0, row, column, true);
    }
    if (score <= 0)
      break;
  }
}

void checkTime() {
  if (floor(millis() / timerPixelFadeOutTimeMs) > timerPixelsFadedOut) {
    ledDisplay.setLed(0, timerCurrentRow, timerCurrentCol, false);
    timerPixelsFadedOut += 1;
    timerCurrentCol -= 1;
    if (timerCurrentCol == -1) {
      timerCurrentCol = 7;
      timerCurrentRow -= 1;
    }
  }
  if (millis() >= playTimeMs) {
    currentState = END_GAME;
    switchLed(nextPin, 0);
  }
}

void fillDisplay(bool isOn) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ledDisplay.setLed(0, i, j, isOn);
    }
  }
}

void switchLed(int pin, int value) {
  analogWrite(ledPins[pin], 255 - value);
}

float readDist(int sensorPin){
  float volts = readAnalog(sensorPin) * voltsPerMeasurement;
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
