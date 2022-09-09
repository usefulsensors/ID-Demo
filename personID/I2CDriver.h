#ifndef I2CDriver_h
#define I2CDriver_h

typedef struct {
  float confidence;
  float id_confidence;
  uint8_t bounding_box[4];
  int8_t identity;
} inference_results_t;

class I2CDriver{
public:  
  typedef enum{
    MODE_STANDBY, MODE_CONTINUOUS
  } DeviceMode_t;

  I2CDriver();
  void setMode(DeviceMode_t mode);
  void setIdModelEnabled(bool enabled);
  void singleCapture();
  void calibrate(byte id);
  inference_results_t read();

private:
  void write(byte reg, byte contents);

  DeviceMode_t mode;
};

#endif
