#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // ADS1115 instance (default address 0x48)

const float VREF = 3.3;       // Reference voltage (3.3V for ESP32-S3)
const float MAX_ANGLE = 300.0; // Max angle for sensor (adjust if needed, e.g., 300° for KY-031)

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor
  Wire.begin(43, 44); // SDA=GPIO43, SCL=GPIO44 for T-Dongle-S3

  Serial.println("\n=== ADS1115 Rotary Angle Reader ===");
  if (!ads.begin()) {
    Serial.println("ERROR: ADS1115 not found at 0x48. Check wiring/address.");
    while (1);
  }
  ads.setGain(GAIN_ONE); // ±4.096V range (suitable for 0-3.3V)
  Serial.println("ADS1115 initialized. Reading A0...\n");
  Serial.println("ADC\tVoltage\tAngle");
  Serial.println("--------------------------------");
}

void loop() {
  int16_t adcValue = ads.readADC_SingleEnded(0); // Read A0 (single-ended)
  float voltage = ads.computeVolts(adcValue);    // Convert to volts
  float angle = (voltage / VREF) * MAX_ANGLE;    // Map voltage to angle

  // Print in tabular format
  Serial.print(adcValue);
  Serial.print("\t");
  Serial.print(voltage, 3);
  Serial.print(" V\t");
  Serial.print(angle, 1);
  Serial.println(" °");

  delay(10); // Update every 500ms
}