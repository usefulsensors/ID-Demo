#include <Arduino.h>
#include "LEDDriver.h"

//Default constructor
LEDDriver::LEDDriver(){
	this->SDI = 17;
	this->CLK = 21;
	this->LE = 20;
	this->microsPeriod = 100;
}

//Overloaded constructor
LEDDriver::LEDDriver(int SDI, int CLK, int LE, int microsPeriod){
	this->SDI = SDI;
	this->CLK = CLK;
	this->LE = LE;
	this->microsPeriod = microsPeriod;
}