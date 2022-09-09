#include <Arduino.h>
#include "LEDDriver.h"

//Default constructor
LEDDriver::LEDDriver(){
	this->SDI = 17;
	this->CLK = 21;
	this->LE = 20;
}

//Overloaded constructor
LEDDriver::LEDDriver(int SDI, int CLK, int LE, int delayLen){
	this->SDI = SDI;
	this->CLK = CLK;
	this->LE = LE;
}

void LEDDriver::setOutputs(int *inputArray){
  for(int i = 0; i < LED_N; i++){
    digitalWrite(this->SDI, inputArray[i]);
    digitalWrite(this->CLK, HIGH);
    digitalWrite(this->CLK, LOW);
  }
  digitalWrite(this->LE, HIGH);
}
