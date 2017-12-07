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

	LinkedList<float> ADCList = LinkedList<float>();

	int ldrPin, ledPin;

	int n_ack;
	int n_reads = 0;

	int twod_flag=0;


public:
	CommI2C();

	CommI2C(int ldrPin_,int ledPin_);

	void setAddress(int address);

	int findNodes();

	unsigned char getAddr(int i);

	void calibration();

	void msgDecoder(byte last8, byte first8);

	void readADC(int address);

	void checkTurnEnd();

	void ledON();

	byte send(byte address, byte firstByte, byte secondByte);

	void sendToAll(byte firstByte, byte secondByte);

	void checkFlags();

	LinkedList<float>* getADCvalues();

  int getConsensusFlag();


	// Flags
	int sendAck = 0;
	int turnEnd = 0;
	int ledFlag = 0;
	int calibFlag = 0;
	int consensusFlag=0;
  int reconsensusFlag=0;
	
	LinkedList<int> dList = LinkedList<int>();
};

#endif
