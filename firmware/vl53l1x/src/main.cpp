// Streams VL53L1X distance readings as one JSON object per line, about 30 Hz.
// Wiring: VIN-3V3, GND-GND, SDA-GPIO8, SCL-GPIO9 (ESP32-S3 Arduino defaults).
#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor;

// Explain why init failed: list I²C devices and the model ID at 0x29 (VL53L1X = 0xEACC).
void diagnoseBus() {
  Serial.printf("I2C scan on SDA=%d SCL=%d:", SDA, SCL);
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf(" 0x%02X", addr);
      found++;
    }
  }
  if (!found) Serial.print(" nothing answered (check power, GND, SDA/SCL pins, XSHUT not pulled low)");
  Serial.println();
  Wire.beginTransmission(0x29);
  Wire.write(0x01);  // model ID register 0x010F, 16-bit address
  Wire.write(0x0F);
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom((uint8_t)0x29, (uint8_t)2) == 2) {
    uint16_t id = (Wire.read() << 8) | Wire.read();
    Serial.printf("Device at 0x29 reports model ID 0x%04X (VL53L1X = 0xEACC, VL53L0X answers 0xEEAA here)\n", id);
  }
}

// If a sensor was reset mid-transfer it can hold SDA low forever. Clocking SCL frees it.
void recoverBus() {
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(5);
  if (digitalRead(SDA) == LOW) {
    pinMode(SCL, OUTPUT_OPEN_DRAIN);
    for (int i = 0; i < 9 && digitalRead(SDA) == LOW; i++) {
      digitalWrite(SCL, LOW); delayMicroseconds(5);
      digitalWrite(SCL, HIGH); delayMicroseconds(5);
    }
  }
  if (digitalRead(SDA) == LOW || digitalRead(SCL) == LOW) {
    Serial.printf("I2C line held low (SDA=%d SCL=%d). Check for a wire on the wrong pin or a short to GND.\n",
                  digitalRead(SDA), digitalRead(SCL));
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  recoverBus();
  Wire.begin();
  Wire.setTimeOut(20);  // a stuck bus fails fast instead of hanging the scan
  Wire.setClock(400000);

  sensor.setTimeout(500);
  while (!sensor.init()) {
    Serial.println("VL53L1X not found at 0x29. Check power, GND, SDA (GPIO8) and SCL (GPIO9).");
    diagnoseBus();
    delay(1000);
  }
  Serial.println("VL53L1X found.");
  // Long mode reaches ~4 m indoors (Short stops at ~1.3 m and flags farther targets as status 4).
  // 33 ms is Long mode's minimum timing budget, giving ~30 Hz.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(33000);
  sensor.startContinuous(33);
}

void loop() {
  uint16_t mm = sensor.read();  // blocks until the next measurement is ready
  if (sensor.timeoutOccurred()) {
    Serial.println("Timeout waiting for a reading.");
    return;
  }
  Serial.printf("{\"d\":%u,\"status\":%u,\"signal\":%.2f,\"ambient\":%.2f}\n",
                mm,
                (unsigned)sensor.ranging_data.range_status,
                sensor.ranging_data.peak_signal_count_rate_MCPS,
                sensor.ranging_data.ambient_count_rate_MCPS);
}
