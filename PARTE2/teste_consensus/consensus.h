#ifndef CONSENSUSH
#define CONSENSUSH

#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"
#include "commi2c.h"

// https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

class Consensus {
private:

	LinkedList<float> Klist = LinkedList<float>(); // K matrix;
	float o1; // background illumination;
	float c1;
	float q1;
	float L1;
	float a_lux, b_lux;

public:

	Consensus();

	Consensus(float c_, float q_, float ref_, float a_lux_, float b_lux_);

	LinkedList<float> adcToLux(LinkedList<float> *adclist);

	void setKmatrix(LinkedList<float> *adclist, int pwmHigh);


  //void setKmatrix(float adc1, float adc2, float adc3, int pwmHigh);

  void setL1(float L1);
  void setKmatrix_user(LinkedList<float> Klist_);

  void setO ( float o_);

	int consensusIter(int myaddress,  CommI2C *i2c);
  
  LinkedList<float> getKlist();

};

#endif
