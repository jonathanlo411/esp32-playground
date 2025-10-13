#include <Arduino.h>

const int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(5000);       // LED on for 5 seconds
  digitalWrite(ledPin, LOW);
  delay(5000);       // LED off for 5 seconds
}
