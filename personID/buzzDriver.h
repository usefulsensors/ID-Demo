#ifndef buzzDriver_h
#define buzzDriver_h

class buzzDriver{
typedef enum{
  DRIVER_STANDBY,DRIVER_BUZZ
}DriverStates_t;
  
public:
  buzzDriver();
  void buzzDriverTick();
  void buzz(int duration, int period);

private:
  int buzzPin;
  uint8_t period;
  int duration;
  long timestamp;
  DriverStates_t DriverState;
};

#endif
