#include <Wire.h>
#include "I2CDriver.h"

static const byte kSlaveAddress = 0x62;
static const byte kModeRegister = 0x01;
static const byte kSingleCaptureRegister = 0x02;
static const byte kCalibrationRegister = 0x03;

I2CDriver::I2CDriver() {
  Wire.begin();
  mode = MODE_STANDBY;
}
void I2CDriver::setMode(DeviceMode_t mode) {
  write(kModeRegister, mode);
}

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
  Wire.requestFrom(kSlaveAddress, sizeof(inference_results_t));
  if (Wire.available() != sizeof(inference_results_t)) {
    return results;
  }
  results.bounding_box[0] = Wire.read();
  results.bounding_box[1] = Wire.read();
  results.bounding_box[2] = Wire.read();
  results.bounding_box[3] = Wire.read();
  results.identity = Wire.read();
  return results;
}
