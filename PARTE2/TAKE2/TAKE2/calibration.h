#ifndef CALIB
#define CALIB

#include "commi2c.h"

class Calibration {
private:
  CommI2C *i2calib;
  
  int myAddress = -1;

public:
  Calibration();  //default constructor

  Calibration(CommI2C *I2C, int myAddress_); //constructor

  void start_calibration();

  
};

#endif
