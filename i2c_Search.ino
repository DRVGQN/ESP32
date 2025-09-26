// #include <Wire.h>

// // int sdaPins[] = {8, 4, 6, 7, 38};   // candidate SDA pins
// // int sclPins[] = {9, 5, 7, 15, 39};  // candidate SCL pins

// void setup() {
//   Serial.begin(115200);
//   while (!Serial) delay(10);
//   Serial.println("\nStarting ESP32-S3 I2C Bus Search...");

//   Wire.begin(43, 44);

//   // for (int i = 0; i < sizeof(sdaPins)/sizeof(int); i++) {
//   //   for (int j = 0; j < sizeof(sclPins)/sizeof(int); j++) {
//   //     if (sdaPins[i] == sclPins[j]) continue; // skip invalid pair

//   //     Serial.printf("Testing SDA=%d SCL=%d ...\n", sdaPins[i], sclPins[j]);
//       // Wire.begin(sdaPins[i], sclPins[j]);
//       // byte error, address;
//   //     int nDevices = 0;


//   //     if (nDevices == 0) {
//   //       Serial.println("  No devices found.");
//   //     }
//   //     Serial.println();
//   //   }
//   // }
// }

// void loop() {
//   Serial.println();
//   byte error;
//   int nDevices = 0;
//       for (byte address = 1; address < 127; address++) {
//         Wire.beginTransmission(address);
//         error = Wire.endTransmission();
//         if (error == 0) {
//           Serial.printf("  Device found at 0x%02X\n", address);
//           nDevices++;
//         } else {
//           Serial.printf(".");
//         }
//       }
// }


#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for serial port to connect
  Serial.println("\nI2C Scanner for T-Dongle-S3");
  Wire.begin(43, 44);  // SDA=43, SCL=44 for T-Dongle-S3
}

void loop() {
  byte error, address;
  int nDevices = 0;
  delay(500);  // Scan every 500 mseconds (remove for one-time run)
  Serial.println("Scanning I2C bus...");

  // Print header similar to i2cdetect
  Serial.print("     00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f");
  Serial.println();
  for (byte row = 0; row < 10; row++) {  // Rows 0x00 to 0x90 (up to 0x77)
    Serial.print("0");
    if (row < 10) Serial.print(row);
    else Serial.print((char)('a' + row - 10));
    Serial.print("  ");
    for (byte col = 0; col < 16; col++) {
      if ((col == 0 && row < 8) || (row == 9 && col > 7)) {  // Reserved or out of range
        Serial.print("--  ");
      } else {
        address = row * 16 + col;
        if (address > 0x77) {
          Serial.print("--  ");
        } else {
          Wire.beginTransmission(address);
          error = Wire.endTransmission();
          if (error == 0) {
            Serial.print(address, HEX);
            Serial.print("  ");
            nDevices++;
          } else {
            Serial.print("--  ");
          }
        }
      }
    }
    Serial.println();
  }
  Serial.print("Found ");
  Serial.print(nDevices);
  Serial.println(" device(s).");
  Serial.println("Scanning complete.\n");

  
}