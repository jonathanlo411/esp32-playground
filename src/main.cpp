/*
 * ESP32 Button LED Blink
 *
 * Wiring:
 * - LED: GPIO 15 -> 220Ω resistor -> LED (+) -> LED (-) -> GND
 * - Button: GPIO 0 -> Button -> GND (uses internal pull-up)
 *
 * Hardware:
 * - ESP32 Dev Board
 * - LED (any color)
 * - 220Ω resistor
 * - Push button
 *
 * Behavior:
 * 1. Press button: LED blinks for 3 seconds
 * 2. Press during blink: Cancel blinking
 * 3. Hold button: LED continuously blinks while held
 */

#include <Arduino.h>
#include "led_helper.h"

const int LED_PIN = 15;
const int BUTTON_PIN = 0;

// Timing constants
const int BLINK_INTERVAL = 100;    // On/off time in ms
const int BLINK_DURATION = 3000;   // Total blink time in ms
const int HOLD_THRESHOLD = 350;    // Time to detect "hold" in ms
const int DEBOUNCE_DELAY = 50;     // Debounce time in ms

// LED controller
LEDController led(LED_PIN);

// Button state tracking
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long buttonPressTime = 0;
bool buttonHandled = false;
bool wasHolding = false;  // Track if we were in hold mode
unsigned long lastDebounceTime = 0;

void setup() {
  led.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println("ESP32 Button LED Controller Ready");
}

void loop() {
  // Read button with debouncing
  int reading = digitalRead(BUTTON_PIN);
  
  // If button state changed, reset debounce timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // Only update button state if debounce time has passed
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // If button state has actually changed
    if (reading != buttonState) {
      buttonState = reading;
      
      // Button was just pressed
      if (buttonState == LOW) {
        buttonPressTime = millis();
        buttonHandled = false;
        
        // If LED is already blinking, cancel it
        if (led.getIsBlinking()) {
          led.stopBlink();
          Serial.println("Blink cancelled");
          buttonHandled = true;  // Mark as handled so we don't start a new blink
        }
      }
      // Button was just released
      else {
        // If we were holding, turn off LED
        if (wasHolding) {
          led.stopBlink();
          Serial.println("Hold released - LED off");
          wasHolding = false;
          buttonHandled = true;
        }
        // If button wasn't held long enough and wasn't used to cancel
        else if (!buttonHandled && (millis() - buttonPressTime < HOLD_THRESHOLD)) {
          // Start timed blink
          led.startBlink(BLINK_DURATION, BLINK_INTERVAL, BLINK_INTERVAL);
          Serial.println("Timed blink started");
        }
        buttonHandled = false;
      }
    }
  }
  
  lastButtonState = reading;
  
  // Check if button is being held down
  if (buttonState == LOW && !buttonHandled && 
      (millis() - buttonPressTime >= HOLD_THRESHOLD)) {
    // Button is held - continuous blink mode
    if (led.getIsBlinking()) {
      led.stopBlink();  // Cancel any timed blink first
    }
    wasHolding = true;  // Mark that we're in hold mode
    led.continuousBlink(BLINK_INTERVAL, BLINK_INTERVAL);
  }
  
  // Update LED state (for timed blinking)
  led.update();
}