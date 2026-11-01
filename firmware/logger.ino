// Raw sensor data logger for the Rev 2 board. Not tested on hardware yet.
// Board: ESP32C3 Dev Module, "USB CDC On Boot: Enabled".
// Streams CSV at 100 Hz: ms,ax_g,ay_g,az_g,hall,label
// hall = 1 when a magnet is near (DRV5032FA output is active low).
// Type a letter in the serial monitor to set the label column.
#include <Wire.h>
#define SDA_PIN 8
#define SCL_PIN 7
#define HALL_PIN 4
#define ADXL 0x53
char label = 'x';

void wr(uint8_t r, uint8_t v){ Wire.beginTransmission(ADXL); Wire.write(r); Wire.write(v); Wire.endTransmission(); }

void setup(){
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);
  pinMode(HALL_PIN, INPUT);
  wr(0x2C, 0x0A);   // BW_RATE 100 Hz
  wr(0x31, 0x09);   // full resolution, +/-4 g
  wr(0x2D, 0x08);   // measure
  delay(100);
  Serial.println("ms,ax,ay,az,hall,label");
}

void loop(){
  static uint32_t next = 0;
  while (Serial.available()) { char c = Serial.read(); if (c > ' ') label = c; }
  if (millis() < next) return;
  next = millis() + 10;
  Wire.beginTransmission(ADXL); Wire.write(0x32); Wire.endTransmission(false);
  if (Wire.requestFrom(ADXL, 6) != 6) return;
  int16_t x = Wire.read() | (Wire.read() << 8);
  int16_t y = Wire.read() | (Wire.read() << 8);
  int16_t z = Wire.read() | (Wire.read() << 8);
  const float s = 0.0078f; // g per LSB at +/-4 g full resolution
  Serial.printf("%lu,%.3f,%.3f,%.3f,%d,%c\n", millis(), x*s, y*s, z*s, digitalRead(HALL_PIN)==LOW, label);
}
