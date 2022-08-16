#ifndef LEDDriver_h
#define LEDDriver_h

class LEDDriver{
public:
	LEDDriver();
	LEDDriver(int SDI, int CLK, int LE, int microsPeriod);
private:
	int SDI;
	int CLK;
	int LE;
	int microsPeriod;
};

#endif

