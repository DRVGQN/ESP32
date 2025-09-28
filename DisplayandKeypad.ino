#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "SparkFun_Alphanumeric_Display.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();
HT16K33 display; // Display object (default address 0x70)

// Keypad mapping for 3x4 layout
const char keyMap[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

// Buffer to store up to 4 digits
char inputBuffer[5] = ""; // Null-terminated string, max 4 digits
uint8_t inputLength = 0;

void dump_regs() {
  Serial.println("========================================");
  Serial.println("CHAN 00 01 02 03 04 05 06 07 08 09 10 11");
  Serial.println(" -- -- -- -- -- -- -- -- -- -- -- --");
  // CDC
  Serial.print("CDC: ");
  for (int chan = 0; chan < 12; chan++) {
    uint8_t reg = cap.readRegister8(0x5F + chan);
    if (reg < 10) Serial.print(" ");
    Serial.print(reg);
    Serial.print(" ");
  }
  Serial.println();
  // CDT
  Serial.print("CDT: ");
  for (int chan = 0; chan < 6; chan++) {
    uint8_t reg = cap.readRegister8(0x6C + chan);
    uint8_t cdtx = reg & 0b111;
    uint8_t cdty = (reg >> 4) & 0b111;
    if (cdtx < 10) Serial.print(" ");
    Serial.print(cdtx);
    Serial.print(" ");
    if (cdty < 10) Serial.print(" ");
    Serial.print(cdty);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("========================================");
}

void updateDisplayWithBuffer() {
  display.clear();
  display.print(inputBuffer);
  display.updateDisplay();
}

void setup() {
  Serial.begin(9600);
  Serial.println("MPR121 Keypad with Display Test Start");
  while (!Serial) delay(10);
  Serial.println("Serial initialized");

  // Initialize I2C for T-Dongle-S3 (SDA=GPIO43, SCL=GPIO44)
  Wire.begin(43, 44);
  Serial.println("I2C initialized");

  // Test I2C communication to MPR121 (0x5A)
  Wire.beginTransmission(0x5A);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("MPR121 detected at 0x5A");
  } else {
    Serial.print("MPR121 error at 0x5A: ");
    Serial.println(error);
  }

  // Initialize MPR121 with autoconfig
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 initialization failed! Check wiring, power, or I2C address.");
    while (1) delay(10);
  }
  Serial.println("MPR121 initialized successfully!");

  Serial.println("Initial CDC/CDT values:");
  dump_regs();
  cap.setAutoconfig(true);
  Serial.println("After autoconfig CDC/CDT values:");
  dump_regs();

  // Initialize Display (0x70)
  if (display.begin() == false) {
    Serial.println("Display not found! Check wiring & I2C address (default 0x70).");
    while (1) delay(10);
  }
  Serial.println("Display initialized!");
  display.setBrightness(15); // Max brightness
  display.clear();
  display.print("INIT");
  display.updateDisplay();
  delay(1000); // Show "INIT" for 1 second
  display.clear();
  display.updateDisplay();
}

void loop() {
  // Read touched electrodes
  uint16_t touched = cap.touched();
  static uint16_t lastTouched = 0;
  if (touched != lastTouched) {
    for (uint8_t i = 0; i < 12; i++) {
      if ((touched & _BV(i)) && !(lastTouched & _BV(i))) { // Key pressed
        char key = keyMap[i];
        if (key == '*') { // Clear
          inputBuffer[0] = '\0';
          inputLength = 0;
          Serial.println("Cleared");
          Serial.print("Current input: ");
          Serial.println(inputBuffer);
          updateDisplayWithBuffer();
        } else if (key == '#') { // Enter
          if (inputLength == 4) {
            Serial.print("Entered: ");
            Serial.println(inputBuffer);
            // Show "OK" briefly on display
            display.clear();
            display.print("OK  ");
            display.updateDisplay();
            delay(1000); // Flash for 1 second
            inputBuffer[0] = '\0';
            inputLength = 0;
            updateDisplayWithBuffer();
          } else {
            Serial.println("Error: Enter exactly 4 digits!");
            Serial.print("Current input: ");
            Serial.println(inputBuffer);
            // Show "ERR" briefly on display
            display.clear();
            display.print("ERR ");
            display.updateDisplay();
            delay(1000); // Flash for 1 second
            updateDisplayWithBuffer();
          }
        } else if (inputLength < 4) { // Digit (0-9)
          inputBuffer[inputLength] = key;
          inputLength++;
          inputBuffer[inputLength] = '\0'; // Null-terminate
          Serial.print("Current input: ");
          Serial.println(inputBuffer);
          updateDisplayWithBuffer();
        }
      }
    }
    lastTouched = touched;
  }

  // Debug: Print raw electrode data every 5 seconds
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 5000) {
    Serial.println("Raw electrode data:");
    for (uint8_t i = 0; i < 12; i++) {
      Serial.print("Electrode ");
      Serial.print(i);
      Serial.print(" (");
      Serial.print(keyMap[i]);
      Serial.print("): ");
      Serial.print(cap.filteredData(i));
      Serial.print(" (baseline: ");
      Serial.print(cap.baselineData(i));
      Serial.println(")");
    }
    Serial.print("Current input: ");
    Serial.println(inputBuffer);
    lastDebug = millis();
  }

  delay(100); // Poll every 100ms
}