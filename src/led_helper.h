// led_helper.h
// Header file for LED control functions

#ifndef LED_HELPER_H
#define LED_HELPER_H

#include <Arduino.h>

// Class to manage LED blinking with cancellation support
class LEDController {
private:
  int ledPin;
  bool isBlinking;
  unsigned long blinkStartTime;
  unsigned long lastToggleTime;
  int onTimeMs;
  int offTimeMs;
  int totalDurationMs;
  bool ledState;

public:
  // Constructor
  LEDController(int pin);
  
  // Initialize the LED pin
  void begin();
  
  // Start a timed blink sequence
  void startBlink(int durationMs, int onMs, int offMs);
  
  // Stop any active blinking
  void stopBlink();
  
  // Update function - call this in loop() to handle non-blocking blink
  void update();
  
  // Check if currently blinking
  bool getIsBlinking();
  
  // Continuous blink mode (for hold functionality)
  void continuousBlink(int onMs, int offMs);
};

#endif