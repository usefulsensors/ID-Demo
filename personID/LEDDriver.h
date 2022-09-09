#ifndef LEDDriver_h
#define LEDDriver_h

#define LED_N 16

class LEDDriver{
public:
	LEDDriver();
	LEDDriver(int SDI, int CLK, int LE, int microsPeriod);
  void setOutputs(int *inputArray);
  
private:
	int SDI;
	int CLK;
	int LE;
};

#endif
