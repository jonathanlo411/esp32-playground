/*
 * ESP32 Button LED Blink with Interrupts and Debouncing
 *
 * Wiring:
 * - LED: GPIO 2 -> 220Ω resistor -> LED (+) -> LED (-) -> GND
 * - Button: GPIO 4 -> Button -> GND (uses internal pull-up)
 *
 * Behavior:
 * 1. Press button: LED blinks for 3 seconds
 * 2. Press during blink: Cancel blinking
 * 3. Hold button: LED continuously blinks while held
 */
#include <Arduino.h>
#include "led_helper.h"

const int LED_PIN = 2;
const int BUTTON_PIN = 4;

// Timing constants
const int BLINK_INTERVAL = 75;
const int BLINK_DURATION = 9000;
const int HOLD_THRESHOLD = 350;
const int DEBOUNCE_DELAY = 50;

// LED controller
LEDController led(LED_PIN);

// Volatile variables (accessed in ISR and main loop)
volatile bool buttonEventFlag = false;
volatile unsigned long lastInterruptTime = 0;

// State tracking
bool lastStableButtonState = HIGH;
bool buttonHandled = false;
bool wasHolding = false;
unsigned long buttonPressTime = 0;

// ISR (Interrupt Service Routine) - keep it SHORT and FAST
void IRAM_ATTR handleButtonInterrupt() {
  unsigned long currentTime = millis();
  
  // Debounce in ISR - ignore if too soon after last interrupt
  if (currentTime - lastInterruptTime > DEBOUNCE_DELAY) {
    lastInterruptTime = currentTime;
    buttonEventFlag = true;
  }
}

void setup() {
  led.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Attach interrupt on CHANGE (both rising and falling edges)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, CHANGE);
  
  Serial.begin(115200);
  Serial.println("ESP32 Button LED Controller Ready (Interrupt Mode)");
}

void loop() {
  // Check if button event occurred
  if (buttonEventFlag) {
    buttonEventFlag = false;
    
    // Read current stable button state
    delay(5); // Small delay to let button settle
    bool currentButtonState = digitalRead(BUTTON_PIN);
    
    // Only process if state actually changed
    if (currentButtonState != lastStableButtonState) {
      lastStableButtonState = currentButtonState;
      
      // Button just pressed (LOW)
      if (currentButtonState == LOW) {
        Serial.println("Button pressed");
        buttonPressTime = millis();
        buttonHandled = false;
        
        // If LED is already blinking, cancel it
        if (led.getIsBlinking()) {
          led.stopBlink();
          Serial.println("Blink cancelled");
          buttonHandled = true;
        }
      }
      // Button just released (HIGH)
      else {
        unsigned long pressDuration = millis() - buttonPressTime;
        
        // If we were holding, turn off LED
        if (wasHolding) {
          led.stopBlink();
          Serial.println("Hold released - LED off");
          wasHolding = false;
          buttonHandled = true;
        }
        // Short press - start timed blink
        else if (!buttonHandled && pressDuration < HOLD_THRESHOLD) {
          led.startBlink(BLINK_DURATION, BLINK_INTERVAL, BLINK_INTERVAL);
          Serial.println("Timed blink started");
        }
        
        buttonHandled = false;
      }
    }
  }
  
  // Check for hold condition (button still pressed after threshold)
  if (lastStableButtonState == LOW && !buttonHandled) {
    unsigned long holdTime = millis() - buttonPressTime;
    
    if (holdTime >= HOLD_THRESHOLD) {
      if (!wasHolding) {
        // First time detecting hold
        if (led.getIsBlinking()) {
          led.stopBlink();
        }
        wasHolding = true;
        Serial.println("Hold detected - continuous blink");
      }
      // Keep the continuous blink active while holding
      led.continuousBlink(BLINK_INTERVAL, BLINK_INTERVAL);
    }
  }
  
  // Update LED state
  led.update();
}