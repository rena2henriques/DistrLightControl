#ifndef COMM_I2C
#define COMM_I2C

#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"

// https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

class CommI2C {
private:

	int myaddress;

	LinkedList<unsigned char> addrList = LinkedList<unsigned char>();

	int ldrPin, ledPin;

	int n_ack;

	int it = 0;
	
public:

	CommI2C();

	CommI2C(int ldrPin_,int ledPin_);

	void setAddress(int address);

	int findNodes();

	unsigned char getAddr(int i);

	void calibration(int myaddress);

	void receiveHandler(int numBytes);

	void msgDecoder(int message);

	void readADC(int address);

	void checkTurnEnd();

	void ledON();
};

#endif