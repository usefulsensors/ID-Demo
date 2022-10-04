#include <Wire.h>
#include "I2CDriver.h"

static const int kSlaveAddress = 0x62;
static const byte kModeRegister = 0x01;
static const byte kRunIdModelRegister = 0x02;
static const byte kSingleCaptureRegister = 0x03;
static const byte kCalibrationRegister = 0x04;
static const byte kPersistentIdsRegister = 0x05;
static const byte kEraseSavedIdsRegister = 0x06;
static const byte kDebugModeRegister = 0x07;

I2CDriver::I2CDriver() {
  Wire.begin();
  mode = MODE_STANDBY;
}
void I2CDriver::setMode(DeviceMode_t mode) {
  write(kModeRegister, mode);
}

void I2CDriver::setIdModelEnabled(bool enabled) {
  write(kRunIdModelRegister, (byte)enabled);
}

void I2CDriver::setDebugMode(bool enabled) {
  write(kDebugModeRegister, (byte)enabled);
}

void I2CDriver::setPersistentIds(bool enabled) {
  write(kPersistentIdsRegister, (byte)enabled);
}

void I2CDriver::eraseSavedIds() {
  Wire.beginTransmission(kSlaveAddress);
  Wire.write(kEraseSavedIdsRegister);
  Wire.endTransmission();}

void I2CDriver::singleCapture() {
  Wire.beginTransmission(kSlaveAddress);
  Wire.write(kSingleCaptureRegister);
  Wire.endTransmission();
}

void I2CDriver::calibrate(byte id) {
  write(kCalibrationRegister, id);
}

void I2CDriver::write(byte reg, byte contents) {
  Wire.beginTransmission(kSlaveAddress);
  Wire.write(reg);
  Wire.write(contents);
  Wire.endTransmission();
}

inference_results_t I2CDriver::read() {
  inference_results_t results = {};
  i2c_header_t header = {};

  int total_size = sizeof(inference_results_t);
  Wire.requestFrom(kSlaveAddress, total_size);
    if (Wire.available() != total_size) {
    Serial.println("avialable " + String(Wire.available()) + " != requested " + String(total_size));
    return results;
  }
  
  int8_t* results_ptr = (int8_t*)(&results);
  for (int i=0; i<sizeof(inference_results_t); i++) {
    *results_ptr++ = Wire.read();
  }

  return results;
}
