#include <Wire.h>
#include "SparkFun_Alphanumeric_Display.h"

HT16K33 display; // Create display object (default address 0x70)

void setup() {
  Serial.begin(9600);
  Serial.println("Qwiic Alphanumeric Display Test");

  // Initialize I2C for T-Dongle-S3 (SDA=GPIO43, SCL=GPIO44)
  Wire.begin(43, 44);

  // Initialize display
  if (display.begin() == false) {
    Serial.println("Display not found! Check wiring & I2C address (default 0x70).");
    while (1) delay(10);
  }
  Serial.println("Display initialized!");

  // Basic settings
  display.setBrightness(15); // 0-15 (max brightness)
  display.clear(); // Clear the display
  display.print("HELLO"); // Print a string (up to 8 chars)
  display.updateDisplay(); // Update the display
}

void loop() {
  // Update with a counter every 2 seconds
  static int counter = 0;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    counter++;
    char buf[9];
    sprintf(buf, "%d", counter % 100); // e.g., "NUM 42"
    display.clear();
    display.print(buf);
    display.updateDisplay();
    Serial.print("Displayed: ");
    Serial.println(buf);
    lastUpdate = millis();
  }

  delay(100);
}