// led_helper.cpp
// Implementation of LED control functions

#include "led_helper.h"

LEDController::LEDController(int pin) {
  ledPin = pin;
  isBlinking = false;
  ledState = false;
  blinkStartTime = 0;
  lastToggleTime = 0;
  onTimeMs = 0;
  offTimeMs = 0;
  totalDurationMs = 0;
}

void LEDController::begin() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void LEDController::startBlink(int durationMs, int onMs, int offMs) {
  isBlinking = true;
  blinkStartTime = millis();
  lastToggleTime = millis();
  totalDurationMs = durationMs;
  onTimeMs = onMs;
  offTimeMs = offMs;
  ledState = false;
  digitalWrite(ledPin, LOW);
}

void LEDController::stopBlink() {
  isBlinking = false;
  ledState = false;
  digitalWrite(ledPin, LOW);
}

void LEDController::update() {
  if (!isBlinking) {
    return;
  }
  
  unsigned long currentTime = millis();
  
  // Check if total duration has elapsed
  if (currentTime - blinkStartTime >= totalDurationMs) {
    stopBlink();
    return;
  }
  
  // Handle LED toggling
  unsigned long targetInterval = ledState ? onTimeMs : offTimeMs;
  
  if (currentTime - lastToggleTime >= targetInterval) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    lastToggleTime = currentTime;
  }
}

bool LEDController::getIsBlinking() {
  return isBlinking;
}

void LEDController::continuousBlink(int onMs, int offMs) {
  unsigned long currentTime = millis();
  unsigned long targetInterval = ledState ? onMs : offMs;
  
  if (currentTime - lastToggleTime >= targetInterval) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    lastToggleTime = currentTime;
  }
}