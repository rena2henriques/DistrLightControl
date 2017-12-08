#ifndef CALIB
#define CALIB

#include "commi2c.h"

class Calibration {
private:
  CommI2C *i2calib;
  
  int myAddress = -1;
  int ledPin;
  int ldrPin;

  //for adc to lux conversion
  float a_lux;
  float b_lux;
  
  int pwm = 150;

  //line of K matrix
  LinkedList<float> Klist = LinkedList<float>();

public:
  //functions
  Calibration();  //default constructor

  Calibration(CommI2C *I2C, int myAddress_, int ldrPin, int ledPin, float a_lux, float b_lux); //constructor

  void start_calibration();

  void readADCvalue(int readADC);

  float adcToLux(int ADCvalue);

  void ledON();

  void check_TurnEnd (int nacks, int nreads);

  //variables
  int howLongToWait = 50;
  int lastTimeItHappened = 0;
  int howLongItsBeen = 0;

  
};

#endif
