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
  
  int pwm = 100;

  int o1 = 0; //background illumin

  //consensus vars
  float c1 = 1;
  float q1 = 0; 
  float L1;

  //lux reference obtained in consensus
  float newref=0;

  //line of K matrix
  LinkedList<float> Klist = LinkedList<float>();
  
  //variables
  unsigned long howLongToWait = 200;
  unsigned long lastTimeItHappened = 0;
  unsigned long howLongItsBeen = 0;

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

  float getRefConsensus();

  void setLowerReference(float newRef);

  float getLowerRef();

<<<<<<< HEAD
=======
  float getKii();


  
>>>>>>> 12f28b9fd4e349d11d0f28964d6148698413c86d
};

#endif
