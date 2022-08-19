#include <Arduino.h>
#include "buzzDriver.h"

//Default constructor
buzzDriver::buzzDriver(){
  this->buzzPin = 6;
  this->period = 255;
  pinMode(this->buzzPin,OUTPUT);
  PORTMUX.TCBROUTEA = 0b00000001; // TCB0 to PF4
  this->DriverState = DRIVER_STANDBY;
}

void buzzDriver::buzzDriverTick(){
  switch(DriverState){
    case DRIVER_STANDBY:{
      
    }
    break;

    case DRIVER_BUZZ:{
      if(millis() - timestamp >= duration){
        TCB0.CTRLB = 0; // Stop PWM output
        TCB0.CTRLA = 0; // Stop TCB peripheral
        DriverState = DRIVER_STANDBY;
      }
    }
    break;
  }
}

void buzzDriver::buzz(int duration, int period){
  this->duration = duration;
  this->period = period;
  TCB0.CCMPL = this->period; // PWM period
  TCB0.CCMPH = 128; // PWM compare/duty cycle
  TCB0.CTRLA = 0b00000101; // Bit 2:1 use TCA0 prescaler, Bit 0 enable TCB
  TCB0.CTRLB = 0b00010111; // Bit 4 pin output enable, Bit 2:0 8-bit PWM mode
  DriverState = DRIVER_BUZZ;
  this->timestamp = millis();
}
