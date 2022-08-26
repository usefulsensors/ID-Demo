#include <Arduino.h>
#include "LEDDriver.h"

//Default constructor
LEDDriver::LEDDriver(){
	this->SDI = 17;
	this->CLK = 21;
	this->LE = 20;
  this->currentRegister = 0;
	this->delayLen = 1;
  this->DriverState = DRIVER_STANDBY;
  this->LEDWriteState = LED_WRITE_STANDBY;
}

//Overloaded constructor
LEDDriver::LEDDriver(int SDI, int CLK, int LE, int delayLen){
	this->SDI = SDI;
	this->CLK = CLK;
	this->LE = LE;
  this->currentRegister = 0;
	this->delayLen = delayLen;
  this->DriverState = DRIVER_STANDBY;
  this->LEDWriteState = LED_WRITE_STANDBY;
}

void LEDDriver::ledDriverTick(){
  switch(DriverState){
    case DRIVER_STANDBY:{
      
    }
    break;

    case DRIVER_SHIFTTOPINS:{
      if(millis() - this->timestamp >= this->delayLen){
        digitalWrite(this->LE,LOW);
        this->DriverState = DRIVER_STANDBY;
      }
    }
    break;

    case DRIVER_WRITETOREGISTERS:{
      this->currentRegister = 0;
      digitalWrite(this->SDI,this->outputs[this->currentRegister]);
      this->DriverState = DRIVER_STANDBY;
      this->LEDWriteState = STAGE_BIT;
      this->timestamp = millis();
    }
    break;
  }

  switch(LEDWriteState){
    case LED_WRITE_STANDBY:{
      
    }
    break;

    case STAGE_BIT:{
      if(millis() - this->timestamp >= this->delayLen){
        digitalWrite(this->CLK,HIGH);
        this->LEDWriteState = WRITE_BIT;
        this->timestamp = millis();
      }
    }
    break;

    case WRITE_BIT:{
      if(millis() - this->timestamp >= this->delayLen){
        digitalWrite(this->CLK,LOW);
        if(this->currentRegister < LED_N - 1){
          this->currentRegister++;
          digitalWrite(this->SDI,this->outputs[this->currentRegister]);
          this->LEDWriteState = STAGE_BIT;
        }else{
          digitalWrite(this->SDI,LOW);
          digitalWrite(this->LE,HIGH);
          this->DriverState = DRIVER_SHIFTTOPINS;
          this->LEDWriteState = LED_WRITE_STANDBY;
        }
        this->timestamp = millis();
      }
    }
    break;
  }
}

void LEDDriver::setOutputs(int *inputArray){
  for(int i = 0; i < LED_N; i++){
    this->outputs[i] = inputArray[i];
  }
  this->DriverState = DRIVER_WRITETOREGISTERS;
}
