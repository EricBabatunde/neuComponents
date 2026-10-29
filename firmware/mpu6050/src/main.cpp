// Streams MPU6050 orientation as one JSON object per line at 50 Hz.
// Wiring: VCC-3V3, GND-GND, SDA-GPIO8, SCL-GPIO9 (ESP32-S3 Arduino defaults).
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

constexpr uint32_t PERIOD_MS = 20;   // 50 Hz
constexpr float ALPHA = 0.98f;       // complementary filter: share given to the gyro
constexpr int CALIBRATION_SAMPLES = 200;

float roll = 0, pitch = 0, yaw = 0;
float biasX = 0, biasY = 0, biasZ = 0;  // gyro offsets at rest, rad/s
uint32_t lastMicros = 0;

void calibrateGyro() {
  // Average the gyro while the board is still; that average is its offset.
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);
    biasX += g.gyro.x;
    biasY += g.gyro.y;
    biasZ += g.gyro.z;
    delay(10);
  }
  biasX /= CALIBRATION_SAMPLES;
  biasY /= CALIBRATION_SAMPLES;
  biasZ /= CALIBRATION_SAMPLES;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  while (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    delay(1000);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("Calibrating gyro, keep the sensor still...");
  calibrateGyro();

  // Start from the accelerometer's angle so the plane does not swing in from zero.
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  roll = atan2f(a.acceleration.y, a.acceleration.z) * RAD_TO_DEG;
  pitch = atan2f(-a.acceleration.x, sqrtf(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * RAD_TO_DEG;
  lastMicros = micros();
}

void loop() {
  static uint32_t nextMs = 0;
  if (millis() < nextMs) return;
  nextMs = millis() + PERIOD_MS;

  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  uint32_t now = micros();
  float dt = (now - lastMicros) / 1e6f;
  lastMicros = now;

  float gx = (g.gyro.x - biasX) * RAD_TO_DEG;  // deg/s
  float gy = (g.gyro.y - biasY) * RAD_TO_DEG;
  float gz = (g.gyro.z - biasZ) * RAD_TO_DEG;

  float accRoll = atan2f(a.acceleration.y, a.acceleration.z) * RAD_TO_DEG;
  float accPitch = atan2f(-a.acceleration.x, sqrtf(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * RAD_TO_DEG;

  roll = ALPHA * (roll + gx * dt) + (1 - ALPHA) * accRoll;
  pitch = ALPHA * (pitch + gy * dt) + (1 - ALPHA) * accPitch;
  yaw += gz * dt;  // no magnetometer, so nothing corrects yaw drift
  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  Serial.printf(
      "{\"roll\":%.2f,\"pitch\":%.2f,\"yaw\":%.2f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
      "\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f,\"temp\":%.1f}\n",
      roll, pitch, yaw, a.acceleration.x, a.acceleration.y, a.acceleration.z, gx, gy, gz, t.temperature);
}
