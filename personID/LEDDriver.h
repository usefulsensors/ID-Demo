#ifndef LEDDriver_h
#define LEDDriver_h

#define LED_N 16

class LEDDriver{
typedef enum{
  DRIVER_STANDBY,DRIVER_SHIFTTOPINS,DRIVER_WRITETOREGISTERS
}DriverStates_t;

typedef enum{
  LED_WRITE_STANDBY,STAGE_BIT,WRITE_BIT
}LEDWriteStates_t;

public:
	LEDDriver();
	LEDDriver(int SDI, int CLK, int LE, int microsPeriod);
  void driverTick();
  void setOutputs(int *inputArray);
  
private:
	int SDI;
	int CLK;
	int LE;
	int delayLen;
  long timestamp;
  DriverStates_t DriverState;
  LEDWriteStates_t LEDWriteState;
  int currentRegister;
  int outputs[LED_N];
};

#endif
