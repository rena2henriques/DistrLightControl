#ifndef COMM_I2C
#define COMM_I2C

#include <Wire.h>
#include "Arduino.h"

// https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

class CommI2C {
private:
  int ldrPin;
  int ledPin;
  int myAddress;

  LinkedList<int> addrList = LinkedList<int>(); //neighbours list


public:
	CommI2C();  //default constructor

	CommI2C(int ldrPin_,int ledPin_); //constructor

  void msgDecoder(int label, int src_addr, String data); //decodes the message received from i2c

  void setMyAddress (int address);  //set arduino's own address in the object

  int getAddr(int index);

  void findNodes (); //finds other arduino in the network
 
};

#endif
