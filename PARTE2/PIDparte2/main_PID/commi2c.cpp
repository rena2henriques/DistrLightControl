#include "commi2c.h"	

//default constructor
CommI2C::CommI2C() {

}

//set arduino's own address in the object
void CommI2C::setMyAddress (int address) {
   myAddress = address;
}

//decodes the message received from i2c
void CommI2C::msgDecoder(int label, int src_addr, String data){
   
   switch(label) {
      case 1:
          readADC = src_addr; //someone told me to read my ADC          
          break;
      case 2:
          checkTurnEnd = 1;  //received an ack, flag that check if all nodes read my lux
          break;
      case 3:
          ledON = 1;        //its my turn to turn the led on
          break;
      case 4:
          recalibration = 1; //in case of reset or a new node joined
          break;
      case 5:
          consensusFlag = 1;
          string_consensus = data;
          break;
      case 6:
          rpiFlagG = 1;
          rpiRequest = data;
          Serial.print("rpi data = ");
          Serial.println(rpiRequest);
          break;

      case 7:
          rpiFlagS = 1;
          rpiRequest = data;
          break;
      case 8: 
          reconsensus=1;
          break;
      case 9:
          consensusState = -(consensusState - 1);
          break;
      case 10:
          rpiFlagT = 1;
          break; 

   }
 
}

int CommI2C::getAddr(int index) {
  return addrList.get(index);
}

int CommI2C::getAddrListSize() {
  return addrList.size();
}

int CommI2C::findNodes() {

  int error, address;
  if(addrList.size() != 0)
    addrList.clear();  //cleans the list in case of reset
  // the devices have 7-bit I2C addresses 
  for(address = 1; address<10;address++){

      // We use the Write.endTransmisstion return value to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      Wire.write('z');
      error = Wire.endTransmission();

      // The data was send successfully
      if (error == 0) {
        // Inserts the discovered address in the list
      addrList.add(address);
      }
  }
  return addrList.size();
}

byte CommI2C::send(byte label, byte dest_address, char data[7]) {

  Wire.beginTransmission(dest_address);
  Wire.write(label);
  Wire.write(myAddress);
  Wire.write(data);
  return Wire.endTransmission();
}


void CommI2C::sendToAll(byte label, char data[7]) {

  // sends message to all arduinos to read their lux values
  for(int i=0; i < addrList.size(); i++) {
    // requests lux reading
    send(label, addrList.get(i), data);
  }
}

