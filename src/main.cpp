#include <Arduino.h>

// ESP32 Button Controlled LED Blink
// Press button to make LED blink for 5 seconds

const int LED_PIN = 15;      // Built-in LED on most ESP32 boards
const int BUTTON_PIN = 0;  // Button connected to GPIO 15

void setup() {
  // Set up the LED pin as output
  pinMode(LED_PIN, OUTPUT);
  
  // Set up the button pin as input with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Make sure LED starts off
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Read the button state (LOW when pressed due to pull-up)
  int buttonState = digitalRead(BUTTON_PIN);
  
  // Check if button is pressed
  if (buttonState == LOW) {
    // Blink for 5 seconds total (5 on/off cycles = 5 seconds)
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, HIGH);  // Turn LED on
      delay(500);                   // Wait 0.5 seconds
      digitalWrite(LED_PIN, LOW);   // Turn LED off
      delay(500);                   // Wait 0.5 seconds
    }
    
    // Small delay to prevent multiple triggers from one press
    delay(200);
  }
}