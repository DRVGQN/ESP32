#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();

// Keypad mapping for 3x4 layout (adjust if your keypad differs)
const char keyMap[12] = {'0','1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

void dump_regs() {
  Serial.println("========================================");
  Serial.println("CHAN 00 01 02 03 04 05 06 07 08 09 10 11");
  Serial.println("     -- -- -- -- -- -- -- -- -- -- -- --"); 
  // CDC
  Serial.print("CDC: ");
  for (int chan=0; chan<12; chan++) {
    uint8_t reg = cap.readRegister8(0x5F+chan);
    if (reg < 10) Serial.print(" ");
    Serial.print(reg);
    Serial.print(" ");
  }
  Serial.println();
  // CDT
  Serial.print("CDT: ");
  for (int chan=0; chan<6; chan++) {
    uint8_t reg = cap.readRegister8(0x6C+chan);
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

void setup() {
  Serial.begin(9600);
  Serial.println("MPR121 Test Start");
  while (!Serial) delay(10);
  Serial.println("Serial initialized");

  // Initialize I2C for T-Dongle-S3 (SDA=GPIO43, SCL=GPIO44)
  Wire.begin(43, 44);
  Serial.println("I2C initialized");

  // Test I2C communication to 0x5A
  Wire.beginTransmission(0x5A);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("I2C device detected at 0x5A");
  } else {
    Serial.print("I2C error at 0x5A: ");
    Serial.println(error);
  }

  // Initialize MPR121 with custom thresholds
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

   //Set touch and release thresholds for all 12 electrodes
 //for (uint8_t i = 0; i < 12; i++) {
    //cap.setThresholds(8, 10); // Touch: 12, Release: 6 (tweak if needed)
 // cap.setThresholds(, 10); // Touch: 12, Release: 6 (tweak if needed)
  //}
}

void loop() {
  // Read touched electrodes
  uint16_t touched = cap.touched();
  static uint16_t lastTouched = 0;
  if (touched != lastTouched) {
    if (touched) {
      Serial.print("Touched keys: ");
      for (uint8_t i = 0; i < 12; i++) {
        if (touched & _BV(i)) {
          Serial.print(keyMap[i]);
          Serial.print(" (electrode ");
          Serial.print(i);
          Serial.print(") ");
        }
      }
      Serial.println();
    } else {
      Serial.println("Released");
    }
    lastTouched = touched;
  }

  // Debug: Print raw filtered data for all electrodes every 5 seconds
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
    lastDebug = millis();
  }

  //delay(10); // Poll every 100ms
}