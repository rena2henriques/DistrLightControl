#ifndef CONS
#define CONS

#include "commi2c.h"
#include <string.h>

class Consensus {
private:
  CommI2C *i2calib;
  
  int myAddress = -1;
  int ledPin;
  int ldrPin;

  //for adc to lux conversion
  float a_lux;
  float b_lux;
  
  int pwm = 200;

  int o1 = 0; //background illumin

  //consensus vars
  float c1 = 1;
  float q1 = 0; 
  float L1;
  

  //line of K matrix
  LinkedList<float> Klist = LinkedList<float>();

public:
  //functions
  Consensus();  //default constructor

  Consensus(CommI2C *I2C, int ldrPin, int ledPin, float a_lux, float b_lux, float L1_, float c1_, float q1_); //constructor

  void start_calibration();

  void readADCvalue(int readADC);

  float adcToLux(int ADCvalue);

  void ledON();

  void check_TurnEnd (int nacks, int &nreads);

  void cleanCalibVars();

  void setMyAddress(int address_);

  float getExternalIlluminance();

  float consensusIter();

  //variables
  int howLongToWait = 200;
  int lastTimeItHappened = 0;
  int howLongItsBeen = 0;

  
};

#endif
