#ifndef I2CDriver_h
#define I2CDriver_h

typedef struct __attribute__((packed)) {
  uint8_t confidence;
  uint8_t data[4];
  uint8_t id_confidence;
  int8_t id;
  uint8_t face_on;
} bbox_t;

typedef struct __attribute__((packed)) {
  uint8_t padding[2];
  uint16_t payload_bytes;
} i2c_header_t;

typedef struct __attribute__((packed)) {
  i2c_header_t header;
  uint8_t num_faces;
  bbox_t boxes[4];
  uint16_t checksum;
} inference_results_t;

class I2CDriver{
public:  
  typedef enum{
    MODE_STANDBY, MODE_CONTINUOUS
  } DeviceMode_t;

  I2CDriver();
  void setMode(DeviceMode_t mode);
  void setIdModelEnabled(bool enabled);
  void setDebugMode(bool enabled);
  void setPersistentIds(bool enabled);
  void eraseSavedIds();
  void singleCapture();
  void calibrate(byte id);
  inference_results_t read();

private:
  void write(byte reg, byte contents);

  DeviceMode_t mode;
};

#endif
