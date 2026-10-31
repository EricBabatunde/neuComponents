// Streams MPU6050 orientation as one JSON object per line at 50 Hz.
// Wiring: VCC-3V3, GND-GND, SDA-GPIO8, SCL-GPIO9 (ESP32-S3 Arduino defaults).
//
// Talks to the registers directly instead of using a library, because many boards sold
// as "MPU6050" carry an MPU-6500 or similar (chip ID 0x70, 0x71, ...). Their accel and gyro
// registers are the same, so one driver covers the whole family.
#include <Arduino.h>
#include <Wire.h>

constexpr uint8_t ADDR = 0x68;           // AD0 low
constexpr uint8_t REG_CONFIG = 0x1A;     // digital low-pass filter
constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
constexpr uint8_t REG_DATA = 0x3B;       // 14 bytes: accel xyz, temp, gyro xyz
constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
constexpr uint8_t REG_WHO_AM_I = 0x75;

constexpr float ACCEL_LSB_PER_G = 8192.0f;   // ±4 g range
constexpr float GYRO_LSB_PER_DPS = 65.5f;    // ±500 °/s range
constexpr float G = 9.80665f;

// Output sign per angle, so the on-screen plane turns the same way as the board.
// Flip one to -1/+1 if that axis moves the wrong way with your mounting.
constexpr float ROLL_SIGN = -1, PITCH_SIGN = -1, YAW_SIGN = -1;

constexpr uint32_t PERIOD_MS = 20;   // 50 Hz
constexpr float ALPHA = 0.98f;       // complementary filter: share given to the gyro
constexpr int CALIBRATION_SAMPLES = 200;

uint8_t chipId = 0;
float roll = 0, pitch = 0, yaw = 0;
float biasX = 0, biasY = 0, biasZ = 0;  // gyro offsets at rest, °/s
uint32_t lastMicros = 0;

struct Reading { float ax, ay, az, gx, gy, gz, temp; };  // m/s², °/s, °C

void writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

int readRegs(uint8_t reg, uint8_t* out, uint8_t len) {
  Wire.beginTransmission(ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  uint8_t n = Wire.requestFrom(ADDR, len);
  for (uint8_t i = 0; i < n; i++) out[i] = Wire.read();
  return n;
}

bool readSensor(Reading& r) {
  uint8_t b[14];
  if (readRegs(REG_DATA, b, 14) != 14) return false;
  // Big-endian signed 16-bit. (Not named `word`: Arduino defines that as a macro.)
  auto s16 = [&](int i) { return (int16_t)((b[i] << 8) | b[i + 1]); };
  r.ax = s16(0) / ACCEL_LSB_PER_G * G;
  r.ay = s16(2) / ACCEL_LSB_PER_G * G;
  r.az = s16(4) / ACCEL_LSB_PER_G * G;
  // The MPU-6050 and the MPU-6500 family use different temperature formulas.
  r.temp = chipId == 0x68 ? s16(6) / 340.0f + 36.53f : s16(6) / 333.87f + 21.0f;
  r.gx = s16(8) / GYRO_LSB_PER_DPS;
  r.gy = s16(10) / GYRO_LSB_PER_DPS;
  r.gz = s16(12) / GYRO_LSB_PER_DPS;
  return true;
}

bool initSensor() {
  if (readRegs(REG_WHO_AM_I, &chipId, 1) != 1) {
    Serial.println("MPU6050 not found at 0x68. Check power, GND, SDA (GPIO8) and SCL (GPIO9).");
    return false;
  }
  writeReg(REG_PWR_MGMT_1, 0x80);  // reset
  delay(100);
  writeReg(REG_PWR_MGMT_1, 0x01);  // wake, clock from gyro PLL
  delay(10);
  writeReg(REG_CONFIG, 0x04);        // ~21 Hz low-pass, smooths vibration
  writeReg(REG_GYRO_CONFIG, 0x08);   // ±500 °/s
  writeReg(REG_ACCEL_CONFIG, 0x08);  // ±4 g
  Serial.printf("Sensor found, chip ID 0x%02X%s\n", chipId,
                chipId == 0x68 ? " (MPU-6050)" : " (MPU-6500 family, compatible)");
  return true;
}

void calibrateGyro() {
  // Average the gyro while the board is still; that average is its offset.
  Reading r;
  int n = 0;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    if (readSensor(r)) {
      biasX += r.gx;
      biasY += r.gy;
      biasZ += r.gz;
      n++;
    }
    delay(10);
  }
  if (n) { biasX /= n; biasY /= n; biasZ /= n; }
}

float accelRoll(const Reading& r) { return atan2f(r.ay, r.az) * RAD_TO_DEG; }
float accelPitch(const Reading& r) { return atan2f(-r.ax, sqrtf(r.ay * r.ay + r.az * r.az)) * RAD_TO_DEG; }

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  while (!initSensor()) delay(1000);

  Serial.println("Calibrating gyro, keep the sensor still...");
  calibrateGyro();

  // Start from the accelerometer's angle so the plane does not swing in from zero.
  Reading r;
  if (readSensor(r)) { roll = accelRoll(r); pitch = accelPitch(r); }
  lastMicros = micros();
}

void loop() {
  static uint32_t nextMs = 0;
  if (millis() < nextMs) return;
  nextMs = millis() + PERIOD_MS;

  Reading r;
  if (!readSensor(r)) {
    Serial.println("Lost contact with the sensor. Check the wires.");
    delay(500);
    return;
  }

  uint32_t now = micros();
  float dt = (now - lastMicros) / 1e6f;
  lastMicros = now;

  float gx = r.gx - biasX, gy = r.gy - biasY, gz = r.gz - biasZ;
  roll = ALPHA * (roll + gx * dt) + (1 - ALPHA) * accelRoll(r);
  pitch = ALPHA * (pitch + gy * dt) + (1 - ALPHA) * accelPitch(r);
  yaw += gz * dt;  // no magnetometer, so nothing corrects yaw drift
  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  Serial.printf(
      "{\"roll\":%.2f,\"pitch\":%.2f,\"yaw\":%.2f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
      "\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f,\"temp\":%.1f}\n",
      ROLL_SIGN * roll, PITCH_SIGN * pitch, YAW_SIGN * yaw, r.ax, r.ay, r.az, gx, gy, gz, r.temp);
}
