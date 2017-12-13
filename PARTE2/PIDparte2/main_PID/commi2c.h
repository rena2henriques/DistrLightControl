#ifndef COMM_I2C
#define COMM_I2C

#include <Wire.h>
#include "Arduino.h"

// https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

class CommI2C {
private:

  int myAddress;


  LinkedList<int> addrList = LinkedList<int>(); //neighbours list


public:
  //variables
    //flags
    int readADC = 0;
    int checkTurnEnd = 0;
    int ledON = 0;
    int recalibration = 0;
    int consensusFlag = 0; 
    int rpiFlagG = 0;
    String string_consensus;
    String rpiRequest;

  
  //functions
	CommI2C();  //default constructor

  void msgDecoder(int label, int src_addr, String data); //decodes the message received from i2c

  void setMyAddress (int address);  //set arduino's own address in the object
  
  int getAddr(int index);

  int getAddrListSize(); //get the number of neihgbours
  
  int findNodes (); //finds other arduino in the network

  byte send(byte label, byte dest_address, char data[7]);

  void sendToAll(byte label, char data[7]);
 
};

#endif
