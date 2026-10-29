// Streams VL53L1X distance readings as one JSON object per line, about 50 Hz.
// Wiring: VIN-3V3, GND-GND, SDA-GPIO8, SCL-GPIO9 (ESP32-S3 Arduino defaults).
#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  sensor.setTimeout(500);
  while (!sensor.init()) {
    Serial.println("VL53L1X not found. Check wiring.");
    delay(1000);
  }
  // Short mode (up to ~1.3 m) handles room light best; 20 ms budget gives ~50 Hz.
  sensor.setDistanceMode(VL53L1X::Short);
  sensor.setMeasurementTimingBudget(20000);
  sensor.startContinuous(20);
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
